/**
 * @file texture.cpp
 * @brief Renderer layer — texture module implementation.
 */

#include <qualquer/renderer/texture.h>

#include <qualquer/renderer/cache.h>
#include <qualquer/renderer/ktx2.h>

#include <qualquer/optix/cuda_check.h>

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <mutex>
#include <vector>

#include <bc7e_ispc.h>
#include <qualquer/bc7enc/rgbcx.h>
#include <qualquer/ispc_texcomp/ispc_texcomp.h>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <stb_image_resize2.h>

namespace qualquer::renderer {
    // ---- image decoding ----

    ImageData load_image(const std::filesystem::path &path) {
        const auto name = path.string();
        int w = 0;
        int h = 0;
        int channels = 0;
        auto *raw = stbi_load(name.c_str(), &w, &h, &channels, 4);
        if (!raw) {
            spdlog::error("Failed to load image '{}': {}", name, stbi_failure_reason());
            return {};
        }
        return {
            .pixels = {raw, stbi_image_free},
            .width = static_cast<uint32_t>(w),
            .height = static_cast<uint32_t>(h),
        };
    }

    ImageData load_image_from_memory(const uint8_t *buffer, const std::size_t byte_length) {
        int w = 0;
        int h = 0;
        int channels = 0;
        auto *raw = stbi_load_from_memory(buffer, static_cast<int>(byte_length),
                                          &w, &h, &channels, 4);
        if (!raw) {
            spdlog::error("Failed to decode image from memory: {}", stbi_failure_reason());
            return {};
        }
        return {
            .pixels = {raw, stbi_image_free},
            .width = static_cast<uint32_t>(w),
            .height = static_cast<uint32_t>(h),
        };
    }

    // ---- BC encoder one-time init ----

    void ensure_bc_init() {
        static std::once_flag flag;
        std::call_once(flag, [] {
            ispc::bc7e_compress_block_init();
            rgbcx::init(rgbcx::bc1_approx_mode::cBC1Ideal);
        });
    }

    // ---- internal helpers ----

    namespace {
        /// Rounds up to the next multiple of 4 (BC block alignment).
        uint32_t align4(const uint32_t v) { return (v + 3u) & ~3u; }

        /// Number of 4x4 BC blocks covering a pixel dimension.
        uint32_t block_count(const uint32_t pixels) { return (pixels + 3u) / 4u; }

        /// One RGBA8 level of a CPU mip chain.
        struct CpuMipLevel {
            std::vector<uint8_t> data;
            uint32_t width = 0;
            uint32_t height = 0;
        };

        /**
         * Generates a full RGBA8 mip chain. Level 0 is the source resized to
         * 4-aligned dimensions (BC blocks are 4x4); when @p srgb is true the
         * filter operates in linear space to avoid darkening in lower mips.
         */
        std::vector<CpuMipLevel> generate_cpu_mip_chain(
            const uint8_t *src, const uint32_t w, const uint32_t h, const bool srgb) {
            const uint32_t aw = align4(w);
            const uint32_t ah = align4(h);
            const uint32_t level_count = static_cast<uint32_t>(
                                             std::floor(std::log2(static_cast<double>(std::max(aw, ah)))))
                                         + 1u;

            // stbir's srgb variant decodes (gamma expand) -> filters -> encodes
            // (gamma compress); the linear variant treats samples as raw values.
            const auto resize = srgb ? stbir_resize_uint8_srgb : stbir_resize_uint8_linear;

            std::vector<CpuMipLevel> levels(level_count);

            levels[0].width = aw;
            levels[0].height = ah;
            levels[0].data.resize(static_cast<size_t>(aw) * ah * 4u);
            if (aw != w || ah != h) {
                resize(src,
                       static_cast<int>(w),
                       static_cast<int>(h),
                       0,
                       levels[0].data.data(),
                       static_cast<int>(aw),
                       static_cast<int>(ah),
                       0,
                       STBIR_RGBA);
            } else {
                std::memcpy(levels[0].data.data(), src, levels[0].data.size());
            }

            for (uint32_t i = 1; i < level_count; ++i) {
                const uint32_t pw = levels[i - 1].width;
                const uint32_t ph = levels[i - 1].height;
                const uint32_t nw = std::max(1u, pw / 2u);
                const uint32_t nh = std::max(1u, ph / 2u);
                levels[i].width = nw;
                levels[i].height = nh;
                levels[i].data.resize(static_cast<size_t>(nw) * nh * 4u);
                resize(levels[i - 1].data.data(),
                       static_cast<int>(pw),
                       static_cast<int>(ph),
                       0,
                       levels[i].data.data(),
                       static_cast<int>(nw),
                       static_cast<int>(nh),
                       0,
                       STBIR_RGBA);
            }
            return levels;
        }

