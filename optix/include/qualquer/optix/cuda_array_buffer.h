#pragma once

/**
 * @file cuda_array_buffer.h
 * @brief CudaArrayBuffer<T> — RAII 2D CUDA array with texture and surface objects.
 */

#include <cstdint>
#include <type_traits>

#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check.h>

namespace qualquer::optix {
    /**
     * @brief Owning handle to a 2D CUDA array backed by both a texture object
     *        (for device-side reading / DLSS consumption) and a surface object
     *        (for device-side surf2Dwrite).
     *
     * Suitable for render-resolution G-buffer channels (depth, normals, albedo,
     * roughness, motion vectors, etc.) that shaders write via surface writes and
     * DLSS reads via texture objects.
     *
     * T controls the default channel layout through cudaCreateChannelDesc<T>():
     *   - float  → R32F  (1 channel)
     *   - float2 → RG32F (2 channels)
     *   - float4 → RGBA32F (4 channels)
     *   - uchar4 → RGBA8 (4 channels, texture reads as normalized float [0,1])
     *
     * The custom alloc overload accepts an explicit cudaChannelFormatDesc +
     * read mode, enabling formats not derivable from T (e.g. half4 via
     * cudaCreateChannelDescHalf4()). resize preserves the format from the
     * most recent alloc.
     *
     * Move-only: the backing cudaArray, texture object, and surface object name
     * single GPU resources; a copy would double-destroy.
     */
    template<typename T>
    class CudaArrayBuffer {
        static_assert(std::is_same_v<T, float> || std::is_same_v<T, float2>
                      || std::is_same_v<T, float4> || std::is_same_v<T, uchar4>,
                      "CudaArrayBuffer<T> supports float, float2, float4, uchar4");

    public:
        /** @brief Constructs an empty buffer owning no resources. */
        CudaArrayBuffer() = default;

        /** @brief Releases held GPU resources. */
        ~CudaArrayBuffer() {
            free();
        }

        CudaArrayBuffer(const CudaArrayBuffer &) = delete;
        CudaArrayBuffer &operator=(const CudaArrayBuffer &) = delete;

        /** @brief Steals another buffer's resources; leaves other empty. */
        CudaArrayBuffer(CudaArrayBuffer &&other) noexcept
            : array_(other.array_), tex_obj_(other.tex_obj_), surf_obj_(other.surf_obj_),
              width_(other.width_), height_(other.height_),
              fmt_desc_(other.fmt_desc_), read_mode_(other.read_mode_) {
            other.array_ = nullptr;
            other.tex_obj_ = 0;
            other.surf_obj_ = 0;
            other.width_ = 0;
            other.height_ = 0;
        }

        /**
         * @brief Releases this buffer's resources, then steals other's.
         * @param other Buffer to steal from; left empty afterwards.
         * @return Reference to this buffer.
         */
        CudaArrayBuffer &operator=(CudaArrayBuffer &&other) noexcept {
            if (this != &other) {
                free();
                array_ = other.array_;
                tex_obj_ = other.tex_obj_;
                surf_obj_ = other.surf_obj_;
                width_ = other.width_;
                height_ = other.height_;
                fmt_desc_ = other.fmt_desc_;
                read_mode_ = other.read_mode_;
                other.array_ = nullptr;
                other.tex_obj_ = 0;
                other.surf_obj_ = 0;
                other.width_ = 0;
                other.height_ = 0;
            }
            return *this;
        }

