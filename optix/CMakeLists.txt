add_library(qualquer_optix STATIC)

target_sources(qualquer_optix PRIVATE
    src/context.cpp
)

target_include_directories(qualquer_optix
    PUBLIC include
)

find_package(spdlog CONFIG REQUIRED)
find_package(CUDAToolkit REQUIRED)
find_package(OptiX REQUIRED)

target_include_directories(qualquer_optix
    PUBLIC ${OptiX_INCLUDE_DIR}
)

target_link_libraries(qualquer_optix
    PUBLIC spdlog::spdlog
    PUBLIC CUDA::cudart
)