        /// Extracts a 4x4 pixel block from RGBA8 data, zero-padding any
        /// texels that fall outside the image bounds.
        void extract_block(const uint8_t *src, const uint32_t img_w, const uint32_t img_h,
                           const uint32_t bx, const uint32_t by, uint8_t out[64]) {
            for (uint32_t y = 0; y < 4; ++y) {
                for (uint32_t x = 0; x < 4; ++x) {
                    const uint32_t px = bx * 4u + x;
                    const uint32_t py = by * 4u + y;
                    const size_t dst_off = (static_cast<size_t>(y) * 4u + x) * 4u;
                    if (px < img_w && py < img_h) {
                        const size_t src_off = (static_cast<size_t>(py) * img_w + px) * 4u;
                        std::memcpy(out + dst_off, src + src_off, 4);
                    } else {
                        std::memset(out + dst_off, 0, 4);
                    }
                }
            }
        }

        /// Compresses one RGBA8 mip level to BC7 via bc7e (ISPC SIMD).
        std::vector<uint8_t> compress_bc7(const uint8_t *rgba, const uint32_t w,
                                          const uint32_t h, const bool perceptual) {
            ispc::bc7e_compress_block_params params{};
#ifdef NDEBUG
            ispc::bc7e_compress_block_params_init_slowest(&params, perceptual);
#else
            ispc::bc7e_compress_block_params_init_slow(&params, perceptual);
#endif
            // Extra refinement over the profile default (1) for higher quality.
            params.m_refinement_passes = 2;

            const uint32_t bx_count = block_count(w);
            const uint32_t by_count = block_count(h);
            const uint32_t total_blocks = bx_count * by_count;

            // bc7e wants all 4x4 blocks packed contiguously as uint32 (one
            // RGBA8 pixel each) so it can gang them across ISPC SIMD lanes.
            std::vector<uint32_t> pixels(static_cast<size_t>(total_blocks) * 16);
            uint8_t tmp[64];
            for (uint32_t by = 0; by < by_count; ++by) {
                for (uint32_t bx = 0; bx < bx_count; ++bx) {
                    extract_block(rgba, w, h, bx, by, tmp);
                    const auto idx = static_cast<size_t>(by) * bx_count + bx;
                    std::memcpy(&pixels[idx * 16], tmp, 64);
                }
            }

            std::vector<uint8_t> out(static_cast<size_t>(total_blocks) * 16);
            ispc::bc7e_compress_blocks(
                total_blocks,
                reinterpret_cast<uint64_t *>(out.data()),
                pixels.data(),
                &params);
            return out;
        }

