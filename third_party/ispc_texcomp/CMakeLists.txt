add_library(ispc_texcomp STATIC
        src/ispc_texcomp.cpp
        src/kernel.ispc
)

# Public: qualquer-style include path (e.g. #include <qualquer/ispc_texcomp/ispc_texcomp.h>)
target_include_directories(ispc_texcomp PUBLIC include)

# Public: ISPC auto-generated headers (e.g. kernel_ispc.h) land in the binary dir
target_include_directories(ispc_texcomp PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Private: ispc_texcomp source includes headers relative to include/qualquer/ispc_texcomp/
target_include_directories(ispc_texcomp PRIVATE include/qualquer/ispc_texcomp)

# ISPC: target multiple SIMD widths; ISPC runtime picks the best at call time
set_source_files_properties(src/kernel.ispc PROPERTIES
        COMPILE_OPTIONS "--opt=fast-math;--opt=disable-assertions"
)
set_property(TARGET ispc_texcomp PROPERTY ISPC_INSTRUCTION_SETS sse2-i32x4;sse4-i32x4;avx2-i32x8;avx512icl-x16)

# Suppress warnings in third-party code (CXX only; ISPC has its own flags)
target_compile_options(ispc_texcomp PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/W0>)
