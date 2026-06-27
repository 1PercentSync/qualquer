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

# Expose device programs to CLion's code model (inherits target include paths)
# without compiling them as regular CUDA objects — actual compilation is handled
# by the compile_optix_ir custom command below.
set_source_files_properties(src/device/programs.cu PROPERTIES HEADER_FILE_ONLY ON)
target_sources(qualquer_renderer PRIVATE src/device/programs.cu)

# Compile and deploy OptiX device programs to the runtime shader directory.
compile_optix_ir(qualquer_renderer src/device/programs.cu)