        /// Compresses one RGBA8 mip level to BC5 (RG channels only) via rgbcx.
        std::vector<uint8_t> compress_bc5(const uint8_t *rgba, const uint32_t w, const uint32_t h) {
            const uint32_t bx_count = block_count(w);
            const uint32_t by_count = block_count(h);
            std::vector<uint8_t> out(static_cast<size_t>(bx_count) * by_count * 16);

            uint8_t block_pixels[64];
            for (uint32_t by = 0; by < by_count; ++by) {
                for (uint32_t bx = 0; bx < bx_count; ++bx) {
                    extract_block(rgba, w, h, bx, by, block_pixels);
                    // BC5 = two BC4 blocks; chan0=R(0), chan1=G(1), stride=4.
                    rgbcx::encode_bc5_hq(
                        out.data() + (static_cast<size_t>(by) * bx_count + bx) * 16,
                        block_pixels,
                        0, 1, 4,
                        rgbcx::BC4_DEFAULT_SEARCH_RAD,
                        rgbcx::BC4_USE_ALL_MODES);
                }
            }
            return out;
        }

        /// Compresses one fp16 RGBA face to BC6H via ISPCTextureCompressor.
        std::vector<uint8_t> compress_bc6h(const uint16_t *rgba, const uint32_t w, const uint32_t h) {
            const uint32_t bx_count = block_count(w);
            const uint32_t by_count = block_count(h);
            const uint32_t total_blocks = bx_count * by_count;

            // BC6H input is 64 bits/pixel (fp16 RGBA); stride is in bytes.
            // The ISPC API takes a non-const pointer but only reads the input.
            const rgba_surface surface{
                .ptr = reinterpret_cast<uint8_t *>(const_cast<uint16_t *>(rgba)),
                .width = static_cast<int32_t>(w),
                .height = static_cast<int32_t>(h),
                .stride = static_cast<int32_t>(w) * 8,
            };

            bc6h_enc_settings settings{};
#ifdef NDEBUG
            GetProfile_bc6h_veryslow(&settings);
#else
            GetProfile_bc6h_slow(&settings);
#endif

            std::vector<uint8_t> out(static_cast<size_t>(total_blocks) * 16);
            CompressBlocksBC6H(&surface, out.data(), &settings);
            return out;
        }

        TextureFormat bc_format_for_role(const TextureRole role) {
            switch (role) {
                case TextureRole::Color: return TextureFormat::BC7_SRGB;
                case TextureRole::Linear: return TextureFormat::BC7_UNORM;
                case TextureRole::Normal: return TextureFormat::BC5_UNORM;
            }
            // ReSharper disable once CppDFAUnreachableCode
            return TextureFormat::BC7_UNORM;
        }

        const char *format_suffix(const TextureFormat format) {
            switch (format) {
                case TextureFormat::BC7_SRGB: return "_bc7s";
                case TextureFormat::BC7_UNORM: return "_bc7u";
                case TextureFormat::BC5_UNORM: return "_bc5u";
                case TextureFormat::BC6H_UFLOAT: return "_bc6h";
            }
            // ReSharper disable once CppDFAUnreachableCode
            return "_bc7u";
        }

        /// BC6H is the only cubemap format; all LDR formats are 2D.
        uint32_t expected_face_count(const TextureFormat format) {
            return format == TextureFormat::BC6H_UFLOAT ? 6u : 1u;
        }

        /// Shared cache lookup for both LDR and HDR paths.
        std::optional<PreparedTexture> load_cached(
            const std::string_view source_hash, const TextureFormat format) {
            const auto path = cache_path(
                "textures",
                std::string(source_hash) + format_suffix(format),
                ".ktx2");
            if (!std::filesystem::exists(path)) {
                return std::nullopt;
            }
            auto ktx2 = read_ktx2(path);
            if (!ktx2 || ktx2->format != format
                || ktx2->face_count != expected_face_count(format)) {
                return std::nullopt;
            }

            PreparedTexture result;
            result.format = format;
            result.base_width = ktx2->base_width;
            result.base_height = ktx2->base_height;
            result.face_count = ktx2->face_count;
            result.level_count = ktx2->level_count;
            result.data = std::move(ktx2->blob);
            result.regions.resize(ktx2->level_count);
            for (uint32_t i = 0; i < ktx2->level_count; ++i) {
                result.regions[i] = {
                    .buffer_offset = ktx2->levels[i].offset,
                    .width = std::max(1u, ktx2->base_width >> i),
                    .height = std::max(1u, ktx2->base_height >> i),
                };
            }
            return result;
        }

