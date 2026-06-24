#pragma once

/**
 * @file context.h
 * @brief OptiX layer root context: CUDA device selection and UUID query.
 */

#include <array>
#include <cstdint>
#include <vector>

#include <cuda_runtime.h>

namespace qualquer::optix {
    /**
     * @brief Number of frames that can be in-flight simultaneously.
     *
     * Matches vulkan::kMaxFramesInFlight. Duplicated per-layer rather than shared
     * to keep the optix layer free of a vulkan dependency (single-direction
     * dependency is a hard architectural constraint).
     */
    constexpr uint32_t kMaxFramesInFlight = 2;
    /**
     * @brief OptiX layer root context owning the selected CUDA device.
     *
     * Selects the best CUDA-capable device during init() from a presentability-
     * constrained candidate list (supplied by the Vulkan layer's pre_init),
     * activates its primary context via cudaSetDevice, and exposes the device
     * UUID for Vulkan physical-device matching. Lifetime is managed explicitly
     * via init() and destroy().
     */
    class Context {
    public:
        /**
         * @brief Selects and activates the best CUDA device among the presentable candidates.
         *
         * The candidate list comes from Vulkan pre_init — devices that cannot
         * present are excluded before CUDA scores them, so the selection never
         * lands on a compute-only device (e.g. a TCC GPU) that Vulkan could not
         * match for presentation.
         * @param presentable_uuids UUIDs of devices the Vulkan layer confirmed presentable.
         */
        void init(const std::vector<std::array<std::uint8_t, 16>> &presentable_uuids);

        /**
         * @brief Imports the Vulkan-allocated display buffer for CUDA access.
         *
         * The resulting display_surface member is the write handle for the shared image.
         * @param win32_handle  NT handle exported by vulkan::InteropImage.
         * @param width         Image width in pixels, must match the Vulkan image.
         * @param height        Image height in pixels, must match the Vulkan image.
         * @param size          Allocation size from vkGetImageMemoryRequirements.
         */
        void import_display_buffer(void *win32_handle, uint32_t width, uint32_t height, uint64_t size);

        /**
         * @brief Imports the per-frame Vulkan external semaphores.
         *
         * Cross-API synchronization (CUDA signal -> Vulkan wait). One per
         * frame-in-flight, indexed by frame.
         * @param win32_handles NT handles exported by vulkan::InteropSemaphore.
         */
        void import_semaphores(std::array<void *, kMaxFramesInFlight> win32_handles);

        /**
         * @brief Releases only the display-buffer import, keeping the semaphores.
         *
         * For swapchain resize: the display buffer is resolution-dependent and rebuilt,
         * while the semaphores are resolution-independent and persist. Idempotent:
         * the display-buffer handles are reset, so a repeat call is a no-op.
         */
        void release_display_buffer();

        /**
         * @brief Releases CUDA resources associated with the selected device.
         *
         * Idempotent: members reset so a repeat call is a no-op.
         */
        void destroy();

        /**
         * @brief Selected device UUID, for Vulkan physical-device matching.
         *
         * Raw std::array<uint8_t,16> (not cudaUUID_t) so the Vulkan layer can
         * compare it without depending on CUDA headers. No alias is defined —
         * the raw type is used directly at every layer.
         */
        std::array<std::uint8_t, 16> device_uuid{};

        /**
         * @brief Surface object over the imported display buffer.
         *
         * Zero before import_display_buffer and after release_display_buffer / destroy.
         */
        cudaSurfaceObject_t display_surface = 0;

        /**
         * @brief Imported external semaphores, one per frame-in-flight.
         *
         * Indexed by frame-in-flight slot. Zero before import_semaphores and after destroy.
         */
        std::array<cudaExternalSemaphore_t, kMaxFramesInFlight> external_semaphores{};

        /**
         * @brief CUDA stream for kernel launch and external semaphore signaling.
         *
         * Explicit, not the default stream: the default stream implicitly
         * synchronizes with all explicit streams, serializing any concurrent work.
         * Null before init and after destroy.
         */
        cudaStream_t stream = nullptr;

    private:
        /** @brief Index of the selected CUDA device, for subsequent runtime calls. */
        int device_id_ = -1;

        /** @brief Imported external memory object wrapping the Vulkan display buffer. */
        cudaExternalMemory_t external_memory_ = nullptr;

        /** @brief Mipmapped array mapped onto the external memory (single mip level). */
        cudaMipmappedArray_t mipmap_array_ = nullptr;

        /** @brief Level-0 cudaArray extracted from the mipmapped array, for the surface object. */
        cudaArray_t array_ = nullptr;
    };
} // namespace qualquer::optix