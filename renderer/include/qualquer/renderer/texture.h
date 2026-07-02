#pragma once

/**
 * @file texture.h
 * @brief Renderer layer — texture module.
 */

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include <qualquer/renderer/ktx2.h>

namespace qualquer::renderer {

    /**
     * @brief Tags an LDR texture by material intent, selecting its BC format
     *        and mip-filter color space.
     *
     * Decouples the material's intent for a slot from the concrete format: the
     * caller tags a texture by role, and the texture module maps it to a
     * TextureFormat plus the correct sRGB handling during mip filtering.
     */
    enum class TextureRole {
        Color,  ///< BC7 SRGB — gamma-correct color data (base color, emissive).
        Linear, ///< BC7 UNORM — linear data (metallic-roughness, occlusion).
        Normal, ///< BC5 UNORM — tangent-space normals (RG only, Z reconstructed in shader).
    };

    /**
     * @brief CPU-side RGBA8 pixel data decoded from disk by stb_image.
     *
     * Always 4 bytes per pixel regardless of source channel count (stb_image is
     * forced to RGBA). Owns its buffer through a deleter that calls
     * stbi_image_free, so it is move-only.
     */
    struct ImageData {
        /** @brief RGBA8 pixel buffer, freed via stbi_image_free; null when invalid. */
        std::unique_ptr<uint8_t[], void (*)(void *)> pixels{nullptr, nullptr};

        /** @brief Pixel width. */
        uint32_t width = 0;

        /** @brief Pixel height. */
        uint32_t height = 0;

        /** @return Total bytes (width * height * 4). */
        [[nodiscard]] std::size_t size_bytes() const {
            return static_cast<std::size_t>(width) * height * 4;
        }

        /** @return Whether the image holds decoded pixels. */
        [[nodiscard]] bool valid() const { return pixels != nullptr; }
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
     * Produced by compress_texture() / compress_texture_bc6h() (or loaded from
     * cache by the load_cached_texture* functions); consumed by finalize_texture().
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

    // ---- LDR: image decoding ----

    /**
     * @brief Loads an image file (JPEG/PNG/...) as RGBA8.
     *
     * Forces 4 channels regardless of source format.
     * @param path Path to the image file.
     * @return Decoded image, or an invalid one on failure (check with valid()).
     */
    [[nodiscard]] ImageData load_image(const std::filesystem::path &path);

    /**
     * @brief Decodes an image from an in-memory byte buffer as RGBA8.
     *
     * Forces 4 channels regardless of source format.
     * @param buffer      Pointer to the encoded image bytes.
     * @param byte_length Number of encoded bytes.
     * @return Decoded image, or an invalid one on failure (check with valid()).
     */
    [[nodiscard]] ImageData load_image_from_memory(const uint8_t *buffer, std::size_t byte_length);

    // ---- LDR: BC encoder one-time init ----

    /**
     * @brief Performs one-time initialization of the bc7e/rgbcx encoders.
     *
     * Idempotent (guarded by std::once_flag). compress_texture() calls this
     * internally, so callers need not invoke it; it is exposed only for
     * pre-warming ahead of timed batches. The BC6H path does not use these
     * encoders and does not require initialization.
     */
    void ensure_bc_init();

    // ---- LDR: cache lookup + compression ----

    /**
     * @brief Looks up a cached LDR texture by source hash and role.
     *
     * Does not decode or compress. The cache key is (source_hash + role suffix),
     * so the same source encoded under different roles occupies separate cache
     * entries. Returns nullopt on miss or if the cached format does not match
     * the role.
     * @param source_hash Content hash of the source file bytes.
     * @param role        Texture role selecting the BC format.
     * @return Prepared texture on hit, nullopt on miss.
     */
    [[nodiscard]] std::optional<PreparedTexture> load_cached_texture(
        std::string_view source_hash, TextureRole role);

    /**
     * @brief Generates a CPU mip chain, BC-compresses each level, and writes
     *        the KTX2 cache.
     *
     * Does not check the cache — the caller should have checked via
     * load_cached_texture() first. Color-role textures are mip-filtered in
     * linear space (sRGB-correct) to avoid darkening; Normal uses BC5 (RG only).
     * Ensures encoder initialization internally.
     * @param data        Source RGBA8 pixels (must be valid).
     * @param role        Texture role selecting the BC format and color space.
     * @param source_hash Content hash used as the cache key.
     * @return Prepared texture ready for finalize_texture().
     */
    [[nodiscard]] PreparedTexture compress_texture(
        const ImageData &data, TextureRole role, std::string_view source_hash);

    // ---- HDR (BC6H): cache lookup + compression, cubemap only ----

    /**
     * @brief Looks up a cached BC6H cubemap by source hash.
     *
     * HDR counterpart of load_cached_texture(). The cache key is
     * (source_hash + "_bc6h"). Returns nullopt on miss or if the cached entry
     * is not a BC6H cubemap (face_count != 6).
     * @param source_hash Content hash of the source bytes.
     * @return Prepared texture on hit, nullopt on miss.
     */
    [[nodiscard]] std::optional<PreparedTexture> load_cached_texture_bc6h(
        std::string_view source_hash);

    /**
     * @brief Compresses an fp16 RGBA cubemap to BC6H (base level only) and
     *        writes the KTX2 cache.
     *
     * BC6H serves only IBL environment maps, which are cubemaps; there is no 2D
     * HDR consumer in the project, so 2D input is rejected. No mip chain is
     * generated: pure-PT environment sampling is per-ray point sampling with no
     * rasterization footprint, so mips have no consumer. The six faces must be
     * square and equal-sized.
     * @param rgba        fp16 RGBA pixels, 6 faces laid out contiguously and
     *                    face-major (all of face 0, then face 1, ...).
     * @param face_size   Edge length of one square face, in pixels.
     * @param source_hash Content hash used as the cache key.
     * @return Prepared texture ready for finalize_texture().
     */
    [[nodiscard]] PreparedTexture compress_texture_bc6h(
        std::span<const uint16_t> rgba, uint32_t face_size, std::string_view source_hash);

} // namespace qualquer::renderer
