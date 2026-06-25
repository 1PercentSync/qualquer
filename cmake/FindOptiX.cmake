# FindOptiX.cmake
# Locates the OptiX SDK include directory.
#
# The OptiX SDK is header-only; no libraries need to be linked.
# Output variables:
#   OptiX_FOUND        - TRUE if optix.h was found
#   OptiX_INCLUDE_DIR  - Path to the directory containing optix.h

find_path(OptiX_INCLUDE_DIR
    NAMES optix.h
    PATHS "C:/ProgramData/NVIDIA Corporation/OptiX SDK 9.1.0/include"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OptiX
    REQUIRED_VARS OptiX_INCLUDE_DIR
)

mark_as_advanced(OptiX_INCLUDE_DIR)
