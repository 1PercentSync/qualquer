#pragma once

/**
 * @file cuda_texture_upload.h
 * @brief CUDA texture resource creation from prepared CPU data (optix layer).
 */

#include <cstdint>
#include <vector>

#include <cuda_runtime.h>

#include <qualquer/optix/cuda_texture.h>

namespace qualquer::optix {

    /**
     * @brief Neutral texture format used by the cache and CUDA upload paths.
     *
     * Decoupled from both the KTX2 on-disk @c vkFormat field and the CUDA
     * channel descriptor: the app layer maps material roles to this enum and
     * the optix layer maps it to @c cudaChannelFormatKind at upload time.
     */
    enum class TextureFormat {
        BC7_UNORM,             ///< BC7, linear (metallic-roughness, occlusion)
        BC7_SRGB,              ///< BC7, gamma-correct (base color, emissive)
        BC5_UNORM,             ///< BC5, RG only (tangent-space normals, Z reconstructed)
        BC6H_UFLOAT,           ///< BC6H, unsigned float (HDR / IBL environment)
    };

    /** @brief One mip level's region within PreparedTexture::data. */
    struct PreparedMipRegion {
        /** @brief Byte offset into PreparedTexture::data. */
        uint64_t buffer_offset = 0;

        /** @brief Pixel width of this level (single face for cubemaps). */
        uint32_t width = 0;

        /** @brief Pixel height of this level (single face for cubemaps). */
        uint32_t height = 0;
    };

    /**
     * @brief CPU-side block-compressed texture ready for GPU upload.
     *
     * Holds compressed mip data in one contiguous buffer plus per-level regions.
     * Produced by the app layer's compress functions (or loaded from cache);
     * consumed by finalize_texture().
     *
     * Layout: levels are stored largest-first in @ref data. For cubemaps (HDR
     * only), each level's region spans all 6 faces concatenated (face-major:
     * all blocks of face 0, then face 1, ...), and width/height describe a
     * single face — the six faces are equal-sized squares.
     */
    struct PreparedTexture {
        /** @brief Compressed format (mapped to the CUDA channel kind at upload). */
        TextureFormat format;

        /** @brief Base-level pixel width (single face for cubemaps). */
        uint32_t base_width = 0;

        /** @brief Base-level pixel height (single face for cubemaps). */
        uint32_t base_height = 0;

        /** @brief 1 (LDR 2D) or 6 (HDR cubemap). */
        uint32_t face_count = 0;

        /** @brief Number of mip levels; 1 for HDR (base level only). */
        uint32_t level_count = 0;

        /** @brief Contiguous compressed mip data, indexed via @ref regions. */
        std::vector<uint8_t> data;

        /** @brief Per-level regions; regions[i] locates level i within @ref data. */
        std::vector<PreparedMipRegion> regions;
    };

    /**
     * @brief Sampler settings for a CUDA texture object.
     *
     * Groups the glTF-derived sampler parameters that a
     * @c cudaTextureObject_t bakes in (CUDA has no separate sampler handle).
     * Callers construct from glTF sampler data; finalize_texture() forwards
     * the values into @c cudaTextureDesc.
     */
    struct SamplerDesc {
        /** @brief Texture filter for magnification and minification. */
        cudaTextureFilterMode filter_mode;

        /** @brief Filter between mip levels (point = no trilinear, linear = trilinear). */
        cudaTextureFilterMode mipmap_filter_mode;

        /** @brief Address mode for the U (S) coordinate. */
        cudaTextureAddressMode address_mode_u;

        /** @brief Address mode for the V (T) coordinate. */
        cudaTextureAddressMode address_mode_v;
    };

    // ---- Default textures ----

    /**
     * @brief Holds the default 1×1 textures for material fallback.
     *
     * Missing material texture slots are filled with these neutral values
     * so the shader can always sample without special-casing.
     */
    struct DefaultTextures {
        /** @brief 1×1 (1,1,1,1) — neutral base color / metallic-roughness / emissive. */
        CudaTexture white;

        /** @brief 1×1 (0.5,0.5,1.0,1.0) — tangent-space Z-up, no perturbation. */
        CudaTexture flat_normal;
    };

    // ---- GPU upload ----

    /**
     * @brief Uploads a PreparedTexture to the GPU and creates a CUDA texture object.
     *
     * Allocates a @c cudaMipmappedArray_t with the native BC channel format,
     * uploads compressed mip data level-by-level, and wraps the result in a
     * @c cudaTextureObject_t. Supports both 2D (LDR, face_count=1) and
     * cubemap (HDR BC6H, face_count=6) textures.
     *
     * @param prepared CPU-side compressed texture from the app layer's
     *                 compress functions or cache.
     * @param sampler  Filter and address mode settings for the texture object.
     * @return RAII handle owning the GPU resources; devices sample via
     *         @c tex2D<float4>() on the contained texture object.
     */
    [[nodiscard]] CudaTexture finalize_texture(
        const PreparedTexture &prepared, const SamplerDesc &sampler);

    /**
     * @brief Creates default 1×1 fp16×4 textures for material fallback.
     *
     * Each texture is a single half-precision (fp16) RGBA texel allocated via
     * @c cudaMallocMipmappedArray (1 level) with @c cudaReadModeElementType,
     * matching BC textures' read mode so @c tex2D<float4>() works uniformly
     * (the hardware reads fp16 and promotes it to float on the texture fetch).
     * Sampler is point-filter + clamp (1×1 has only one texel, so filter/wrap
     * mode has no practical effect).
     *
     * @return DefaultTextures holding RAII CudaTexture handles.
     */
    [[nodiscard]] DefaultTextures create_default_textures();

} // namespace qualquer::optix
