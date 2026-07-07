#pragma once

/**
 * @file equirect_to_cubemap.h
 * @brief Equirectangular-to-cubemap conversion kernel launch entry (app layer).
 */

#include <cstdint>
#include <vector>

namespace qualquer::app {
    /**
     * @brief Converts an equirectangular HDR image to a 6-face cubemap in fp16 RGBA.
     *
     * Uploads the source RGB float32 pixels to device memory, runs a CUDA kernel
     * that maps each cubemap texel to a direction, samples the equirect via
     * bilinear interpolation, clamps to float16 range, and packs RGBA half4.
     * Downloads the result to host memory in face-major layout (all of face 0,
     * then face 1, ..., face 5), matching the input layout expected by
     * compress_texture_bc6h().
     *
     * Face size is derived from the equirect width to match angular resolution:
     * min(bit_ceil(equirect_width / 4), 2048).
     *
     * @param rgb_data       Host pointer to RGB float32 equirect pixels (3 floats/pixel).
     * @param equirect_width  Source image width in pixels.
     * @param equirect_height Source image height in pixels.
     * @param out_face_size   Receives the computed cubemap face edge length.
     * @return Host buffer of fp16 RGBA pixels (4 uint16_t/pixel), 6 faces
     *         contiguous in face-major order. Empty on failure.
     */
    [[nodiscard]] std::vector<uint16_t> equirect_to_cubemap(
        const float *rgb_data,
        uint32_t equirect_width,
        uint32_t equirect_height,
        uint32_t &out_face_size);
} // namespace qualquer::app
