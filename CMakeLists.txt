cmake_minimum_required(VERSION 4.2)
project(qualquer LANGUAGES CXX CUDA)

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

add_subdirectory(vulkan)
add_subdirectory(optix)
add_subdirectory(renderer)
add_subdirectory(app)
