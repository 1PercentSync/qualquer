/**
 * @file cuda_device_pool.cpp
 * @brief CudaDevicePool bump allocator implementation.
 */

#include <qualquer/optix/cuda_device_pool.h>

#include <cassert>

#include <qualquer/optix/cuda_check.h>

namespace qualquer::optix {
    CudaDevicePool::CudaDevicePool(CudaDevicePool &&other) noexcept
        : base_(other.base_),
          total_bytes_(other.total_bytes_),
          offset_(other.offset_) {
        other.base_ = nullptr;
        other.total_bytes_ = 0;
        other.offset_ = 0;
    }

    CudaDevicePool &CudaDevicePool::operator=(CudaDevicePool &&other) noexcept {
        if (this != &other) {
            free();
            base_ = other.base_;
            total_bytes_ = other.total_bytes_;
            offset_ = other.offset_;
            other.base_ = nullptr;
            other.total_bytes_ = 0;
            other.offset_ = 0;
        }
        return *this;
    }

    void CudaDevicePool::alloc(const std::size_t total_bytes) {
        free();
        if (total_bytes == 0) {
            return;
        }
        CUDA_CHECK(cudaMalloc(&base_, total_bytes));
        total_bytes_ = total_bytes;
        offset_ = 0;
    }

    void *CudaDevicePool::suballocate(const std::size_t bytes, const std::size_t alignment) {
        assert(base_ != nullptr && "suballocate on empty pool");
        assert((alignment & (alignment - 1)) == 0 && "alignment must be power of two");

        // Align the current offset.
        const auto base_addr = reinterpret_cast<std::uintptr_t>(base_);
        const auto current = base_addr + offset_;
        const auto aligned = (current + alignment - 1) & ~(alignment - 1);
        const auto padding = aligned - current;

        assert(offset_ + padding + bytes <= total_bytes_ && "pool overflow");

        offset_ += padding + bytes;
        return reinterpret_cast<void *>(aligned);
    }

    void CudaDevicePool::free() {
        if (base_ != nullptr) {
            CUDA_CHECK(cudaFree(base_));
            base_ = nullptr;
        }
        total_bytes_ = 0;
        offset_ = 0;
    }
} // namespace qualquer::optix
