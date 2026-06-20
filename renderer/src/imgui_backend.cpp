/**
 * @file imgui_backend.cpp
 * @brief ImGui integration implementation.
 */

#include <qualquer/renderer/imgui_backend.h>

#include <cstdlib>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <GLFW/glfw3.h>

#include <qualquer/vulkan/context.h>
#include <qualquer/vulkan/swapchain.h>

namespace qualquer::renderer {
    // ReSharper disable once CppMemberFunctionMayBeStatic
    void ImGuiBackend::init(const vulkan::Context &context, // NOLINT(*-convert-member-functions-to-static)
                            const vulkan::Swapchain &swapchain,
                            GLFWwindow *window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        // Scale font and style by the window content scale for HiDPI legibility.
        float scale_x = 0.0f;
        float scale_y = 0.0f;
        glfwGetWindowContentScale(window, &scale_x, &scale_y);

        ImFontConfig font_config;
        font_config.SizePixels = 13.0f * scale_y;
        io.Fonts->AddFontDefault(&font_config);
        ImGui::GetStyle().ScaleAllSizes(scale_y);

        // install_callbacks = true: ImGui chains the app's existing GLFW callbacks
        // behind its own, so future input handling is preserved.
        ImGui_ImplGlfw_InitForVulkan(window, true);

        // Zero-clear first: the backend requires it, and most fields stay default.
        ImGui_ImplVulkan_InitInfo init_info{};

        init_info.ApiVersion = VK_API_VERSION_1_4;
        init_info.Instance = context.instance;
        init_info.PhysicalDevice = context.physical_device;
        init_info.Device = context.device;
        init_info.QueueFamily = context.graphics_queue_family;
        init_info.Queue = context.graphics_queue;

        // Backend owns the descriptor pool (no VkDescriptorPool in this class).
        // Font-atlas minimum for now; bump when registering real textures.
        init_info.DescriptorPoolSize = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE;

        init_info.MinImageCount = 2; // backend-required lower bound
        init_info.ImageCount = static_cast<uint32_t>(swapchain.images.size());

        init_info.UseDynamicRendering = true;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.sType =
                VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
        init_info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchain.format;

        if (!ImGui_ImplVulkan_Init(&init_info)) {
            // Clean up only the already-initialized GLFW backend + context; the Vulkan
            // backend may be partially set up, so don't call its shutdown.
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            std::abort();
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void ImGuiBackend::destroy() { // NOLINT(*-convert-member-functions-to-static)
        // No external handle here: the descriptor pool is backend-owned. Caller must
        // ensure the queue is idle first.
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void ImGuiBackend::begin_frame() { // NOLINT(*-convert-member-functions-to-static)
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    // ReSharper disable once CppParameterMayBeConst
    void ImGuiBackend::render(VkCommandBuffer cmd) { // NOLINT(*-convert-member-functions-to-static)
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    }
} // namespace qualquer::renderer
