find_package(imgui CONFIG REQUIRED)
find_package(OptiX REQUIRED)

include(CompileOptiXIR)

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

# Compile and deploy OptiX device programs to the runtime shader directory.
compile_optix_ir(qualquer_renderer src/device/programs.cu)
