#pragma once

/**
 * @file material.h
 * @brief Material data structures: Material, GPUGeometryInfo (renderer layer).
 */

#include <cstddef>
#include <cstdint>

#include <cuda.h>

#include <glm/glm.hpp>

namespace qualquer::renderer {
    /**
     * @brief Material parameters and texture indices, shared between host and device.
     *
     * The same definition is uploaded to a device buffer and read by the
     * closest-hit shader, so its layout must match on both sides. The texture
     * fields (base_color_tex, etc.) are indices into the device texture-object
     * array populated at load: the shader fetches the cudaTextureObject_t by
     * index, then samples. Missing texture slots are filled with default-texture
     * indices so sampling is unconditional. This is the sole material
     * representation: a geometry's material_id directly indexes the Material
     * array (1:1), and alpha_mode / double_sided live only here.
     */
    struct alignas(16) Material {
        /** @brief glTF baseColorFactor (RGBA). */
        glm::vec4 base_color_factor;

        /** @brief glTF emissiveFactor (xyz; w unused). */
        glm::vec4 emissive_factor;

        /** @brief glTF metallicFactor. */
        float metallic_factor;

        /** @brief glTF roughnessFactor. */
        float roughness_factor;

        /** @brief glTF normalTexture scale. */
        float normal_scale;

        /** @brief glTF occlusionTexture strength. */
        float occlusion_strength;

        /** @brief Index into the texture-object array for the base color map. */
        uint32_t base_color_tex;

        /** @brief Index into the texture-object array for the emissive map. */
        uint32_t emissive_tex;

        /** @brief Index into the texture-object array for the metallic-roughness map. */
        uint32_t metallic_roughness_tex;

        /** @brief Index into the texture-object array for the normal map. */
        uint32_t normal_tex;

        /** @brief Index into the texture-object array for the occlusion map. */
        uint32_t occlusion_tex;

        /** @brief glTF alphaCutoff (Mask-mode discard threshold). */
        float alpha_cutoff;

        /** @brief AlphaMode stored as uint32 (0 = Opaque, 1 = Mask, 2 = Blend). */
        uint32_t alpha_mode;

        /** @brief 1 if glTF doubleSided (disable back-face culling), else 0. */
        uint32_t double_sided;
    };

    static_assert(sizeof(Material) == 80, "Material must stay 80 bytes (host/device layout lock)");
    static_assert(offsetof(Material, base_color_factor) == 0);
    static_assert(offsetof(Material, base_color_tex) == 48);
    static_assert(offsetof(Material, alpha_mode) == 72);

    /**
     * @brief Per-geometry RT query data read by the closest-hit shader.
     *
     * The closest-hit shader reads the entry for the hit geometry, dereferences
     * the device pointers to fetch and interpolate vertex attributes, and uses
     * material_buffer_offset to fetch the matching material. Addresses are
     * stored as CUdeviceptr (the OptiX/CUDA device-pointer form).
     */
    struct GPUGeometryInfo {
        /** @brief Device address of the vertex buffer (Vertex[]). */
        CUdeviceptr vertex_buffer_address;

        /** @brief Device address of the index buffer (uint32_t[]). */
        CUdeviceptr index_buffer_address;

        /**
         * @brief Index into the Material array; filled directly from the
         *        geometry's material_id (1:1 material-to-slot, no indirection).
         */
        uint32_t material_buffer_offset;

        /** @brief Padding to 24 bytes, forced by the 8-byte alignment of the two addresses. */
        uint32_t padding;
    };

    static_assert(sizeof(GPUGeometryInfo) == 24, "GPUGeometryInfo must stay 24 bytes (host/device layout lock)");
    static_assert(offsetof(GPUGeometryInfo, vertex_buffer_address) == 0);
    static_assert(offsetof(GPUGeometryInfo, index_buffer_address) == 8);
    static_assert(offsetof(GPUGeometryInfo, material_buffer_offset) == 16);
} // namespace qualquer::renderer
