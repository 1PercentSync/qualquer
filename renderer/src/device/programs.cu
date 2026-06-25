/**
 * @file programs.cu
 * @brief OptiX device programs (renderer layer).
 */

#include <optix_device.h>

// OptiX locates program groups by the entry function symbol name (e.g.
// "__raygen__rg"), so entry points must be extern "C" at global scope.
extern "C" {

/// OptiX ray generation entry point.
__global__ void __raygen__rg() {
}

}  // extern "C"
