#pragma once

/**
 * @file cuda_mipmap_array.h
 * @brief CudaMipmapArray — RAII owner of a cudaMipmappedArray_t.
 */

#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check.h>

namespace qualquer::optix {
    /**
     * @brief Owning handle to a CUDA mipmapped array (texture backing storage).
     *
     * Holds the raw GPU array that stores texel data (BC-compressed scene
     * textures, fp16 defaults, or HDR cubemaps). One or more
     * @c cudaTextureObject_t can be created against this array, each with
     * different sampler parameters; the array outlives all of them.
     *
     * Move-only: the array names a single GPU resource; a copy would
     * double-free.
     */
    struct CudaMipmapArray {
        /** @brief Backing mipmapped array; null when empty. */
        cudaMipmappedArray_t handle = nullptr;

        /** @brief Constructs an empty array owning no GPU resources. */
        CudaMipmapArray() = default;

        /** @brief Releases held GPU resources. */
        ~CudaMipmapArray() {
            destroy();
        }

        /** @brief Non-copyable: GPU array has a single owner. */
        CudaMipmapArray(const CudaMipmapArray &) = delete;

        /** @brief Non-copyable: GPU array has a single owner. */
        CudaMipmapArray &operator=(const CudaMipmapArray &) = delete;

        /**
         * @brief Steals another array's resources; leaves other empty.
         * @param other Array to steal from; left empty afterwards.
         */
        CudaMipmapArray(CudaMipmapArray &&other) noexcept : handle(other.handle) {
            other.handle = nullptr;
        }

        /**
         * @brief Releases this array's resources, then steals other's.
         * @param other Array to steal from; left empty afterwards.
         * @return Reference to this array.
         */
        CudaMipmapArray &operator=(CudaMipmapArray &&other) noexcept {
            if (this != &other) {
                destroy();
                handle = other.handle;
                other.handle = nullptr;
            }
            return *this;
        }

        /**
         * @brief Releases the mipmapped array, resets to empty.
         *
         * Idempotent: repeated calls are no-ops. All texture objects
         * referencing this array must be destroyed before calling this.
         */
        void destroy() {
            if (handle != nullptr) {
                CUDA_CHECK(cudaFreeMipmappedArray(handle));
                handle = nullptr;
            }
        }

        /** @return Whether the array holds allocated GPU resources. */
        [[nodiscard]] bool valid() const {
            return handle != nullptr;
        }
    };
} // namespace qualquer::optix
