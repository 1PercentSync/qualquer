#pragma once

/**
 * @file ktx2.h
 * @brief Minimal KTX2 reader/writer for texture and IBL caching.
 *
 * Supports a fixed set of formats (BC5, BC6H UFloat, BC7 SRGB/UNORM,
 * B10G11R11_UFLOAT_PACK32, R16G16_UNORM) for 2D and
 * cubemap textures with mip chains. No supercompression or Basis Universal
 * support.
 */

#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <vector>

namespace qualquer::renderer {

    /**
     * @brief Neutral texture format used by the cache and CUDA upload paths.
     *
     * Decoupled from both the KTX2 on-disk @c vkFormat field and the CUDA
     * channel descriptor: @ref to_vk_format / @ref from_vk_format bridge to
     * KTX2 storage, and the texture module maps this to a
     * @c cudaChannelFormatKind at upload time.
     */
    enum class TextureFormat {
        BC7_UNORM,             ///< BC7, linear (metallic-roughness, occlusion)
        BC7_SRGB,              ///< BC7, gamma-correct (base color, emissive)
        BC5_UNORM,             ///< BC5, RG only (tangent-space normals, Z reconstructed)
        BC6H_UFLOAT,           ///< BC6H, unsigned float (HDR / IBL environment)
        B10G11R11_UFLOAT_PACK32, ///< IBL irradiance cubemap
        R16G16_UNORM,          ///< IBL utility (e.g. filtered importance sampling)
    };

    /** @brief Data read back from a KTX2 file. */
    struct Ktx2Data {
        TextureFormat format;
        uint32_t base_width;
        uint32_t base_height;
        uint32_t face_count; ///< 1 (2D) or 6 (cubemap).
        uint32_t level_count;

        /** @brief Per-level byte region within blob. */
        struct Level {
            uint64_t offset; ///< Byte offset into blob.
            uint64_t size;   ///< Byte size of this level (all faces).
        };

        std::vector<Level> levels;   ///< levels[0] = base (largest), levels[N-1] = smallest.
        std::vector<uint8_t> blob;   ///< Contiguous mip data (KTX2 metadata stripped).
    };

    /** @brief Per-level data descriptor for write_ktx2(). */
    struct Ktx2WriteLevel {
        const void *data; ///< This level's data (cubemap: all 6 faces concatenated).
        uint64_t size;    ///< Byte size.
    };

    /**
     * @brief Maps a neutral format to the KTX2 @c vkFormat integer.
     * @param format Neutral texture format.
     * @return vkFormat value (matches the Vulkan @c VK_FORMAT_* enum).
     */
    [[nodiscard]] uint32_t to_vk_format(TextureFormat format);

    /**
     * @brief Maps a KTX2 @c vkFormat integer back to a neutral format.
     * @param vk_format vkFormat value read from a KTX2 file.
     * @return Neutral format, or std::nullopt if unsupported.
     */
    [[nodiscard]] std::optional<TextureFormat> from_vk_format(uint32_t vk_format);

    /**
     * @brief Writes a KTX2 file.
     *
     * levels[0] = base level (largest), levels[N-1] = smallest mip. For
     * cubemaps, each level's data must contain all 6 faces contiguous. Mip data
     * is stored smallest-first per the KTX2 spec. The file is written atomically.
     * @param path        Destination file path.
     * @param format      Neutral texture format.
     * @param base_width  Pixel width of the base level.
     * @param base_height Pixel height of the base level.
     * @param face_count  1 (2D) or 6 (cubemap).
     * @param levels      Per-level data, largest first.
     * @return true on success, false on write failure or unsupported format.
     */
    bool write_ktx2(const std::filesystem::path &path,
                    TextureFormat format,
                    uint32_t base_width,
                    uint32_t base_height,
                    uint32_t face_count,
                    std::span<const Ktx2WriteLevel> levels);

    /**
     * @brief Reads a KTX2 file.
     * @param path Source file path.
     * @return Ktx2Data on success, std::nullopt on unsupported format or corrupt file.
     *         The returned blob contains only mip data; Level::offset indexes into it.
     */
    [[nodiscard]] std::optional<Ktx2Data> read_ktx2(const std::filesystem::path &path);

} // namespace qualquer::renderer
