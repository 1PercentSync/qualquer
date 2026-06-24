find_package(imgui CONFIG REQUIRED)

add_library(qualquer_renderer STATIC)

target_sources(qualquer_renderer PRIVATE
    src/imgui_backend.cpp
    src/debug_ui.cpp
    src/renderer.cpp
    src/test_kernel.cu
)

target_include_directories(qualquer_renderer
    PUBLIC include
)

target_link_libraries(qualquer_renderer
    PUBLIC qualquer_vulkan
    PRIVATE imgui::imgui
    PRIVATE qualquer_optix
)
