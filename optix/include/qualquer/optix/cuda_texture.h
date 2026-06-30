#pragma once

/**
 * @file cuda_texture.h
 * @brief CudaTexture — RAII handle to a CUDA mipmapped array + texture object.
 */

#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check.h>

namespace qualquer::optix {
    /**
     * @brief Owning handle to a single CUDA texture's GPU resources.
     *
     * Pairs a @c cudaMipmappedArray_t (the backing storage, BC-compressed for
     * authored textures or R8G8B8A8 for 1x1 defaults) with the
     * @c cudaTextureObject_t that devices sample through. Creation is performed
     * by the renderer layer (BC upload + @c cudaCreateTextureObject); this
     * struct only owns the lifecycle of whatever was created.
     *
     * Move-only: both handles name single GPU resources and a copy would alias
     * one resource under two lifetimes, leading to double-destroy.
     */
    struct CudaTexture {
        /** @brief Backing mipmapped array; null when empty. */
        cudaMipmappedArray_t mipmap_array = nullptr;

        /** @brief Sampler-facing texture object; 0 when empty. */
        cudaTextureObject_t texture_object = 0;

        /** @brief Constructs an empty texture owning no GPU resources. */
        CudaTexture() = default;

        /** @brief Releases held GPU resources. */
        ~CudaTexture() { destroy(); }

        CudaTexture(const CudaTexture &) = delete;

        CudaTexture &operator=(const CudaTexture &) = delete;

        /**
         * @brief Steals another texture's resources; leaves other empty.
         * @param other Texture to steal from; left empty afterwards.
         */
        CudaTexture(CudaTexture &&other) noexcept
            : mipmap_array(other.mipmap_array), texture_object(other.texture_object) {
            other.mipmap_array = nullptr;
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
                mipmap_array = other.mipmap_array;
                texture_object = other.texture_object;
                other.mipmap_array = nullptr;
                other.texture_object = 0;
            }
            return *this;
        }

        /**
         * @brief Releases the mipmapped array and texture object, resets to empty.
         *
         * Idempotent: members are reset, so a repeat call is a no-op (matches
         * the optix layer's destroy/release convention). The texture object is
         * destroyed before the array it references.
         */
        void destroy() {
            if (texture_object != 0) {
                CUDA_CHECK(cudaDestroyTextureObject(texture_object));
                texture_object = 0;
            }
            if (mipmap_array != nullptr) {
                CUDA_CHECK(cudaFreeMipmappedArray(mipmap_array));
                mipmap_array = nullptr;
            }
        }

        /** @return Whether the texture holds any GPU resources. */
        [[nodiscard]] bool valid() const { return texture_object != 0; }
    };
} // namespace qualquer::optix
