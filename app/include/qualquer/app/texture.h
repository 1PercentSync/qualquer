#pragma once

/**
 * @file texture.h
 * @brief Asset preprocessing — image decoding, BC compression, cache lookup (app layer).
 */

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include <qualquer/optix/cuda_texture_upload.h>

namespace qualquer::app {

    using optix::TextureFormat;
    using optix::PreparedTexture;
    using optix::PreparedMipRegion;

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

    /**
     * @brief CPU-side RGB float32 pixel data decoded from an HDR file by stb_image.
     *
     * Always 3 floats per pixel (stbi_loadf is forced to 3 channels). Owns its
     * buffer through a deleter that calls stbi_image_free, so it is move-only.
     */
    struct HdrImageData {
        /** @brief RGB float32 pixel buffer, freed via stbi_image_free; null when invalid. */
        std::unique_ptr<float[], void (*)(void *)> pixels{nullptr, nullptr};

        /** @brief Pixel width. */
        uint32_t width = 0;

        /** @brief Pixel height. */
        uint32_t height = 0;

        /** @return Total bytes (width * height * 3 * sizeof(float)). */
        [[nodiscard]] std::size_t size_bytes() const {
            return static_cast<std::size_t>(width) * height * 3 * sizeof(float);
        }

        /** @return Whether the image holds decoded pixels. */
        [[nodiscard]] bool valid() const { return pixels != nullptr; }
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

    // ---- HDR: image decoding ----

    /**
     * @brief Loads an HDR file (.hdr / .exr) as RGB float32.
     *
     * Forces 3 channels regardless of source format. The returned pixel data is
     * used both by the equirect-to-cubemap conversion and by the env alias table
     * builder.
     * @param path Path to the HDR image file.
     * @return Decoded HDR image, or an invalid one on failure (check with valid()).
     */
    [[nodiscard]] HdrImageData load_hdr_image(const std::filesystem::path &path);

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
     * @return Prepared texture ready for optix::finalize_texture().
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
     * @return Prepared texture ready for optix::finalize_texture().
     */
    [[nodiscard]] PreparedTexture compress_texture_bc6h(
        std::span<const uint16_t> rgba, uint32_t face_size, std::string_view source_hash);

} // namespace qualquer::app
