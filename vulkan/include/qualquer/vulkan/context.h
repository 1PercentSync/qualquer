#pragma once

/**
 * @file context.h
 * @brief Vulkan context module and VK_CHECK macro.
 */

#include <array>
#include <optional>
#include <string>
#include <vector>
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
     * @brief Device-local VRAM usage snapshot, aggregated across all device-local heaps.
     *
     * Requires VK_EXT_memory_budget, which is enabled as an optional device extension.
     * When unsupported, Context::query_vram_usage returns std::nullopt instead.
     */
    struct VramInfo {
        /** @brief Bytes currently used by the application on device-local heaps. */
        uint64_t used = 0;

        /** @brief Bytes available to the application per the driver-reported budget. */
        uint64_t budget = 0;
    };

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
    };

    /**
     * @brief Core Vulkan context owning instance-level and device-level Vulkan objects.
     *
     * Lifetime is managed explicitly via init() and destroy().
     */
    class Context {
    public:
        /**
         * @brief Phase 1 of the two-stage init: instance, surface, and candidate enumeration.
         *
         * Creates the Vulkan instance, debug messenger, and window surface, then
         * enumerates every physical device that can present to that surface and
         * collects its UUID. The returned UUID list constrains CUDA device
         * selection so a compute-only device that cannot present (e.g. a TCC GPU)
         * is never chosen by the CUDA side and later mismatched here.
         * @param window GLFW window used to create the presentation surface.
         * @return UUIDs of all physical devices supporting presentation to the surface.
         */
        std::vector<std::array<std::uint8_t, 16>> pre_init(GLFWwindow *window);

        /**
         * @brief Phase 2 of the two-stage init: complete the device and its resources.
         *
         * Re-enumerates physical devices, matches the one whose UUID equals the
         * CUDA-selected UUID, and creates the logical device, queues, allocator,
         * and per-frame resources. The UUID must come from the set returned by
         * pre_init (guaranteed presentable).
         * @param device_uuid UUID of the CUDA-selected device to bind to.
         */
        void init(std::array<std::uint8_t, 16> device_uuid);

        /**
         * @brief Destroys all Vulkan objects in reverse creation order.
         *
         * Idempotent: members are reset to null handles, so a repeat call is a no-op
         * (vkDestroy* accept VK_NULL_HANDLE).
         */
        void destroy();

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

        /**
         * @brief Whether the selected device supports VK_EXT_memory_budget.
         *
         * Populated during pick_physical_device() by enumerating device extensions.
         * Drives optional extension enablement in create_device() and the VMA budget
         * flag in create_allocator(). query_vram_usage() returns nullopt when false.
         */
        bool memory_budget_supported = false;

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

        /**
         * @brief Queries aggregated device-local VRAM usage via VMA.
         *
         * Sums usage and budget across all device-local heaps. Returns std::nullopt
         * when VK_EXT_memory_budget is not supported on this device (VMA's budget
         * figures are only meaningful when the budget flag was set at allocator
         * creation).
         *
         * @return Snapshot of current VRAM usage and budget, or nullopt if unsupported.
         */
        [[nodiscard]] std::optional<VramInfo> query_vram_usage() const;

    private:
        /** @brief Creates VkInstance with validation layers and debug_utils extension. */
        void create_instance();

        /** @brief Sets up the debug messenger callback for validation messages. */
        void create_debug_messenger();

        /**
         * @brief Enumerates physical devices that can present to the surface and collects their UUIDs.
         *
         * Called during pre_init. A device qualifies when it has a queue family
         * supporting both graphics and present, exposes the swapchain extension,
         * and supports Vulkan 1.4. UUIDs are read via VkPhysicalDeviceIDProperties.
         * @return UUIDs of all qualifying devices, for CUDA-side constrained selection.
         */
        [[nodiscard]] std::vector<std::array<std::uint8_t, 16>> enumerate_presentable_devices() const;

        /**
         * @brief Matches the physical device whose UUID equals the given UUID.
         *
         * Called during init, after CUDA has selected a device. Sets physical_device,
         * gpu_name, and memory_budget_supported. Aborts if no match is found.
         * @param device_uuid UUID to match (must come from pre_init's candidate set).
         */
        void match_physical_device(std::array<std::uint8_t, 16> device_uuid);

        /**
         * @brief Finds and records the queue family supporting both graphics and present.
         *
         * Must be called after match_physical_device(). Aborts if none found.
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