        /**
         * Assembles a PreparedTexture from per-level compressed buffers and
         * writes the KTX2 cache. Shared tail of the LDR and HDR compress paths.
         */
        PreparedTexture finalize_compressed(
            const TextureFormat format,
            const uint32_t base_width, const uint32_t base_height,
            const uint32_t face_count,
            const std::vector<std::vector<uint8_t> > &levels, // [level] for 2D, or [0]=6 faces for cubemap
            const std::string_view source_hash) {
            const auto level_count = static_cast<uint32_t>(levels.size());

            PreparedTexture result;
            result.format = format;
            result.base_width = base_width;
            result.base_height = base_height;
            result.face_count = face_count;
            result.level_count = level_count;
            result.regions.resize(level_count);

            uint64_t offset = 0;
            for (uint32_t i = 0; i < level_count; ++i) {
                result.regions[i] = {
                    .buffer_offset = offset,
                    .width = std::max(1u, base_width >> i),
                    .height = std::max(1u, base_height >> i),
                };
                offset += levels[i].size();
            }
            result.data.resize(offset);
            for (uint32_t i = 0; i < level_count; ++i) {
                std::memcpy(result.data.data() + result.regions[i].buffer_offset,
                            levels[i].data(), levels[i].size());
            }

            // Best-effort KTX2 cache write.
            const auto path = cache_path(
                "textures",
                std::string(source_hash) + format_suffix(format),
                ".ktx2");
            std::vector<Ktx2WriteLevel> write_levels(level_count);
            for (uint32_t i = 0; i < level_count; ++i) {
                write_levels[i] = {
                    .data = result.data.data() + result.regions[i].buffer_offset,
                    .size = levels[i].size(),
                };
            }
            if (!write_ktx2(path, format, base_width, base_height, face_count, write_levels)) {
                spdlog::warn("Failed to write texture cache: {}", path.string());
            }

            return result;
        }
    } // namespace

    // ---- LDR cache + compress ----

    std::optional<PreparedTexture> load_cached_texture(
        const std::string_view source_hash, const TextureRole role) {
        return load_cached(source_hash, bc_format_for_role(role));
    }

    PreparedTexture compress_texture(const ImageData &data, const TextureRole role,
                                     const std::string_view source_hash) {
        assert(data.valid() && "ImageData must be valid");
        ensure_bc_init();

        const TextureFormat format = bc_format_for_role(role);
        // Color data is mip-filtered in linear space and BC7-encoded with
        // perceptual weighting; linear/normal use raw values.
        const bool is_color = (role == TextureRole::Color);

        const auto mip_chain = generate_cpu_mip_chain(data.pixels.get(),
                                                      data.width,
                                                      data.height,
                                                      is_color);
        const uint32_t base_w = mip_chain[0].width;
        const uint32_t base_h = mip_chain[0].height;

        std::vector<std::vector<uint8_t> > levels(mip_chain.size());
        for (size_t i = 0; i < mip_chain.size(); ++i) {
            const auto &mip = mip_chain[i];
            if (role == TextureRole::Normal) {
                levels[i] = compress_bc5(mip.data.data(), mip.width, mip.height);
            } else {
                levels[i] = compress_bc7(mip.data.data(), mip.width, mip.height, is_color);
            }
        }

        return finalize_compressed(format, base_w, base_h, 1, levels, source_hash);
    }

