/**
 * @file pipeline.cpp
 * @brief OptiX layer: ray tracing pipeline implementation.
 */

#include <qualquer/optix/pipeline.h>
#include <qualquer/optix/optix_check.h>

#include <array>
#include <fstream>
#include <sstream>
#include <string>

#include <optix_stubs.h>
#include <spdlog/spdlog.h>

namespace qualquer::optix {
    namespace {
        /**
         * @brief OptiX module debug-info level, selected by build type.
         *
         * Release builds strip debug info (NONE) for the smallest shader size;
         * debug builds keep line info (MINIMAL) so OptiX validation and exception
         * reports can point at the offending source line. MINIMAL is chosen over
         * higher levels because it carries no runtime cost.
         */
#ifdef NDEBUG
        constexpr OptixCompileDebugLevel kModuleDebugLevel = OPTIX_COMPILE_DEBUG_LEVEL_NONE;
#else
        constexpr OptixCompileDebugLevel kModuleDebugLevel = OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL;
#endif

        /**
         * @brief Reads a file as raw bytes into a string.
         *
         * Binary mode is required: a .optixir blob contains embedded null bytes
         * that a text-mode read would truncate at, and optixModuleCreate consumes
         * the full byte range (pointer + size). Aborts on open failure since a
         * missing shader is an unrecoverable configuration error.
         * @param path File path to read.
         * @return File contents as a binary-safe string.
         */
        std::string read_file(const std::string &path) {
            const std::ifstream file(path, std::ios::binary);
            if (!file) {
                spdlog::critical("Failed to open OptiX IR file: {}", path);
                std::abort();
            }
            std::ostringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }
    } // namespace

    void Pipeline::init(const OptixDeviceContext device_context,
                        const std::string &optixir_path,
                        const std::size_t launch_params_size,
                        const char *launch_params_variable_name) {
        const std::string optixir = read_file(optixir_path);

        constexpr OptixModuleCompileOptions module_options{
            .maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT,
            .optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT,
            .debugLevel = kModuleDebugLevel,
        };

        // traversableGraphFlags selects single-level instancing (TLAS->BLAS), the
        // graph shape used for instanced geometry, over the broader ALLOW_ANY so
        // the traversal shader compiles for the actual graph depth rather than the
        // most general one. numPayloadValues=0: the device programs issue no
        // optixTrace, so no payload registers are reserved, and without optixTrace
        // there is no traversal, so the graph flag carries no runtime cost.
        const OptixPipelineCompileOptions pipeline_options{
            .traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING,
            .numPayloadValues = 0,
            .numAttributeValues = 2,
            .pipelineLaunchParamsVariableName = launch_params_variable_name,
            .pipelineLaunchParamsSizeInBytes = launch_params_size,
        };

        OPTIX_CHECK(optixModuleCreate(device_context,
                                      &module_options,
                                      &pipeline_options,
                                      optixir.c_str(),
                                      optixir.size(),
                                      nullptr,
                                      nullptr,
                                      &module_));

        // Entry function names are the extern "C" symbols defined in the device
        // program source; OptiX resolves each program group by matching these
        // symbol names. The hit group pairs closest-hit with any-hit so a single
        // SBT hit record covers both stages.
        const std::array<OptixProgramGroupDesc, 3> program_descs{{
            {
                .kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN,
                .raygen = {.module = module_, .entryFunctionName = "__raygen__rg"},
            },
            {
                .kind = OPTIX_PROGRAM_GROUP_KIND_MISS,
                .miss = {.module = module_, .entryFunctionName = "__miss__ms"},
            },
            {
                .kind = OPTIX_PROGRAM_GROUP_KIND_HITGROUP,
                .hitgroup = {
                    .moduleCH = module_,
                    .entryFunctionNameCH = "__closesthit__ch",
                    .moduleAH = module_,
                    .entryFunctionNameAH = "__anyhit__ah",
                },
            },
        }};

        std::array<OptixProgramGroup, 3> program_groups{};
        OPTIX_CHECK(optixProgramGroupCreate(device_context,
                                            program_descs.data(),
                                            program_descs.size(),
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            program_groups.data()));
        raygen_program = program_groups[0];
        miss_program = program_groups[1];
        hitgroup_program = program_groups[2];

        // maxTraceDepth=0 is permitted by OptiX: raygen launches but traces no
        // rays (no optixTrace call), so no continuation stack for tracing is
        // reserved.
        constexpr OptixPipelineLinkOptions link_options{
            .maxTraceDepth = 0,
        };

        OPTIX_CHECK(optixPipelineCreate(device_context,
                                        &pipeline_options,
                                        &link_options,
                                        program_groups.data(),
                                        program_groups.size(),
                                        nullptr,
                                        nullptr,
                                        &handle));

        // maxTraversableGraphDepth=0 lets OptiX derive the default from
        // traversableGraphFlags (single-level instancing -> depth 2), matching the
        // TLAS->BLAS graph depth. Trace and callable depths are 0 since no
        // optixTrace or callable programs are used.
        OPTIX_CHECK(optixPipelineSetStackSizeFromCallDepths(
            handle,
            /*maxTraceDepth=*/0,
            /*maxContinuationCallableDepth=*/0,
            /*maxDirectCallableDepthFromState=*/0,
            /*maxDirectCallableDepthFromTraversal=*/0,
            /*maxTraversableGraphDepth=*/0));

        spdlog::info("OptiX pipeline created ({} program groups)", program_groups.size());
    }

    void Pipeline::destroy() {
        // Reverse creation order: the pipeline links the program groups, which in
        // turn reference the module, so each object is destroyed before the one it
        // depends on. Idempotent via the null resets.
        if (handle != nullptr) {
            OPTIX_CHECK(optixPipelineDestroy(handle));
            handle = nullptr;
        }
        if (raygen_program != nullptr) {
            OPTIX_CHECK(optixProgramGroupDestroy(raygen_program));
            raygen_program = nullptr;
        }
        if (miss_program != nullptr) {
            OPTIX_CHECK(optixProgramGroupDestroy(miss_program));
            miss_program = nullptr;
        }
        if (hitgroup_program != nullptr) {
            OPTIX_CHECK(optixProgramGroupDestroy(hitgroup_program));
            hitgroup_program = nullptr;
        }
        if (module_ != nullptr) {
            OPTIX_CHECK(optixModuleDestroy(module_));
            module_ = nullptr;
        }
    }
} // namespace qualquer::optix
