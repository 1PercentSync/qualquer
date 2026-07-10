add_library(dlss STATIC IMPORTED GLOBAL)

set_target_properties(dlss PROPERTIES
    IMPORTED_CONFIGURATIONS "Debug;Release;RelWithDebInfo"
    IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib/nvsdk_ngx_d.lib"
    IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/lib/nvsdk_ngx_d.lib"
    IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/lib/nvsdk_ngx_d_dbg.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Runtime DLL paths for consumer targets to deploy via POST_BUILD copy.
# The NGX loader (nvsdk_ngx_d.lib) loads nvngx_dlssd.dll at runtime;
# it must be beside the executable.
set(DLSS_RUNTIME_DLL_DEV "${CMAKE_CURRENT_SOURCE_DIR}/bin/dev/nvngx_dlssd.dll" CACHE INTERNAL "")
set(DLSS_RUNTIME_DLL_REL "${CMAKE_CURRENT_SOURCE_DIR}/bin/rel/nvngx_dlssd.dll" CACHE INTERNAL "")
