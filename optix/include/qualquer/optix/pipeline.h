#pragma once

/**
 * @file pipeline.h
 * @brief OptiX layer: ray tracing pipeline.
 */

#include <cstddef>
#include <string>

#include <optix.h>

namespace qualquer::optix {
    /**
     * @brief Owns an OptiX ray tracing pipeline and its building blocks.
     *
     * Holds the compiled Module (loaded from a .optixir file), the ProgramGroup
     * handles, and the linked OptixPipeline. The program-group handles and the
     * pipeline handle are exposed publicly: the renderer layer packs SBT records
     * from the program groups and passes the pipeline handle to optixLaunch, so
     * this layer does not own or build the SBT. Lifetime is managed explicitly
     * via init() and destroy().
     */
    class Pipeline {
    public:
        /**
         * @brief Builds the pipeline from a compiled .optixir file.
         *
         * Reads the file, creates the Module, creates the three program groups
         * (raygen, miss, hitgroup containing closest-hit + any-hit), links the
         * pipeline, and configures the stack size.
         * @param device_context              OptiX device context to build against.
         * @param optixir_path                Path to the compiled .optixir file.
         * @param launch_params_size          Size of the launch-params struct in bytes
         *                                    (sizeof of the renderer's LaunchParams).
         *                                    Passed in rather than taken here because
         *                                    this layer cannot include the renderer
         *                                    header (single-direction dependency).
         * @param launch_params_variable_name Name of the device-side
         *                                    extern __constant__ launch-params variable.
         */
        void init(OptixDeviceContext device_context,
                  const std::string &optixir_path,
                  std::size_t launch_params_size,
                  const char *launch_params_variable_name);

        /**
         * @brief Releases the pipeline, program groups, and module.
         *
         * Idempotent: members are reset, so a repeat call is a no-op (matches
         * the optix layer's destroy convention).
         */
        void destroy();

        /** @brief Linked OptiX pipeline handle for optixLaunch; null before init and after destroy. */
        OptixPipeline handle = nullptr;

        /** @brief Ray-generation program group; null before init and after destroy. */
        OptixProgramGroup raygen_program = nullptr;

        /** @brief Miss program group; null before init and after destroy. */
        OptixProgramGroup miss_program = nullptr;

        /** @brief Hit-group program (closest-hit + any-hit); null before init and after destroy. */
        OptixProgramGroup hitgroup_program = nullptr;

    private:
        /** @brief Compiled OptiX module loaded from the .optixir file; null before init and after destroy. */
        OptixModule module_ = nullptr;
    };
} // namespace qualquer::optix
