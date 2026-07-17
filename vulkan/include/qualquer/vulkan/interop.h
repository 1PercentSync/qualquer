#pragma once

/**
 * @file interop.h
 * @brief Vulkan external memory and semaphore resources for CUDA interop.
 */

#include <vulkan/vulkan.h>

namespace qualquer::vulkan {
    class Context;

    /**
     * @brief Vulkan image backed by exportable device memory, for CUDA interop.
     *
     * CUDA imports the exported NT handle via cudaImportExternalMemory and writes
     * the shared image (e.g. through a cudaSurfaceObject_t).
     *
     * Per the project ownership principle, this class owns the VkImage,
     * VkDeviceMemory, and the exported NT handle; it holds no handle owned by
     * Context. Device / physical-device are obtained from the Context reference
     * passed to each operation.
     */
    class InteropImage {
    public:
        /**
         * @brief Creates the image, allocates and binds dedicated exportable memory,
         *        and exports the Win32 NT handle.
         * @param context Vulkan context providing the device and physical device.
         * @param format  Image format (e.g. R16G16B16A16_SFLOAT for the display buffer).
         * @param extent  Image extent in pixels.
         * @param usage   Image usage flags (e.g. TRANSFER_SRC_BIT for a blit source).
         */
        void init(const Context &context, VkFormat format, VkExtent2D extent, VkImageUsageFlags usage);

        /**
         * @brief Closes the NT handle, then destroys the image and frees the memory.
         *
         * The NT handle holds a reference to the underlying memory, so it must be
         * closed before the memory can be freed; the image is destroyed before the
         * memory is freed (creation order reversed). Idempotent: members reset so a
         * repeat call is a no-op (vkDestroy* and vkFreeMemory accept VK_NULL_HANDLE,
         * CloseHandle(nullptr) is a no-op).
         * @param context Vulkan context providing the owning device.
         */
        void destroy(const Context &context);

        /** @brief The image handle. */
        VkImage image = VK_NULL_HANDLE;

        /** @brief Dedicated device memory bound to the image. */
        VkDeviceMemory memory = VK_NULL_HANDLE;

        /**
         * @brief Allocation size reported by vkGetImageMemoryRequirements.
         *
         * Consumed by the CUDA import path, which requires the allocation size to
         * match what Vulkan allocated for the external memory object.
         */
        VkDeviceSize size = 0;

        /**
         * @brief Win32 NT handle exported from the memory.
         *
         * Ownership stays with this object: CUDA does not take handle ownership on
         * import, so the application reads it to call cudaImportExternalMemory and
         * this object closes it on destroy. Raw void* so this header does not depend
         * on windows.h (CUDA's handle.win32.handle is void*).
         */
        void *win32_handle = nullptr;
    };

    /**
     * @brief Vulkan semaphore with an exported Win32 handle, for CUDA interop.
     *
     * CUDA imports the handle and signals it; Vulkan waits on it, coordinating
     * access to the shared interop image (CUDA signal -> Vulkan wait).
     *
     * Per the project ownership principle, this class owns the VkSemaphore and the
     * exported NT handle; it holds no handle owned by Context.
     */
    class InteropSemaphore {
    public:
        /**
         * @brief Creates the semaphore and exports the Win32 NT handle.
         * @param context Vulkan context providing the device.
         */
        void init(const Context &context);

        /**
         * @brief Closes the NT handle, then destroys the semaphore.
         *
         * Idempotent: members reset so a repeat call is a no-op.
         * @param context Vulkan context providing the owning device.
         */
        void destroy(const Context &context);

        /** @brief The semaphore handle. */
        VkSemaphore semaphore = VK_NULL_HANDLE;

        /**
         * @brief Win32 NT handle exported from the semaphore.
         *
         * Ownership stays with this object: CUDA does not take handle ownership on
         * import, so the application reads it to call cudaImportExternalSemaphore
         * and this object closes it on destroy. Raw void* so this header does not
         * depend on windows.h.
         */
        void *win32_handle = nullptr;
    };
} // namespace qualquer::vulkan
