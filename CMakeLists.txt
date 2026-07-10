cmake_minimum_required(VERSION 4.2)
project(qualquer LANGUAGES CXX CUDA ISPC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 89)

# Forward /utf-8 to the MSVC host compiler for CUDA compilation, matching the
# C++ targets' default. Without it nvcc-compiled .cu files trigger C4819 on the
# non-ASCII characters inside NVIDIA's own CUDA headers (code page 936 cannot
# represent them). Suppresses the warning spam without affecting codegen.
add_compile_options($<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=/utf-8>)

# OptiX validation overlay: an independent option (not a build type) that turns
# on OptiX device-context validation for extra error logging. Overlays on the
# standard Debug/Release build types, so existing NDEBUG-based switches are
# left untouched. Enable with -DQUALQUER_OPTIX_VALIDATE=ON.
option(QUALQUER_OPTIX_VALIDATE "Enable OptiX validation mode for extra error logging" OFF)
if(QUALQUER_OPTIX_VALIDATE)
    add_compile_definitions(QUALQUER_OPTIX_VALIDATE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(third_party/bc7enc)
add_subdirectory(third_party/dlss)
add_subdirectory(third_party/ispc_texcomp)
add_subdirectory(vulkan)
add_subdirectory(optix)
add_subdirectory(renderer)
add_subdirectory(app)
