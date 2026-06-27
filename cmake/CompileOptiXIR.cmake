# CompileOptiXIR.cmake
#
# Build-time compilation of OptiX device programs to OptiX IR (.optixir),
# consumed at runtime by optixModuleCreate.

## compile_optix_ir(target file1.cu [file2.cu ...])
#
# Compiles each .cu source with nvcc --optix-ir into
# ${CMAKE_BINARY_DIR}/optix_ir/, then deploys all generated .optixir files into
# <target runtime dir>/shaders/ at POST_BUILD.
#
# Prerequisites at the call site:
#   - CUDA language enabled (defines CMAKE_CUDA_COMPILER)
#   - OptiX_INCLUDE_DIR defined (via find_package(OptiX))
function(compile_optix_ir target)
    set(_cu_files ${ARGN})

    # Collect -I flags: the target's own include directories (so device
    # programs can include project headers) plus the OptiX SDK headers.
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

    set(_optixir_files)
    foreach(_cu ${_cu_files})
        if(NOT IS_ABSOLUTE "${_cu}")
            set(_cu_abs "${CMAKE_CURRENT_SOURCE_DIR}/${_cu}")
        else()
            set(_cu_abs "${_cu}")
        endif()

        get_filename_component(_name "${_cu_abs}" NAME_WE)
        set(_out "${CMAKE_BINARY_DIR}/optix_ir/${_name}.optixir")

        # --optix-ir cannot combine with --compile: nvcc emits the .optixir
        # directly as a single phase. compute_89 is the virtual architecture
        # required here because OptiX IR is JIT-compiled by the OptiX runtime to
        # the actual GPU, so it must stay at the virtual level (never sm_XX).
        # -Xcompiler=/utf-8 mirrors the top-level add_compile_options for CUDA:
        # this custom command bypasses it, and without it MSVC under code page
        # 936 spams C4819 on non-ASCII bytes inside NVIDIA's own CUDA headers.
        add_custom_command(
            OUTPUT "${_out}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/optix_ir"
            COMMAND "${CMAKE_CUDA_COMPILER}"
                    --optix-ir
                    --gpu-architecture=compute_89
                    -std=c++20
                    -Xcompiler=/utf-8
                    ${_include_flags}
                    -o "${_out}"
                    "${_cu_abs}"
            DEPENDS "${_cu_abs}"
            COMMENT "Compiling OptiX IR: ${_name}"
            VERBATIM
        )

        list(APPEND _optixir_files "${_out}")
    endforeach()

    # Deploy compiled artifacts to the target's runtime shader directory.
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${target}>/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${_optixir_files}
                "$<TARGET_FILE_DIR:${target}>/shaders"
        COMMENT "Deploying OptiX IR to $<TARGET_FILE_DIR:${target}>/shaders"
        VERBATIM
    )

    # Wire the generated files as sources so the target depends on them. CMake
    # has no rule to compile .optixir, so this only drives the custom command
    # ordering without producing an object file.
    target_sources(${target} PRIVATE ${_optixir_files})
endfunction()
