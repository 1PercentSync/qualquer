/**
 * @file programs.cu
 * @brief OptiX device programs (renderer layer).
 */

#include <cstdint>

#include <optix_device.h>

#include <qualquer/renderer/launch_params.h>

// OptiX locates program groups by the entry function symbol name (e.g.
// "__raygen__rg"), so entry points must be extern "C" at global scope. The
// launch-params constant is bound by name to pipelineLaunchParamsVariableName.
extern "C" {
__constant__ qualquer::renderer::LaunchParams params;

/// Ray generation entry point: writes a solid color into the accumulation
/// buffer at the launch index.
__global__ void __raygen__rg() { // NOLINT(*-reserved-identifier)
    const uint3 launch_index = optixGetLaunchIndex();
    const uint32_t linear_index = launch_index.y * params.width + launch_index.x;
    params.accumulation_buffer[linear_index] = make_float4(0.1f, 0.2f, 0.3f, 1.0f);
}

/// Miss entry point.
__global__ void __miss__ms() { // NOLINT(*-reserved-identifier)
}

/// Closest-hit entry point.
__global__ void __closesthit__ch() { // NOLINT(*-reserved-identifier)
}

/// Any-hit entry point.
__global__ void __anyhit__ah() { // NOLINT(*-reserved-identifier)
}

}  // extern "C"
