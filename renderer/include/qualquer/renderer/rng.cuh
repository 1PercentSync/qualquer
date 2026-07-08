#pragma once

/**
 * @file rng.cuh
 * @brief PCG hash random number generator (renderer layer).
 */

// PCG hash gives white noise — sufficient for multi-bounce PT convergence.
// Dimension allocation mirrors the Sobol layout so the upgrade to Sobol +
// Blue Noise (low-discrepancy sequences) requires no call-site changes.

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

/** @brief Reserved for Russian Roulette (Phase 4.5). */
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

// ---- PCG hash ---------------------------------------------------------------

/**
 * @brief PCG hash (single-state, XSH-RR variant).
 *
 * @param input Seed value.
 * @return Hashed 32-bit value.
 */
__forceinline__ __device__ uint32_t pcg_hash(const uint32_t input) {
    const uint32_t state = input * 747796405u + 2891336453u;
    const uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

// ---- RNG entry point --------------------------------------------------------

/**
 * @brief Generates a uniform float in [0, 1) from pixel, sample, and dimension.
 *
 * Triple-nested hashing decorrelates the three axes (spatial, temporal,
 * dimensional) without requiring stored state or lookup tables.
 *
 * @param pixel_index  Linear pixel index (y * width + x).
 * @param sample_index Cumulative sample index for the pixel.
 * @param dimension    RNG dimension (see dimension allocation constants).
 * @return Uniform float in [0, 1).
 */
__forceinline__ __device__ float rng(const uint32_t pixel_index,
                                     const uint32_t sample_index,
                                     const uint32_t dimension) {
    const uint32_t seed = pcg_hash(pixel_index ^ pcg_hash(sample_index ^ pcg_hash(dimension)));
    return static_cast<float>(seed) / static_cast<float>(0xFFFFFFFFu);
}

} // namespace qualquer::renderer
