find_package(fastgltf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenMP REQUIRED)
find_package(xxHash CONFIG REQUIRED)
find_package(mikktspace CONFIG REQUIRED)
find_package(Stb REQUIRED)

add_executable(qualquer_app
    src/main.cpp
    src/application.cpp
    src/camera_controller.cpp
    src/config.cpp
    src/scene_loader.cpp
    src/cache.cpp
    src/ktx2.cpp
    src/mesh.cpp
    src/stb_impl.cpp
    src/texture.cpp
    src/emissive_alias_table.cpp
    src/env_alias_table.cpp
    src/equirect_to_cubemap.cu
)

target_include_directories(qualquer_app
    PRIVATE include
    PRIVATE ${Stb_INCLUDE_DIR}
)

target_link_libraries(qualquer_app
    PRIVATE qualquer_renderer
    PRIVATE qualquer_optix
    PRIVATE fastgltf::fastgltf
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE xxHash::xxhash
    PRIVATE mikktspace::mikktspace
    PRIVATE bc7enc
    PRIVATE ispc_texcomp
    PRIVATE OpenMP::OpenMP_CXX
)

# Deploy OptiX IR to the executable's runtime directory so Pipeline::init can
# load it via a CWD-relative path. copy_directory sweeps every .optixir in
# QUALQUER_OPTIX_IR_DIR (which holds only .optixir artifacts), so adding device
# programs needs no change here. The copy runs on the app side because renderer
# cannot depend on app (single-direction dependency) and thus cannot deploy into
# the executable's directory itself.
add_custom_command(TARGET qualquer_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:qualquer_app>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${QUALQUER_OPTIX_IR_DIR}"
            "$<TARGET_FILE_DIR:qualquer_app>/shaders"
)
