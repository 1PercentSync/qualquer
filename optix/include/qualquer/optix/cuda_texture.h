#pragma once

/**
 * @file cuda_texture.h
 * @brief CudaTexture — RAII owner of a cudaTextureObject_t.
 */

#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check.h>

namespace qualquer::optix {
    /**
     * @brief Owning handle to a CUDA texture object (sampler configuration).
     *
     * Wraps the @c cudaTextureObject_t that device code samples through.
     * The backing @c cudaMipmappedArray_t is owned separately by a
     * @c CudaMipmapArray; this struct only owns the texture object's
     * lifecycle. The array must outlive all texture objects referencing it.
     *
     * Move-only: the texture object names a single GPU resource; a copy
     * would double-destroy.
     */
    struct CudaTexture {
        /** @brief Sampler-facing texture object; 0 when empty. */
        cudaTextureObject_t texture_object = 0;

        /** @brief Constructs an empty texture owning no GPU resources. */
        CudaTexture() = default;

        /** @brief Releases held GPU resources. */
        ~CudaTexture() {
            destroy();
        }

        /** @brief Non-copyable: GPU texture object has a single owner. */
        CudaTexture(const CudaTexture &) = delete;

        /** @brief Non-copyable: GPU texture object has a single owner. */
        CudaTexture &operator=(const CudaTexture &) = delete;

        /**
         * @brief Steals another texture's resources; leaves other empty.
         * @param other Texture to steal from; left empty afterwards.
         */
        CudaTexture(CudaTexture &&other) noexcept : texture_object(other.texture_object) {
            other.texture_object = 0;
        }

        /**
         * @brief Releases this texture's resources, then steals other's.
         * @param other Texture to steal from; left empty afterwards.
         * @return Reference to this texture.
         */
        CudaTexture &operator=(CudaTexture &&other) noexcept {
            if (this != &other) {
                destroy();
                texture_object = other.texture_object;
                other.texture_object = 0;
            }
            return *this;
        }

        /**
         * @brief Releases the texture object, resets to empty.
         *
         * Idempotent: repeated calls are no-ops.
         */
        void destroy() {
            if (texture_object != 0) {
                CUDA_CHECK(cudaDestroyTextureObject(texture_object));
                texture_object = 0;
            }
        }

        /** @return Whether the texture holds a GPU resource. */
        [[nodiscard]] bool valid() const {
            return texture_object != 0;
        }
    };
} // namespace qualquer::optix
