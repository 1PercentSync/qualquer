#pragma once

/**
 * @file context.h
 * @brief Vulkan context module and VK_CHECK macro.
 */

#include <array>
#include <string>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <spdlog/spdlog.h>

/**
 * @brief Checks a VkResult and aborts on failure with diagnostic output.
 *
 * Logs the failed expression, VkResult code, and source location via spdlog.
 * Vulkan API errors during development are programming errors
 * and do not need runtime recovery.
 */
#define VK_CHECK(x)                                                      \
    do {                                                                 \
        VkResult vk_check_result_ = (x);                                 \
        if (vk_check_result_ != VK_SUCCESS) {                            \
            spdlog::critical("VK_CHECK failed: {} returned {} at {}:{}", \
                             #x,                                         \
                             static_cast<int>(vk_check_result_),         \
                             __FILE__,                                   \
                             __LINE__);                                  \
            std::abort();                                                \
        }                                                                \
    } while (0)

struct GLFWwindow;

namespace qualquer::vulkan {
    /** @brief Number of frames that can be in-flight simultaneously. */
    constexpr uint32_t kMaxFramesInFlight = 2;

    /**
     * @brief Per-frame GPU synchronization and command recording resources.
     *
     * Each in-flight frame owns an independent set of these objects
     * so the CPU can record frame N+1 while the GPU is still executing frame N.
     */
    struct FrameData {
        /** @brief Command pool for this frame's command buffer allocation. */
        VkCommandPool command_pool = VK_NULL_HANDLE;

        /** @brief Primary command buffer recorded each frame. */
        VkCommandBuffer command_buffer = VK_NULL_HANDLE;

        /** @brief Signaled by the GPU when this frame's commands finish executing. */
        VkFence render_fence = VK_NULL_HANDLE;

        /** @brief Signaled when a swapchain image has been acquired for this frame. */
        VkSemaphore image_available_semaphore = VK_NULL_HANDLE;

        /** @brief Signaled by queue submit, waited by present to ensure rendering is complete. */
        VkSemaphore render_finished_semaphore = VK_NULL_HANDLE;
    };

    /**
     * @brief Core Vulkan context owning instance-level and device-level Vulkan objects.
     *
     * Lifetime is managed explicitly via init() and destroy().
     */
    class Context {
    public:
        /**
         * @brief Initializes the Vulkan context.
         * @param window GLFW window used to create the presentation surface.
         */
        void init(GLFWwindow *window);

        /**
         * @brief Destroys all Vulkan objects in reverse creation order.
         */
        void destroy() const;

        /** @brief Vulkan instance. */
        VkInstance instance = VK_NULL_HANDLE;

        /** @brief Debug messenger for validation layer callbacks. */
        VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

        /** @brief Window surface for swapchain presentation. */
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        /** @brief Selected physical device (GPU). */
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;

        /** @brief Human-readable GPU name, populated during init. */
        std::string gpu_name;

        /** @brief Queue family index supporting both graphics and present. */
        uint32_t graphics_queue_family = 0;

        /** @brief Logical device. */
        VkDevice device = VK_NULL_HANDLE;

        /** @brief Graphics queue (also used for presentation). */
        VkQueue graphics_queue = VK_NULL_HANDLE;

        /** @brief VMA allocator for GPU memory management. */
        VmaAllocator allocator = VK_NULL_HANDLE;

        /** @brief Per-frame synchronization and command recording resources. */
        std::array<FrameData, kMaxFramesInFlight> frames{};

        /** @brief Index of the current in-flight frame (0 to kMaxFramesInFlight-1). */
        uint32_t frame_index = 0;

        /** @brief Returns the FrameData for the current in-flight frame. */
        FrameData &current_frame() { return frames[frame_index]; }

        /** @brief Advances to the next in-flight frame index. */
        void advance_frame() { frame_index = (frame_index + 1) % kMaxFramesInFlight; }

    private:
        /** @brief Creates VkInstance with validation layers and debug_utils extension. */
        void create_instance();

        /** @brief Sets up the debug messenger callback for validation messages. */
        void create_debug_messenger();

        /** @brief Selects a suitable physical device, preferring discrete GPUs. */
        void pick_physical_device();

        /**
         * @brief Finds and records the queue family supporting both graphics and present.
         *
         * Must be called after pick_physical_device(). Aborts if none found.
         */
        void find_graphics_queue_family();

        /**
         * @brief Creates the logical device with swapchain extension and Vulkan 1.4 core features.
         */
        void create_device();

        /** @brief Initializes the VMA allocator. */
        void create_allocator();

        /**
         * @brief Creates per-frame command pools, command buffers, fences, and semaphores.
         *
         * Fences are created in the signaled state so the first frame's fence wait
         * succeeds immediately without special-casing.
         */
        void create_frame_data();
    };
} // namespace qualquer::vulkan
