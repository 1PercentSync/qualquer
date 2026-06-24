/**
 * @file interop.cpp
 * @brief Vulkan external memory and semaphore resource implementation.
 */

// windows.h must precede any vulkan_win32.h inclusion: the Win32 handle types
// (HANDLE) that vkGetMemoryWin32HandleKHR's signature depends on come from it.
// WIN32_LEAN_AND_MEAN trims the macro surface to only the handle API needed here.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <qualquer/vulkan/interop.h>

#include <vulkan/vulkan_win32.h>

#include <spdlog/spdlog.h>

#include <qualquer/vulkan/context.h>

namespace qualquer::vulkan {
    void InteropImage::init(const Context &context,
                            const VkFormat format,
                            const VkExtent2D extent,
                            const VkImageUsageFlags usage) {
        // OPAQUE_WIN32 pairs with CUDA's cudaExternalMemoryHandleTypeOpaqueWin32 so the
        // exported handle is importable by CUDA. The exportable flag lets the backing
        // memory be allocated for cross-API sharing.
        VkExternalMemoryImageCreateInfo external_info{
            .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO,
            .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR,
        };

        const VkImageCreateInfo image_info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = &external_info,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = {extent.width, extent.height, 1},
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VK_CHECK(vkCreateImage(context.device, &image_info, nullptr, &image));

        VkMemoryRequirements mem_reqs;
        vkGetImageMemoryRequirements(context.device, image, &mem_reqs);

        // Device-local for the fastest blit; the image stays GPU-resident since CUDA
        // writes it through a surface object.
        VkPhysicalDeviceMemoryProperties mem_props;
        vkGetPhysicalDeviceMemoryProperties(context.physical_device, &mem_props);

        uint32_t memory_type_index = VK_MAX_MEMORY_TYPES;
        for (uint32_t i = 0; i < mem_props.memoryTypeCount; ++i) {
            if (!(mem_reqs.memoryTypeBits & (1u << i))) {
                continue;
            }
            if (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memory_type_index = i;
                break;
            }
        }
        if (memory_type_index == VK_MAX_MEMORY_TYPES) {
            spdlog::critical("No device-local memory type available for interop image");
            std::abort();
        }

        // Exportable memory bypasses VMA's suballocation; a dedicated allocation ties
        // this one vkAllocateMemory to the image, which interop export requires.
        VkMemoryDedicatedAllocateInfo dedicated{
            .sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
            .image = image,
            .buffer = VK_NULL_HANDLE,
        };

        VkExportMemoryAllocateInfo export_info{
            .sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
            .pNext = &dedicated,
            .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR,
        };

        const VkMemoryAllocateInfo alloc_info{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = &export_info,
            .allocationSize = mem_reqs.size,
            .memoryTypeIndex = memory_type_index,
        };

        VK_CHECK(vkAllocateMemory(context.device, &alloc_info, nullptr, &memory));
        VK_CHECK(vkBindImageMemory(context.device, image, memory, 0));

        // VkExportMemoryWin32HandleInfoKHR (security attrs / access mask) is omitted:
        // same-process CUDA import needs no custom authorization — the driver's default
        // descriptor is accessible within the process.
        const VkMemoryGetWin32HandleInfoKHR handle_info{
            .sType = VK_STRUCTURE_TYPE_MEMORY_GET_WIN32_HANDLE_INFO_KHR,
            .memory = memory,
            .handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR,
        };

        // vkGetMemoryWin32HandleKHR is a non-WSI device-level extension function:
        // the Vulkan loader provides trampolines only for core and WSI functions,
        // so this one must be loaded via vkGetDeviceProcAddr rather than called
        // directly (unlike WSI functions such as vkCreateSwapchainKHR).
        const auto get_handle = reinterpret_cast<PFN_vkGetMemoryWin32HandleKHR>(
            vkGetDeviceProcAddr(context.device, "vkGetMemoryWin32HandleKHR"));
        if (get_handle == nullptr) {
            spdlog::critical("vkGetMemoryWin32HandleKHR not available");
            std::abort();
        }

        VK_CHECK(get_handle(context.device, &handle_info, &win32_handle));

        spdlog::info("Interop image created ({}x{})", extent.width, extent.height);
    }

    // Close the handle first: it holds a reference to the memory, so freeing memory
    // before closing leaks it. Image before memory (creation order reversed).
    void InteropImage::destroy(const Context &context) const {
        CloseHandle(win32_handle);
        vkDestroyImage(context.device, image, nullptr);
        vkFreeMemory(context.device, memory, nullptr);
    }

    void InteropSemaphore::init(const Context &context) {
        // OPAQUE_WIN32 pairs with CUDA's cudaExternalSemaphoreHandleTypeOpaqueWin32 so
        // the exported handle is importable by CUDA (signal side of the cross-API sync).
        VkExportSemaphoreCreateInfo export_info{
            .sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO,
            .handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR,
        };

        const VkSemaphoreCreateInfo semaphore_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = &export_info,
        };

        VK_CHECK(vkCreateSemaphore(context.device, &semaphore_info, nullptr, &semaphore));

        // VkExportSemaphoreWin32HandleInfoKHR (security attrs / access mask) is omitted:
        // same-process CUDA import needs no custom authorization — the driver's default
        // descriptor is accessible within the process.
        const VkSemaphoreGetWin32HandleInfoKHR handle_info{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_WIN32_HANDLE_INFO_KHR,
            .semaphore = semaphore,
            .handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT_KHR,
        };

        // Loaded via vkGetDeviceProcAddr, same reason as vkGetMemoryWin32HandleKHR
        // above (non-WSI device-level extension function, no loader trampoline).
        const auto get_handle = reinterpret_cast<PFN_vkGetSemaphoreWin32HandleKHR>(
            vkGetDeviceProcAddr(context.device, "vkGetSemaphoreWin32HandleKHR"));
        if (get_handle == nullptr) {
            spdlog::critical("vkGetSemaphoreWin32HandleKHR not available");
            std::abort();
        }

        VK_CHECK(get_handle(context.device, &handle_info, &win32_handle));

        spdlog::info("Interop semaphore created");
    }

    // Same close-before-destroy ordering as InteropImage: the NT handle holds a
    // reference to the semaphore and must be released first.
    void InteropSemaphore::destroy(const Context &context) const {
        CloseHandle(win32_handle);
        vkDestroySemaphore(context.device, semaphore, nullptr);
    }
} // namespace qualquer::vulkan
