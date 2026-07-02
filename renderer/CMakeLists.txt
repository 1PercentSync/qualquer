find_package(imgui CONFIG REQUIRED)
find_package(OptiX REQUIRED)
find_package(xxHash CONFIG REQUIRED)
find_package(Stb REQUIRED)

include(CompileOptiXIR)

add_library(qualquer_renderer STATIC)

target_sources(qualquer_renderer PRIVATE
    src/imgui_backend.cpp
    src/debug_ui.cpp
    src/renderer.cpp
    src/camera.cpp
    src/cache.cpp
    src/ktx2.cpp
    src/stb_impl.cpp
    src/texture.cpp
    src/tonemap.cu
)

target_include_directories(qualquer_renderer
    PUBLIC include
    PRIVATE ${Stb_INCLUDE_DIR}
)

target_link_libraries(qualquer_renderer
    PUBLIC qualquer_vulkan
    PRIVATE imgui::imgui
    PRIVATE qualquer_optix
    PRIVATE xxHash::xxhash
    PRIVATE bc7enc
    PRIVATE ispc_texcomp
)

# Expose device programs to CLion's code model (inherits target include paths)
# without compiling them as regular CUDA objects — actual compilation is handled
# by the compile_optix_ir custom command below.
set_source_files_properties(src/device/programs.cu PROPERTIES HEADER_FILE_ONLY ON)
target_sources(qualquer_renderer PRIVATE src/device/programs.cu)

# Compile and deploy OptiX device programs to the runtime shader directory.
compile_optix_ir(qualquer_renderer src/device/programs.cu)
