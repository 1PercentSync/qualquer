add_library(qualquer_vulkan STATIC)

target_sources(qualquer_vulkan PRIVATE
    src/context.cpp
    src/swapchain.cpp
    src/vma_impl.cpp
)

target_include_directories(qualquer_vulkan
    PUBLIC include
)

# Unlocks the Vulkan Win32 platform section (vulkan_win32.h), which declares the
# VK_KHR_external_memory_win32 / VK_KHR_external_semaphore_win32 extensions used
# for CUDA interop handle export. PUBLIC so upper layers including vulkan.h get
# it too.
target_compile_definitions(qualquer_vulkan PUBLIC VK_USE_PLATFORM_WIN32_KHR)

find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

target_link_libraries(qualquer_vulkan
    PUBLIC spdlog::spdlog
    PUBLIC glfw
    PUBLIC Vulkan::Vulkan
    PUBLIC GPUOpen::VulkanMemoryAllocator
)