    // ---- GPU upload ----

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
                case TextureFormat::BC7_UNORM:
                    return cudaCreateChannelDesc(8, 8, 8, 8,
                                                 cudaChannelFormatKindUnsignedBlockCompressed7);
                case TextureFormat::BC7_SRGB:
                    return cudaCreateChannelDesc(8, 8, 8, 8,
                                                 cudaChannelFormatKindUnsignedBlockCompressed7SRGB);
            }
            // ReSharper disable once CppDFAUnreachableCode
            return cudaCreateChannelDesc(8, 8, 8, 8,
                                         cudaChannelFormatKindUnsignedBlockCompressed7);
        }
    } // namespace

    optix::CudaTexture finalize_texture(const PreparedTexture &prepared, const SamplerDesc &sampler) {
        auto channel_desc = channel_desc_for_format(prepared.format);
        const bool is_cubemap = (prepared.face_count == 6);

        // BC5/BC6H/BC7 blocks are all 4x4 texels, 16 bytes each.

        // Allocate mipmapped array (extent in texels).
        const cudaExtent extent = is_cubemap
                                      ? make_cudaExtent(prepared.base_width, prepared.base_height, 6)
                                      : make_cudaExtent(prepared.base_width, prepared.base_height, 0);
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

        // Create texture object.
        cudaResourceDesc res_desc = {};
        res_desc.resType = cudaResourceTypeMipmappedArray;
        res_desc.res.mipmap.mipmap = mipmap_array;

        cudaTextureDesc tex_desc = {};
        tex_desc.addressMode[0] = sampler.address_mode_u;
        tex_desc.addressMode[1] = sampler.address_mode_v;
        tex_desc.addressMode[2] = sampler.address_mode_u; // W axis: reuse U (no glTF W wrap)
        tex_desc.filterMode = sampler.filter_mode;
        tex_desc.mipmapFilterMode = sampler.mipmap_filter_mode;
        tex_desc.readMode = cudaReadModeElementType;
        tex_desc.normalizedCoords = 1;
        tex_desc.maxMipmapLevelClamp = static_cast<float>(prepared.level_count - 1);
        tex_desc.seamlessCubemap = is_cubemap ? 1 : 0;

        cudaTextureObject_t texture_object = 0;
        CUDA_CHECK(cudaCreateTextureObject(
            &texture_object, &res_desc, &tex_desc, nullptr));

        optix::CudaTexture result;
        result.mipmap_array = mipmap_array;
        result.texture_object = texture_object;
        return result;
    }

    // ---- HDR (BC6H) cache + compress, cubemap only ----

    std::optional<PreparedTexture> load_cached_texture_bc6h(const std::string_view source_hash) {
        return load_cached(source_hash, TextureFormat::BC6H_UFLOAT);
    }

    PreparedTexture compress_texture_bc6h(
        const std::span<const uint16_t> rgba, const uint32_t face_size,
        const std::string_view source_hash) {
        const uint64_t face_uint16s = static_cast<uint64_t>(face_size) * face_size * 4u;
        assert(rgba.size() == face_uint16s * 6u &&
            "BC6H cubemap rgba must hold exactly 6 square faces");

        // Base level only: six faces concatenated into a single "level" so
        // finalize_compressed writes one KTX2 level spanning all faces.
        std::vector<uint8_t> base_level;
        const uint32_t blocks_per_face = block_count(face_size) * block_count(face_size);
        base_level.resize(static_cast<size_t>(blocks_per_face) * 16u * 6u);
        for (uint32_t f = 0; f < 6; ++f) {
            auto face_bc6h = compress_bc6h(
                rgba.data() + static_cast<std::size_t>(f) * face_uint16s,
                face_size, face_size);
            const size_t dst = static_cast<size_t>(f) * blocks_per_face * 16u;
            std::memcpy(base_level.data() + dst, face_bc6h.data(), face_bc6h.size());
        }

        std::vector<std::vector<uint8_t> > levels;
        levels.push_back(std::move(base_level));

        return finalize_compressed(
            TextureFormat::BC6H_UFLOAT, face_size, face_size, 6, levels, source_hash);
    }
} // namespace qualquer::renderer
