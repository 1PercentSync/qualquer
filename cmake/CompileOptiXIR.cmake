# CompileOptiXIR.cmake
#
# Build-time compilation of OptiX device programs to OptiX IR (.optixir),
# consumed at runtime by optixModuleCreate.

## compile_optix_ir(target file1.cu [file2.cu ...])
#
# Compiles each .cu source with nvcc --optix-ir into ${QUALQUER_OPTIX_IR_DIR}.
# Deployment to a runtime directory is the consumer's responsibility: the app
# target POST_BUILD-copies the whole directory to its executable's shaders/ so
# Pipeline::init can load the .optixir via a CWD-relative path.
#
# Prerequisites at the call site:
#   - CUDA language enabled (defines CMAKE_CUDA_COMPILER)
#   - OptiX_INCLUDE_DIR defined (via find_package(OptiX))

# Directory where compiled .optixir artifacts land; exported as a global
# variable so consumers (e.g. the app target's POST_BUILD copy) can reference
# it without hardcoding this module's internal layout.
set(QUALQUER_OPTIX_IR_DIR "${CMAKE_BINARY_DIR}/optix_ir" CACHE INTERNAL "OptiX IR output directory")

function(compile_optix_ir target)
    set(_cu_files ${ARGN})

    # -lineinfo (Debug only) embeds source line mapping so OptiX's MINIMAL debug
    # level (requested for Debug builds in Pipeline::init) has line info to point
    # at; line info carries no runtime cost, only a larger .optixir. Decided at
    # configure time via CMAKE_BUILD_TYPE rather than a $<CONFIG:Debug> generator
    # expression: under add_custom_command + VERBATIM, a false generator expansion
    # is preserved as an empty argument, which nvcc treats as an input file.
    set(_lineinfo_flag)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_lineinfo_flag -lineinfo)
    endif()

    # Device programs include project and OptiX SDK headers, resolved from the
    # target's include directories and the SDK path.
    get_target_property(_target_includes ${target} INCLUDE_DIRECTORIES)
    if(_target_includes STREQUAL "_target_includes-NOTFOUND")
        set(_target_includes)
    endif()
    set(_include_flags)
    foreach(_inc ${_target_includes})
        get_filename_component(_abs_inc "${_inc}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
        list(APPEND _include_flags "-I${_abs_inc}")
    endforeach()
    list(APPEND _include_flags "-I${OptiX_INCLUDE_DIR}")

    # Collect all device-visible headers so changes trigger recompilation.
    # Both .cuh and .h files are included (directly or transitively) by the
    # .cu sources (e.g. launch_params.h). A flat glob is simpler and more
    # robust than manually listing each header.
    file(GLOB_RECURSE _cuh_deps
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cuh"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    )

    set(_optixir_files)
    foreach(_cu ${_cu_files})
        if(NOT IS_ABSOLUTE "${_cu}")
            set(_cu_abs "${CMAKE_CURRENT_SOURCE_DIR}/${_cu}")
        else()
            set(_cu_abs "${_cu}")
        endif()

        get_filename_component(_name "${_cu_abs}" NAME_WE)
        set(_out "${QUALQUER_OPTIX_IR_DIR}/${_name}.optixir")

        # --optix-ir cannot combine with --compile: nvcc emits the .optixir
        # directly as a single phase. compute_89 is the virtual architecture
        # required here because OptiX IR is JIT-compiled by the OptiX runtime to
        # the actual GPU, so it must stay at the virtual level (never sm_XX).
        # -Xcompiler=/utf-8 mirrors the top-level add_compile_options for CUDA:
        # this custom command bypasses it, and without it MSVC under code page
        # 936 spams C4819 on non-ASCII bytes inside NVIDIA's own CUDA headers.
        # Clear the OptiX JIT cache when device code is recompiled. The cache
        # keys the compiled SASS by .optixir content, but stale entries have
        # been observed to cause the runtime to use an outdated kernel. Removing
        # the cache file forces a fresh JIT on the next launch.
        set(_optix_cache "$ENV{LOCALAPPDATA}/NVIDIA/OptixCache/optix7cache.db")

        add_custom_command(
            OUTPUT "${_out}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${QUALQUER_OPTIX_IR_DIR}"
            COMMAND "${CMAKE_CUDA_COMPILER}"
                    --optix-ir
                    --gpu-architecture=compute_89
                    -std=c++20
                    -Xcompiler=/utf-8
                    ${_lineinfo_flag}
                    ${_include_flags}
                    -o "${_out}"
                    "${_cu_abs}"
            COMMAND ${CMAKE_COMMAND} -E rm -f "${_optix_cache}"
            DEPENDS "${_cu_abs}" ${_cuh_deps}
            COMMENT "Compiling OptiX IR: ${_name}"
            VERBATIM
        )

        list(APPEND _optixir_files "${_out}")
    endforeach()

    # Wire the generated files as sources so the target depends on them. CMake
    # has no rule to compile .optixir, so this only drives the custom command
    # ordering without producing an object file.
    target_sources(${target} PRIVATE ${_optixir_files})
endfunction()
