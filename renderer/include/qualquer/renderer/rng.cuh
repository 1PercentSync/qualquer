#pragma once

/**
 * @file rng.cuh
 * @brief Sobol quasi-random sequence with hash decorrelation (renderer layer).
 */

// 128-dimension Sobol low-discrepancy sequence with per-pixel additive
// Cranley-Patterson rotation (pcg_hash) and golden-ratio temporal offset.
// Dimensions >= 128 fall back to xxhash32.
//
// Replaces the Phase 4 PCG-only RNG. Dimension allocation is unchanged so
// call-site dimension indices remain valid.

#include <cstdint>

#include <cuda_runtime.h>

namespace qualquer::renderer {

// ---- Dimension allocation ---------------------------------------------------
//
// Matches the Sobol convention: dims 0-1 are subpixel jitter (x, y);
// each bounce occupies a contiguous block of kDimsPerBounce dimensions
// starting at base = 2 + bounce * kDimsPerBounce.

/** @brief Number of RNG dimensions consumed per bounce. */
constexpr uint32_t kDimsPerBounce = 12;

/** @brief Subpixel jitter X dimension index. */
constexpr uint32_t kDimJitterX = 0;

/** @brief Subpixel jitter Y dimension index. */
constexpr uint32_t kDimJitterY = 1;

// Per-bounce offsets from the bounce base dimension.

/** @brief Lobe selection (specular vs diffuse). */
constexpr uint32_t kBounceOffsetLobeSelect = 0;

/** @brief BRDF sampling random number (first of two). */
constexpr uint32_t kBounceOffsetBrdfXi0 = 1;

/** @brief BRDF sampling random number (second of two). */
constexpr uint32_t kBounceOffsetBrdfXi1 = 2;

/** @brief Russian Roulette survival draw. */
constexpr uint32_t kBounceOffsetRR = 3;

/** @brief First of four env NEE sampling dimensions. */
constexpr uint32_t kBounceOffsetEnvNee = 4;

/** @brief First of four emissive NEE sampling dimensions. */
constexpr uint32_t kBounceOffsetEmissiveNee = 8;

/**
 * @brief Computes the base dimension index for a given bounce.
 *
 * @param bounce Zero-based bounce index.
 * @return First dimension index for this bounce's RNG block.
 */
__forceinline__ __device__ uint32_t bounce_dim_base(const uint32_t bounce) {
    return 2 + bounce * kDimsPerBounce;
}

// ---- Number of Sobol dimensions in the table --------------------------------

/** @brief Sobol table covers dimensions 0..127; higher dimensions use xxhash32. */
constexpr uint32_t kSobolDims = 128;

// ---- PCG hash ---------------------------------------------------------------

/**
 * @brief PCG hash (single-state, XSH-RR variant).
 *
 * Retained for Cranley-Patterson per-pixel offset and stochastic alpha.
 *
 * @param input Seed value.
 * @return Hashed 32-bit value.
 */
__forceinline__ __device__ uint32_t pcg_hash(const uint32_t input) {
    const uint32_t state = input * 747796405u + 2891336453u;
    const uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

// ---- xxhash32 ---------------------------------------------------------------

/**
 * @brief xxHash32 with 96-bit input (three uint32 → one uint32).
 *
 * Higher mixing quality than chained pcg_hash for multi-dimensional fallback.
 * Ported from nvpro_core2 nvshaders/random.h.slang (Cyan4973/xxHash).
 *
 * @param p0 First input word.
 * @param p1 Second input word.
 * @param p2 Third input word.
 * @return Hashed 32-bit value.
 */
__forceinline__ __device__ uint32_t xxhash32(const uint32_t p0,
                                             const uint32_t p1,
                                             const uint32_t p2) {
    constexpr uint32_t PRIME32_2 = 2246822519u;
    constexpr uint32_t PRIME32_3 = 3266489917u;
    constexpr uint32_t PRIME32_4 = 668265263u;
    constexpr uint32_t PRIME32_5 = 374761393u;
    uint32_t h32 = p2 + PRIME32_5 + p0 * PRIME32_3;
    h32 = PRIME32_4 * ((h32 << 17) | (h32 >> 15));
    h32 += p1 * PRIME32_3;
    h32 = PRIME32_4 * ((h32 << 17) | (h32 >> 15));
    h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
    h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
    return h32 ^ (h32 >> 16);
}

// ---- Sobol sequence ---------------------------------------------------------

/**
 * @brief Generates one Sobol sample via the binary representation method.
 *
 * XORs the left-justified direction numbers selected by the set bits of the
 * sample index. The result is a 32-bit integer in [0, 2^32); the caller
 * converts to float after applying scramble offsets.
 *
 * @param directions Pointer to the 4096-entry direction number table
 *                   (params.sobol_directions, in __constant__ memory).
 * @param dim        Sampling dimension in [0, kSobolDims).
 * @param sample_index Zero-based sample index.
 * @return Raw 32-bit Sobol value (before scrambling).
 */
__forceinline__ __device__ uint32_t sobol_sample(const uint32_t *directions,
                                                 const uint32_t dim,
                                                 uint32_t sample_index) {
    uint32_t result = 0;
    const uint32_t offset = dim * 32u;
    for (uint32_t bit = 0; bit < 32u && sample_index != 0u; ++bit) {
        if ((sample_index & 1u) != 0u) {
            result ^= directions[offset + bit];
        }
        sample_index >>= 1u;
    }
    return result;
}

// ---- Sobol RNG entry point --------------------------------------------------

/**
 * @brief Generates a decorrelated quasi-random sample for path tracing.
 *
 * For dim < 128: Sobol sequence + additive Cranley-Patterson rotation (per-pixel
 * pcg_hash offset) + golden-ratio temporal offset. The additive shift preserves
 * Sobol low-discrepancy (XOR would break the stratification structure).
 *
 * For dim >= 128: falls back to xxhash32 (96-bit multi-dimensional mixing).
 *
 * @param directions     Pointer to sobol_directions in LaunchParams (__constant__).
 * @param pixel_index    Linear pixel index (y * width + x).
 * @param sequence_index Sample index driving the Sobol sequence. For jitter dims
 *                       (0-1) this is frame_index (D37 per-frame jitter); for
 *                       BRDF/NEE dims (2+) this is the cumulative sample_index.
 * @param frame_index    Monotonic frame counter for temporal decorrelation.
 * @param dimension      RNG dimension (see dimension allocation constants).
 * @return Uniform float in [0, 1).
 */
__forceinline__ __device__ float sobol_rng(const uint32_t *directions,
                                           const uint32_t pixel_index,
                                           const uint32_t sequence_index,
                                           const uint32_t frame_index,
                                           const uint32_t dimension) {
    if (dimension >= kSobolDims) {
        const uint32_t h = xxhash32(pixel_index, sequence_index, dimension);
        return static_cast<float>(h >> 8) * 0x1p-24f;
    }
    uint32_t sobol_val = sobol_sample(directions, dimension, sequence_index);
    // Cranley-Patterson rotation: additive shift preserves low-discrepancy.
    const uint32_t pixel_offset = pcg_hash(pixel_index * 0x1f1f1f1fu ^ dimension);
    // Golden-ratio temporal offset (2654435769 ≈ φ × 2^32).
    const uint32_t temporal_offset = frame_index * 2654435769u;
    sobol_val += pixel_offset + temporal_offset;
    return static_cast<float>(sobol_val) / static_cast<float>(0xFFFFFFFFu);
}

} // namespace qualquer::renderer
