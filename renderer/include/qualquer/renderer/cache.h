#pragma once

/**
 * @file cache.h
 * @brief Shared cache infrastructure: directory, content hashing, and path management.
 */

#include <cstddef>
#include <filesystem>
#include <string>
#include <string_view>

namespace qualquer::renderer {

    /**
     * @brief Returns the cache root directory (%TEMP%\qualquer\).
     *
     * Creates the directory on first call. All cache consumers (textures, IBL)
     * store files under this root.
     * @return Absolute path to the cache root.
     */
    std::filesystem::path cache_root();

    /**
     * @brief Computes the content hash of an in-memory buffer (XXH3_128).
     * @param data Pointer to the bytes to hash; may be null only when size is 0.
     * @param size Number of bytes to hash.
     * @return 32-character lowercase hexadecimal string.
     */
    std::string content_hash(const void *data, std::size_t size);

    /**
     * @brief Computes the content hash of a file by reading its entire contents.
     *
     * Reads the file fully into memory, then hashes it with @ref content_hash.
     * @param file Path to the file to hash.
     * @return 32-character hex string, or an empty string on read failure.
     */
    std::string content_hash(const std::filesystem::path &file);

    /**
     * @brief Builds a cache file path: cache_root() / category / (hash + extension).
     *
     * Creates the category subdirectory if it does not exist. Safe to call from
     * multiple threads; directory creation is tracked to avoid repeated syscalls.
     * @param category  Subdirectory name (e.g. "textures", "ibl").
     * @param hash      Content hash string used as the filename stem.
     * @param extension File extension including the leading dot (e.g. ".ktx2").
     * @return Absolute path to the cache file (not yet created).
     */
    std::filesystem::path cache_path(std::string_view category,
                                     std::string_view hash,
                                     std::string_view extension);

    /**
     * @brief Atomically writes a binary file (write-to-temp + rename).
     *
     * Writes data to @c path.tmp, then renames to @p path. On failure the temp
     * file is removed and @p path is left unchanged (either the previous version
     * or absent). Guarantees the file is either complete or absent — never
     * partially written.
     * @param path Destination file path.
     * @param data Pointer to the bytes to write.
     * @param size Number of bytes to write.
     * @return true on success, false on write or rename failure.
     */
    bool atomic_write_file(const std::filesystem::path &path,
                           const void *data, std::size_t size);

} // namespace qualquer::renderer
