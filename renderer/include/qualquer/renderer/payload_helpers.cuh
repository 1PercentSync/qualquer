#pragma once

/**
 * @file payload_helpers.cuh
 * @brief 16-register OptiX payload pack/unpack helpers (renderer layer).
 */

// Register layout (see current-phase.md "Payload Layout"):
//   p0-p2   next_origin (float3)
//   p3-p5   next_direction (float3)
//   p6-p8   throughput_update (float3)
//   p9-p11  color (float3) — emissive + NEE radiance
//   p12     hit_distance (float) — negative means miss
//   p13     last_brdf_pdf (float)
//   p14     path sequence_index (uint32) — raygen → closesthit for Sobol
//   p15     bounce index (uint32) — raygen → closesthit (read-only, not written back)

#include <cstdint>

#include <optix_device.h>

namespace qualquer::renderer {
    /** @brief Payload type ID for bounce traces (typed optixTraverse/optixInvoke). */
    constexpr OptixPayloadTypeID kPayloadTypeBounce = OPTIX_PAYLOAD_TYPE_ID_0;

    // ---- Unpacked payload (raygen reads after optixTrace) -----------------------

    /**
     * @brief All 16 payload registers unpacked into named fields.
     *
     * Raygen constructs this from the 16 local uint32_t variables that
     * optixTrace populated. Changing the register layout only requires
     * updating payload_unpack and the closesthit/miss setters below.
     */
    struct PayloadData {
        float3 next_origin; ///< Offset hit position for the next bounce.
        float3 next_direction; ///< BRDF-sampled direction for the next bounce.
        float3 throughput_update; ///< BRDF weight to multiply into path throughput.
        float3 color; ///< Emissive + NEE radiance (miss shader applies env MIS weight).
        float hit_distance; ///< Ray tHit (negative = miss, terminates path).
        float last_brdf_pdf; ///< Combined multi-lobe BRDF PDF (for emissive MIS).
        uint32_t bounce; ///< Current bounce index (passed through payload).
    };

    /**
     * @brief Unpacks 16 payload registers into PayloadData.
     *
     * Called by raygen after optixTrace / optixInvoke returns.
     */
    __forceinline__ __device__ PayloadData payload_unpack(
        const uint32_t p0, const uint32_t p1, const uint32_t p2,
        const uint32_t p3, const uint32_t p4, const uint32_t p5,
        const uint32_t p6, const uint32_t p7, const uint32_t p8,
        const uint32_t p9, const uint32_t p10, const uint32_t p11,
        const uint32_t p12, const uint32_t p13, const uint32_t p14,
        const uint32_t p15) {
        (void) p14;
        (void) p15;
        PayloadData d{};
        d.next_origin = make_float3(__uint_as_float(p0), __uint_as_float(p1), __uint_as_float(p2));
        d.next_direction = make_float3(__uint_as_float(p3), __uint_as_float(p4), __uint_as_float(p5));
        d.throughput_update = make_float3(__uint_as_float(p6), __uint_as_float(p7), __uint_as_float(p8));
        d.color = make_float3(__uint_as_float(p9), __uint_as_float(p10), __uint_as_float(p11));
        d.hit_distance = __uint_as_float(p12);
        d.last_brdf_pdf = __uint_as_float(p13);
        d.bounce = p15;
        return d;
    }

    // ---- Closesthit / miss setters (write individual payload fields) ------------

    /** @brief Writes next_origin into p0-p2. */
    __forceinline__ __device__ void payload_set_next_origin(const float3 v) {
        optixSetPayload_0(__float_as_uint(v.x));
        optixSetPayload_1(__float_as_uint(v.y));
        optixSetPayload_2(__float_as_uint(v.z));
    }

    /** @brief Writes next_direction into p3-p5. */
    __forceinline__ __device__ void payload_set_next_direction(const float3 v) {
        optixSetPayload_3(__float_as_uint(v.x));
        optixSetPayload_4(__float_as_uint(v.y));
        optixSetPayload_5(__float_as_uint(v.z));
    }

    /** @brief Writes throughput_update into p6-p8. */
    __forceinline__ __device__ void payload_set_throughput_update(const float3 v) {
        optixSetPayload_6(__float_as_uint(v.x));
        optixSetPayload_7(__float_as_uint(v.y));
        optixSetPayload_8(__float_as_uint(v.z));
    }

    /** @brief Writes color (emissive + NEE) into p9-p11. */
    __forceinline__ __device__ void payload_set_color(const float3 v) {
        optixSetPayload_9(__float_as_uint(v.x));
        optixSetPayload_10(__float_as_uint(v.y));
        optixSetPayload_11(__float_as_uint(v.z));
    }

    /** @brief Writes hit_distance into p12. Negative signals miss. */
    __forceinline__ __device__ void payload_set_hit_distance(const float v) {
        optixSetPayload_12(__float_as_uint(v));
    }

    /** @brief Writes last_brdf_pdf into p13. */
    __forceinline__ __device__ void payload_set_last_brdf_pdf(const float v) {
        optixSetPayload_13(__float_as_uint(v));
    }

    /** @brief Writes bounce index into p15. */
    __forceinline__ __device__ void payload_set_bounce(const uint32_t v) {
        optixSetPayload_15(v);
    }

    /** @brief Writes path sequence_index into p14 (Sobol; not sample_count). */
    __forceinline__ __device__ void payload_set_sample_index(const uint32_t v) {
        optixSetPayload_14(v);
    }

    // ---- Closesthit / miss getters (read values passed from raygen) -------------

    /** @brief Reads bounce index from p15 (set by raygen before trace). */
    __forceinline__ __device__ uint32_t payload_get_bounce() {
        return optixGetPayload_15();
    }

    /** @brief Reads path sequence_index from p14 (set by raygen before trace). */
    __forceinline__ __device__ uint32_t payload_get_sample_index() {
        return optixGetPayload_14();
    }

    /** @brief Reads last_brdf_pdf from p13 (set by previous closesthit). */
    __forceinline__ __device__ float payload_get_last_brdf_pdf() {
        return __uint_as_float(optixGetPayload_13());
    }
} // namespace qualquer::renderer
