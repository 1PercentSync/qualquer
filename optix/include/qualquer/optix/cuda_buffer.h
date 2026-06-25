#pragma once

/**
 * @file cuda_buffer.h
 * @brief CudaBuffer<T> device-memory RAII template.
 */

#include <cstddef>
#include <type_traits>

#include <cuda.h>
#include <cuda_runtime.h>

#include <qualquer/optix/cuda_check.h>

namespace qualquer::optix {
    /**
     * @brief Owning handle to a contiguous block of device memory typed as T[].
     *
     * RAII wrapper over cudaMalloc/cudaFree for fixed-layout POD buffers whose
     * raw bytes are shared between host and device (SBT records, accumulation
     * buffers, launch params). Move-only: device memory has a single owner, and
     * a copy would either double-free or alias one pointer under two lifetimes.
     *
     * T is constrained to trivially-copyable types because the buffer is
     * populated by raw byte copies (cudaMemcpyAsync / cudaMemsetAsync), which
     * is only well-defined for types without per-element construction logic.
     */
    template<typename T>
    class CudaBuffer {
        static_assert(std::is_trivially_copyable_v<T>,
                      "CudaBuffer<T> requires a trivially-copyable T (raw byte copies only)");

    public:
        /** @brief Constructs an empty buffer owning no memory. */
        CudaBuffer() = default;

        /** @brief Releases held device memory. */
        ~CudaBuffer() { free(); }

        CudaBuffer(const CudaBuffer &) = delete;

        CudaBuffer &operator=(const CudaBuffer &) = delete;

        /** @brief Steals another buffer's memory; leaves other empty. */
        CudaBuffer(CudaBuffer &&other) noexcept
            : data_(other.data_), count_(other.count_) {
            other.data_ = nullptr;
            other.count_ = 0;
        }

        /**
         * @brief Releases this buffer's memory, then steals other's.
         * @param other Buffer to steal from; left empty afterwards.
         * @return Reference to this buffer.
         */
        CudaBuffer &operator=(CudaBuffer &&other) noexcept {
            if (this != &other) {
                free();
                data_ = other.data_;
                count_ = other.count_;
                other.data_ = nullptr;
                other.count_ = 0;
            }
            return *this;
        }

        /**
         * @brief Allocates device memory for count elements, freeing any prior allocation.
         *
         * Unconditional: previously held memory is released first, so callers
         * need not track prior state. A count of 0 leaves the buffer empty.
         * @param count Number of elements to allocate.
         */
        void alloc(const std::size_t count) {
            free();
            if (count == 0) {
                return;
            }
            CUDA_CHECK(cudaMalloc(reinterpret_cast<void **>(&data_), count * sizeof(T)));
            count_ = count;
        }

        /**
         * @brief Releases device memory and resets to the empty state.
         *
         * Idempotent: members are reset, so a repeat call is a no-op (matches
         * the optix layer's destroy/release convention).
         */
        void free() {
            if (data_ != nullptr) {
                CUDA_CHECK(cudaFree(data_));
                data_ = nullptr;
            }
            count_ = 0;
        }

        /**
         * @brief Ensures the buffer holds exactly count elements.
         *
         * Skips reallocation when the current count already matches, avoiding
         * churn when a resize request lands on the same size as before.
         * @param count Desired element count.
         */
        void resize(const std::size_t count) {
            if (count != count_) {
                alloc(count);
            }
        }

        /**
         * @brief Asynchronously copies count host elements into the device buffer.
         *
         * The caller must ensure the buffer holds at least count elements (via a
         * prior alloc/resize) and that src remains valid until the copy completes
         * on stream. No-op when count is 0.
         * @param src    Host source address of at least count elements.
         * @param count  Number of elements to copy.
         * @param stream CUDA stream the copy is enqueued on.
         */
        void upload(const T *src, const std::size_t count, const cudaStream_t stream) {
            if (count == 0) {
                return;
            }
            CUDA_CHECK(cudaMemcpyAsync(data_, src, count * sizeof(T), cudaMemcpyHostToDevice, stream));
        }

        /**
         * @brief Asynchronously zeroes the entire device buffer.
         *
         * An all-zero byte pattern is the well-defined zero value for any
         * trivially-copyable T, so a byte-wise memset resets every element
         * without a typed fill. No-op when the buffer is empty.
         * @param stream CUDA stream the memset is enqueued on.
         */
        void clear(const cudaStream_t stream) const {
            if (count_ == 0) {
                return;
            }
            CUDA_CHECK(cudaMemsetAsync(data_, 0, count_ * sizeof(T), stream));
        }

        /**
         * @brief Device pointer typed as T*.
         *
         * Points at device memory; host code must not dereference it. Null when
         * the buffer is empty.
         * @return Device-side T*.
         */
        T *data() { return data_; }

        /** @brief Const overload of data(); null when the buffer is empty. */
        const T *data() const { return data_; }

        /**
         * @brief Device pointer reinterpreted as CUdeviceptr for OptiX API use.
         *
         * OptiX APIs (SBT record addresses, pipeline launch params) take a
         * CUdeviceptr rather than a typed pointer; this cast is the NVIDIA-
         * documented bridge between runtime-allocated memory and the driver/
         * OptiX handle form.
         * @return Device pointer in OptiX's integer-handle form; 0 when empty.
         */
        CUdeviceptr device_ptr() const {
            return reinterpret_cast<CUdeviceptr>(data_);
        }

        /** @return Number of elements currently allocated (0 when empty). */
        std::size_t count() const { return count_; }

        /** @return Allocated size in bytes (count() * sizeof(T)). */
        std::size_t size_bytes() const { return count_ * sizeof(T); }

    private:
        /** @brief Device pointer to the first element; null when empty. */
        T *data_ = nullptr;
        /** @brief Number of elements allocated; 0 when empty. */
        std::size_t count_ = 0;
    };
} // namespace qualquer::optix
