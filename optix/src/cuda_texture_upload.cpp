/**
 * @file cuda_texture_upload.cpp
 * @brief CUDA texture resource creation implementation (optix layer).
 */

#include <qualquer/optix/cuda_texture_upload.h>

#include <qualquer/optix/cuda_check.h>

#include <cuda_fp16.h>

#include <cassert>

namespace qualquer::optix {
    namespace {
        /// Maps TextureFormat to the native CUDA BC channel descriptor.
        cudaChannelFormatDesc channel_desc_for_format(const TextureFormat format) {
            switch (format) {
                case TextureFormat::BC5_UNORM:
                    return cudaCreateChannelDesc(8, 8, 0, 0,
                                                 cudaChannelFormatKindUnsignedBlockCompressed5);
                case TextureFormat::BC6H_UFLOAT:
                    return cudaCreateChannelDesc(16, 16, 16, 0,
                                                 cudaChannelFormatKindUnsignedBlockCompressed6H);
                case TextureFormat::BC7_SRGB:
                    // sRGB→linear conversion is requested via cudaTextureDesc::sRGB
                    // at texture object creation, not through the channel format.
                    // cudaChannelFormatKindUnsignedBlockCompressed7SRGB is broken
                    // in CUDA 13.x (rejects all readMode combinations).
                    [[fallthrough]];
                case TextureFormat::BC7_UNORM:
                    return cudaCreateChannelDesc(8, 8, 8, 8,
                                                 cudaChannelFormatKindUnsignedBlockCompressed7);
            }
            // ReSharper disable once CppDFAUnreachableCode
            return cudaCreateChannelDesc(8, 8, 8, 8,
                                         cudaChannelFormatKindUnsignedBlockCompressed7);
        }

        /// Number of 4x4 BC blocks covering a pixel dimension.
        uint32_t block_count(const uint32_t pixels) {
            return (pixels + 3u) / 4u;
        }

        /**
         * @brief Creates a 1×1 fp16×4 solid-color CudaTexture with one texture object.
         * @return CudaTexture owning the array and a single texture object.
         */
        CudaTexture create_solid_texture(
            const float r, const float g, const float b, const float a) {
            const auto channel_desc = cudaCreateChannelDesc(
                16, 16, 16, 16, cudaChannelFormatKindFloat);

            cudaMipmappedArray_t mipmap_array = nullptr;
            CUDA_CHECK(cudaMallocMipmappedArray(
                &mipmap_array, &channel_desc, make_cudaExtent(1, 1, 0), 1, 0));

            cudaArray_t level_array = nullptr;
            CUDA_CHECK(cudaGetMipmappedArrayLevel(&level_array, mipmap_array, 0));

            // fp16 RGBA: 4 channels × 2 bytes = 8 bytes per texel. The source
            // values (0/0.5/1.0) are all exactly representable in half precision.
            const __half pixels_data[4] = {
                __float2half(r), __float2half(g), __float2half(b), __float2half(a)};
            CUDA_CHECK(cudaMemcpy2DToArray(
                level_array, 0, 0,
                pixels_data,
                8, // srcPitch (1 pixel × 4 halves × 2 bytes)
                8, // width in bytes
                1, // height in rows
                cudaMemcpyHostToDevice));

            // Create the texture object directly (not via the BC-format path
            // in upload_texture) because default textures use raw fp16 with
            // cudaReadModeElementType.
            cudaResourceDesc res_desc = {};
            res_desc.resType = cudaResourceTypeMipmappedArray;
            res_desc.res.mipmap.mipmap = mipmap_array;

            cudaTextureDesc tex_desc = {};
            tex_desc.addressMode[0] = cudaAddressModeClamp;
            tex_desc.addressMode[1] = cudaAddressModeClamp;
            tex_desc.addressMode[2] = cudaAddressModeClamp;
            tex_desc.filterMode = cudaFilterModePoint;
            tex_desc.mipmapFilterMode = cudaFilterModePoint;
            tex_desc.readMode = cudaReadModeElementType;
            tex_desc.normalizedCoords = 1;

            cudaTextureObject_t texture_object = 0;
            CUDA_CHECK(cudaCreateTextureObject(
                &texture_object, &res_desc, &tex_desc, nullptr));

            CudaTexture result;
            result.array = mipmap_array;
            result.texture_objects.push_back(texture_object);
            return result;
        }
    } // namespace

