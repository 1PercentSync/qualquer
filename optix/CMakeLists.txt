enable_language(CUDA)

set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_ARCHITECTURES 89)

add_library(qualquer_optix STATIC)

target_sources(qualquer_optix PRIVATE
    src/context.cpp
)

target_include_directories(qualquer_optix
    PUBLIC include
)

find_package(CUDAToolkit REQUIRED)

target_link_libraries(qualquer_optix
    PUBLIC spdlog::spdlog
    PUBLIC CUDA::cudart
)