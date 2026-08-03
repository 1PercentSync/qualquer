#pragma once

/**
 * @file cuda_device_pool.h
 * @brief CudaDevicePool: single-allocation bump allocator for device memory.
 */

#include <cstddef>
#include <cstdint>

#include <cuda_runtime.h>

namespace qualquer::optix {
    /**
     * @brief Single contiguous device allocation with bump suballocation.
     *
     * All suballocated regions share one cudaMalloc block, enabling a single
     * cudaAccessPolicyWindow to cover the entire pool. This is the mechanism
     * for marking PT's BVH and scene data as L2-streaming during DLSS overlap.
     *
     * Intended for init-time data whose sizes are known before allocation
     * (acceleration structures after compacted-size query, geometry info,
     * materials). Not a general-purpose allocator: individual suballocations
     * cannot be freed; the entire pool is freed at once.
     */
    class CudaDevicePool {
    public:
        CudaDevicePool() = default;
        ~CudaDevicePool() { free(); }

        CudaDevicePool(const CudaDevicePool &) = delete;
        CudaDevicePool &operator=(const CudaDevicePool &) = delete;

        CudaDevicePool(CudaDevicePool &&other) noexcept;
        CudaDevicePool &operator=(CudaDevicePool &&other) noexcept;

        /**
         * @brief Allocates the backing device memory block.
         *
         * Frees any prior allocation. The bump offset resets to 0.
         *
         * @param total_bytes Total size of the pool in bytes.
         */
        void alloc(std::size_t total_bytes);

        /**
         * @brief Returns a suballocated region within the pool.
         *
         * Advances the internal bump pointer by (alignment padding + bytes).
         * The returned pointer is device memory; host code must not dereference it.
         *
         * @param bytes     Number of bytes to suballocate.
         * @param alignment Required alignment (must be a power of two).
         * @return Device pointer to the suballocated region.
         */
        void *suballocate(std::size_t bytes, std::size_t alignment);

        /**
         * @brief Releases the entire backing allocation.
         *
         * All previously returned suballocated pointers become invalid.
         * Idempotent: a repeat call is a no-op.
         */
        void free();

        /** @brief Base device pointer of the pool; null when empty. */
        [[nodiscard]] void *base_ptr() const { return base_; }

        /** @brief Total allocated size in bytes; 0 when empty. */
        [[nodiscard]] std::size_t total_bytes() const { return total_bytes_; }

        /** @brief Bytes consumed so far by suballocations (including padding). */
        [[nodiscard]] std::size_t used_bytes() const { return offset_; }

        /** @brief Whether the pool owns an allocation. */
        [[nodiscard]] bool valid() const { return base_ != nullptr; }

    private:
        void *base_ = nullptr;
        std::size_t total_bytes_ = 0;
        std::size_t offset_ = 0;
    };
} // namespace qualquer::optix
