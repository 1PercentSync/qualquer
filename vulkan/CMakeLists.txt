add_library(qualquer_vulkan STATIC)

target_sources(qualquer_vulkan PRIVATE
    src/context.cpp
    src/swapchain.cpp
    src/vma_impl.cpp
)

target_include_directories(qualquer_vulkan
    PUBLIC include
)

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
