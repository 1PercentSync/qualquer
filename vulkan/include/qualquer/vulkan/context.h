#pragma once

/**
 * @file context.h
 * @brief Vulkan context: instance, device, queues, per-frame resources.
 */

#include <array>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct GLFWwindow;
struct IDXGIAdapter3;

namespace qualquer::vulkan {
    /** @brief Number of frames that can be in-flight simultaneously. */
    constexpr uint32_t kMaxFramesInFlight = 2;

    /**
     * @brief Process-local VRAM usage snapshot reported by Windows WDDM.
     *
     * The local memory segment covers allocations attributed to this process on
     * the selected adapter across Vulkan, CUDA, OptiX, and other GPU APIs.
     */
    struct VramInfo {
        /** @brief Current process usage of the adapter's local memory segment, in bytes. */
        uint64_t used = 0;

        /** @brief OS-provided local-memory budget for the process, in bytes. */
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
        std::vector<std::array<std::uint8_t, 16> > pre_init(GLFWwindow *window);

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

        /** @brief Index of the current in-flight frame (0 to kMaxFramesInFlight-1). */
        uint32_t frame_index = 0;

        /** @brief Returns the FrameData for the current in-flight frame. */
        FrameData &current_frame() {
            return frames_[frame_index];
        }

        /** @brief Advances to the next in-flight frame index. */
        void advance_frame() {
            frame_index = (frame_index + 1) % kMaxFramesInFlight;
        }

        /**
         * @brief Queries process-wide local VRAM usage through Windows WDDM.
         *
         * The query includes memory attributed to this process on the selected GPU,
         * independent of whether Vulkan, CUDA, OptiX, or another API allocated it.
         * Shared system memory in the non-local segment is intentionally excluded.
         *
         * @return Current process usage and budget, or nullopt if DXGI is unavailable.
         */
        [[nodiscard]] std::optional<VramInfo> query_vram_usage() const;

    private:
        /** @brief Debug messenger for validation layer callbacks. */
        VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;

        /** @brief VMA allocator for GPU memory management. */
        VmaAllocator allocator_ = VK_NULL_HANDLE;

        /** @brief Per-frame synchronization and command recording resources. */
        std::array<FrameData, kMaxFramesInFlight> frames_{};

        /** @brief DXGI view of the selected GPU, owned and released by this context. */
        IDXGIAdapter3 *dxgi_adapter_ = nullptr;

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
        [[nodiscard]] std::vector<std::array<std::uint8_t, 16> > enumerate_presentable_devices() const;

        /**
         * @brief Matches the physical device whose UUID equals the given UUID.
         *
         * Called during init, after CUDA has selected a device. Sets physical_device,
         * gpu_name, and the matching DXGI adapter. Aborts if no Vulkan match is found.
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