        /**
         * @brief Allocates a 2D CUDA array and creates the texture/surface objects.
         *
         * Frees any prior allocation first. Width or height of 0 leaves the
         * buffer empty. resize preserves the format from the most recent alloc.
         *
         * @param width  Array width in elements.
         * @param height Array height in elements.
         * @param desc   Channel format descriptor for the backing array.
         * @param mode   Texture read mode (element-type or normalized-float).
         */
        void alloc(uint32_t width, uint32_t height,
                   const cudaChannelFormatDesc &desc,
                   cudaTextureReadMode mode) {
            free();
            if (width == 0 || height == 0) {
                return;
            }

            fmt_desc_ = desc;
            read_mode_ = mode;

            CUDA_CHECK(cudaMallocArray(&array_, &desc, width, height, cudaArraySurfaceLoadStore));

            cudaResourceDesc res_desc{};
            res_desc.resType = cudaResourceTypeArray;
            res_desc.res.array.array = array_;

            cudaTextureDesc tex_desc{};
            tex_desc.addressMode[0] = cudaAddressModeClamp;
            tex_desc.addressMode[1] = cudaAddressModeClamp;
            tex_desc.filterMode = cudaFilterModePoint;
            tex_desc.readMode = mode;
            tex_desc.normalizedCoords = 0;
            CUDA_CHECK(cudaCreateTextureObject(&tex_obj_, &res_desc, &tex_desc, nullptr));

            CUDA_CHECK(cudaCreateSurfaceObject(&surf_obj_, &res_desc));

            width_ = width;
            height_ = height;
        }

        /**
         * @brief Releases the array, texture object, and surface object.
         *
         * Idempotent: members are reset, so a repeat call is a no-op.
         * Objects are destroyed before the backing array they reference.
         * Stored format is preserved so a subsequent resize recreates with
         * the same format.
         */
        void free() {
            if (surf_obj_ != 0) {
                CUDA_CHECK(cudaDestroySurfaceObject(surf_obj_));
                surf_obj_ = 0;
            }
            if (tex_obj_ != 0) {
                CUDA_CHECK(cudaDestroyTextureObject(tex_obj_));
                tex_obj_ = 0;
            }
            if (array_ != nullptr) {
                CUDA_CHECK(cudaFreeArray(array_));
                array_ = nullptr;
            }
            width_ = 0;
            height_ = 0;
        }

        /**
         * @brief Ensures the buffer matches the given dimensions.
         *
         * No-op when the dimensions already match; reallocates with the
         * format from the most recent alloc otherwise.
         * @param width  Desired width.
         * @param height Desired height.
         */
        void resize(uint32_t width, uint32_t height) {
            if (width != width_ || height != height_) {
                alloc(width, height, fmt_desc_, read_mode_);
            }
        }

        /** @brief Texture object for device-side reading (DLSS input). */
        [[nodiscard]] cudaTextureObject_t tex_object() const {
            return tex_obj_;
        }

        /** @brief Surface object for device-side writing (surf2Dwrite). */
        [[nodiscard]] cudaSurfaceObject_t surf_object() const {
            return surf_obj_;
        }

        /** @brief Allocated width in elements. */
        [[nodiscard]] uint32_t width() const {
            return width_;
        }

        /** @brief Allocated height in elements. */
        [[nodiscard]] uint32_t height() const {
            return height_;
        }

        /** @brief Whether the buffer holds allocated GPU resources. */
        [[nodiscard]] bool valid() const {
            return array_ != nullptr;
        }

    private:
        /** @brief Backing 2D CUDA array; null when empty. */
        cudaArray_t array_ = nullptr;

        /** @brief Point-sample texture object for reading; 0 when empty. */
        cudaTextureObject_t tex_obj_ = 0;

        /** @brief Surface object for writing; 0 when empty. */
        cudaSurfaceObject_t surf_obj_ = 0;

        /** @brief Allocated width in elements; 0 when empty. */
        uint32_t width_ = 0;

        /** @brief Allocated height in elements; 0 when empty. */
        uint32_t height_ = 0;

        /** @brief Channel format used by the most recent alloc. */
        cudaChannelFormatDesc fmt_desc_ = {};

        /** @brief Texture read mode used by the most recent alloc. */
        cudaTextureReadMode read_mode_ = cudaReadModeElementType;
    };
} // namespace qualquer::optix