    CudaTexture upload_texture(
        const PreparedTexture &prepared,
        std::span<const SamplerDesc> samplers) {
        assert(!samplers.empty() && "upload_texture requires at least one sampler");
        auto channel_desc = channel_desc_for_format(prepared.format);
        const bool is_cubemap = (prepared.face_count == 6);

        // BC5/BC6H/BC7 blocks are all 4x4 texels, 16 bytes each.

        // Allocate mipmapped array (extent in texels, base level).
        const auto base_width = prepared.regions[0].width;
        const auto base_height = prepared.regions[0].height;
        const cudaExtent extent = is_cubemap
                                      ? make_cudaExtent(base_width, base_height, 6)
                                      : make_cudaExtent(base_width, base_height, 0);
        const unsigned int flags = is_cubemap ? cudaArrayCubemap : 0;

        cudaMipmappedArray_t mipmap_array = nullptr;
        CUDA_CHECK(
            cudaMallocMipmappedArray(&mipmap_array, &channel_desc, extent, prepared.level_count, flags));

        // Upload compressed data per level.
        for (uint32_t level = 0; level < prepared.level_count; ++level) {
            constexpr size_t kBcBlockBytes = 16;
            cudaArray_t level_array = nullptr;
            CUDA_CHECK(cudaGetMipmappedArrayLevel(&level_array, mipmap_array, level));

            const auto &region = prepared.regions[level];
            const uint32_t bx = block_count(region.width);
            const uint32_t by = block_count(region.height);
            const size_t row_bytes = static_cast<size_t>(bx) * kBcBlockBytes;
            const auto *src = prepared.data.data() + region.buffer_offset;

            if (is_cubemap) {
                // Face-major data layout aligns with cudaPitchedPtr z-stride
                // (z-stride = ysize * pitch = by * row_bytes = face_bytes).
                cudaMemcpy3DParms params = {};
                params.srcPtr = make_cudaPitchedPtr(
                    const_cast<uint8_t *>(src),
                    row_bytes,
                    row_bytes,
                    by);
                params.dstArray = level_array;
                params.extent = make_cudaExtent(region.width, region.height, 6);
                params.kind = cudaMemcpyHostToDevice;
                CUDA_CHECK(cudaMemcpy3D(&params));
            } else {
                CUDA_CHECK(cudaMemcpy2DToArray(
                    level_array,
                    0, 0,
                    src,
                    row_bytes,
                    row_bytes,
                    by,
                    cudaMemcpyHostToDevice));
            }
        }

        // Derive texture object parameters from the format.
        const cudaTextureReadMode read_mode =
            (prepared.format == TextureFormat::BC6H_UFLOAT)
                ? cudaReadModeElementType
                : cudaReadModeNormalizedFloat;
        const int srgb = (prepared.format == TextureFormat::BC7_SRGB) ? 1 : 0;

        // Create texture objects.
        CudaTexture result;
        result.array = mipmap_array;
        result.texture_objects.reserve(samplers.size());

        cudaResourceDesc res_desc = {};
        res_desc.resType = cudaResourceTypeMipmappedArray;
        res_desc.res.mipmap.mipmap = mipmap_array;

        for (const auto &sampler : samplers) {
            cudaTextureDesc tex_desc = {};
            tex_desc.addressMode[0] = sampler.address_mode_u;
            tex_desc.addressMode[1] = sampler.address_mode_v;
            tex_desc.addressMode[2] = sampler.address_mode_u; // W axis: reuse U (no glTF W wrap)
            tex_desc.filterMode = sampler.filter_mode;
            tex_desc.mipmapFilterMode = sampler.mipmap_filter_mode;
            // BC UNORM formats (BC5/BC7) decompress to 8-bit integers; linear
            // filtering requires cudaReadModeNormalizedFloat for integer types.
            // BC6H decompresses to half-float and must use cudaReadModeElementType.
            tex_desc.readMode = read_mode;
            tex_desc.sRGB = srgb;
            tex_desc.normalizedCoords = 1;
            tex_desc.maxMipmapLevelClamp = static_cast<float>(prepared.level_count - 1);
            tex_desc.seamlessCubemap = is_cubemap ? 1 : 0;

            cudaTextureObject_t texture_object = 0;
            CUDA_CHECK(cudaCreateTextureObject(
                &texture_object, &res_desc, &tex_desc, nullptr));
            result.texture_objects.push_back(texture_object);
        }

        return result;
    }

    DefaultTextures create_default_textures() {
        return DefaultTextures{
            .white = create_solid_texture(1.0f, 1.0f, 1.0f, 1.0f),
            .flat_normal = create_solid_texture(0.5f, 0.5f, 1.0f, 1.0f),
        };
    }
} // namespace qualquer::optix
