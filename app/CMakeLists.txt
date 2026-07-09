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
# load it via a CWD-relative path. The copy lives on the app side because
# renderer cannot depend on app (single-direction dependency) and thus cannot
# deploy into the executable's directory itself.
#
# Dependency-tracked copy, NOT a POST_BUILD step: POST_BUILD only fires when
# the app relinks, but device-only edits recompile the .optixir without
# touching any host object — the app never relinks and a POST_BUILD copy keeps
# serving the stale file. An OUTPUT rule re-runs whenever the compiled
# .optixir changes, independent of host-side state.
#
# OUTPUT cannot use target-dependent generator expressions, so the destination
# is spelled via CMAKE_CURRENT_BINARY_DIR — equal to the executable directory
# under this project's single-config (Ninja) setup.
set(_deployed_optixir)
foreach(_ir IN LISTS QUALQUER_OPTIX_IR_FILES)
    get_filename_component(_ir_name "${_ir}" NAME)
    set(_dst "${CMAKE_CURRENT_BINARY_DIR}/shaders/${_ir_name}")
    add_custom_command(
        OUTPUT "${_dst}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_ir}" "${_dst}"
        DEPENDS "${_ir}"
        COMMENT "Deploying OptiX IR: ${_ir_name}"
        VERBATIM
    )
    list(APPEND _deployed_optixir "${_dst}")
endforeach()
target_sources(qualquer_app PRIVATE ${_deployed_optixir})
