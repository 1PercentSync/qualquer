#pragma once

/**
 * @file cuda_texture.h
 * @brief CudaTexture — RAII owner of a mipmapped array and its texture objects.
 */

#include <algorithm>
#include <vector>

#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check.h>

namespace qualquer::optix {
    /**
     * @brief Owning handle to a CUDA mipmapped array and its texture objects.
     *
     * Bundles a single @c cudaMipmappedArray_t (the texel data) with one or
     * more @c cudaTextureObject_t (sampler configurations). Multiple texture
     * objects are needed when different glTF samplers reference the same
     * image — each bakes a distinct filter/wrap combination into a lightweight
     * GPU handle while the backing array (the expensive allocation) is shared.
     *
     * Move-only: the array and texture objects name single GPU resources; a
     * copy would double-destroy.
     */
    struct CudaTexture {
        /** @brief Backing mipmapped array; null when empty. */
        cudaMipmappedArray_t array = nullptr;

        /** @brief Texture objects created against the array. */
        std::vector<cudaTextureObject_t> texture_objects;

        /** @brief Constructs an empty texture owning no GPU resources. */
        CudaTexture() = default;

        /** @brief Releases held GPU resources. */
        ~CudaTexture() {
            destroy();
        }

        /** @brief Non-copyable: GPU resources have a single owner. */
        CudaTexture(const CudaTexture &) = delete;

        /** @brief Non-copyable: GPU resources have a single owner. */
        CudaTexture &operator=(const CudaTexture &) = delete;

        /**
         * @brief Steals another texture's resources; leaves other empty.
         * @param other Texture to steal from; left empty afterwards.
         */
        CudaTexture(CudaTexture &&other) noexcept : array(other.array),
                                                    texture_objects(std::move(other.texture_objects)) {
            other.array = nullptr;
        }

        /**
         * @brief Releases this texture's resources, then steals other's.
         * @param other Texture to steal from; left empty afterwards.
         * @return Reference to this texture.
         */
        CudaTexture &operator=(CudaTexture &&other) noexcept {
            if (this != &other) {
                destroy();
                array = other.array;
                texture_objects = std::move(other.texture_objects);
                other.array = nullptr;
            }
            return *this;
        }

        /**
         * @brief Releases all texture objects and the backing array.
         *
         * Idempotent: repeated calls are no-ops. Texture objects are
         * destroyed before the backing array they reference.
         */
        void destroy() {
            // Deduplicate before destroying: (min, mag) pairs may share a
            // handle when both filters are identical.
            std::sort(texture_objects.begin(), texture_objects.end());
            texture_objects.erase(
                std::unique(texture_objects.begin(), texture_objects.end()),
                texture_objects.end());
            for (const auto obj : texture_objects) {
                if (obj != 0) {
                    CUDA_CHECK(cudaDestroyTextureObject(obj));
                }
            }
            texture_objects.clear();
            if (array != nullptr) {
                CUDA_CHECK(cudaFreeMipmappedArray(array));
                array = nullptr;
            }
        }

        /** @return Whether the texture holds an allocated GPU array. */
        [[nodiscard]] bool valid() const {
            return array != nullptr;
        }
    };
} // namespace qualquer::optix
