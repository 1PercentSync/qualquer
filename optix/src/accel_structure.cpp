/**
 * @file accel_structure.cpp
 * @brief OptiX acceleration structure builder implementation.
 */

#include <qualquer/optix/accel_structure.h>

#include <algorithm>

#include <qualquer/optix/cuda_check.h>
#include <qualquer/optix/optix_check.h>

#include <spdlog/spdlog.h>

namespace qualquer::optix {
    namespace {
        /// Per-BLAS state shared across build phases.
        struct BuildInfo {
            std::vector<unsigned int> geometry_flags;
            std::vector<OptixBuildInput> build_inputs;
            CudaBuffer<uint8_t> output_buffer;
            OptixTraversableHandle uncompacted_handle = 0;
            size_t output_size = 0;
        };

        /// Fills build inputs for one BLAS group and queries buffer sizes.
        /// @return Scratch (temp) buffer size needed for this group.
        // ReSharper disable once CppParameterMayBeConst
        size_t prepare_group_inputs(OptixDeviceContext context,
                                    const OptixAccelBuildOptions &accel_options,
                                    const std::span<const BLASGeometry> geoms,
                                    BuildInfo &info) {
            const auto geom_count = static_cast<unsigned int>(geoms.size());
            info.geometry_flags.resize(geom_count);
            info.build_inputs.resize(geom_count);

            for (unsigned int j = 0; j < geom_count; ++j) {
                const auto &geom = geoms[j];
                info.geometry_flags[j] = geom.opaque
                                             ? OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT
                                             : OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL;

                auto &input = info.build_inputs[j];
                input = {};
                input.type = OPTIX_BUILD_INPUT_TYPE_TRIANGLES;
                auto &tri = input.triangleArray;
                tri.vertexBuffers = &geom.vertex_buffer;
                tri.numVertices = geom.vertex_count;
                tri.vertexFormat = OPTIX_VERTEX_FORMAT_FLOAT3;
                tri.vertexStrideInBytes = geom.vertex_stride;
                tri.indexBuffer = geom.index_buffer;
                tri.numIndexTriplets = geom.index_count / 3;
                tri.indexFormat = OPTIX_INDICES_FORMAT_UNSIGNED_INT3;
                tri.flags = &info.geometry_flags[j];
                tri.numSbtRecords = 1;
            }

            OptixAccelBufferSizes sizes{};
            OPTIX_CHECK(optixAccelComputeMemoryUsage(context,
                &accel_options,
                info.build_inputs.data(),
                geom_count,
                &sizes));

            info.output_size = sizes.outputSizeInBytes;
            return sizes.tempSizeInBytes;
        }

        void log_blas_compaction(size_t blas_index, const size_t original_size, const uint64_t compacted_size) {
            if (compacted_size < original_size) {
                spdlog::info("BLAS #{}: {:.1f} KB -> {:.1f} KB (compacted {:.0f}%)",
                             blas_index,
                             static_cast<double>(original_size) / 1024.0,
                             static_cast<double>(compacted_size) / 1024.0,
                             (1.0 - static_cast<double>(compacted_size) / static_cast<double>(original_size)) * 100.0);
            } else {
                spdlog::info("BLAS #{}: {:.1f} KB (compaction skipped)",
                             blas_index,
                             static_cast<double>(original_size) / 1024.0);
            }
        }

        /// Compacts an AS if the compacted size is smaller; otherwise moves the
        /// source buffer as-is. Does not synchronize — caller must sync before
        /// accessing the result.
        // ReSharper disable once CppParameterMayBeConst
        void compact_as(OptixDeviceContext context,
                        const OptixTraversableHandle uncompacted_handle,
                        CudaBuffer<uint8_t> &source,
                        const uint64_t compacted_size,
                        CudaBuffer<uint8_t> &dest_buffer,
                        OptixTraversableHandle &dest_handle) {
            if (compacted_size < source.size_bytes()) {
                dest_buffer.alloc(compacted_size);
                OPTIX_CHECK(optixAccelCompact(
                    context,
                    nullptr,
                    uncompacted_handle,
                    dest_buffer.device_ptr(),
                    dest_buffer.size_bytes(),
                    &dest_handle));
            } else {
                dest_buffer = std::move(source);
                dest_handle = uncompacted_handle;
            }
        }
    } // anonymous namespace

    // ReSharper disable once CppParameterMayBeConst
    void AccelStructure::build_all_blas(OptixDeviceContext context,
                                        const std::vector<std::span<const BLASGeometry> > &groups) {
        if (groups.empty()) {
            return;
        }

        constexpr OptixAccelBuildOptions accel_options{
            .buildFlags = OPTIX_BUILD_FLAG_PREFER_FAST_TRACE | OPTIX_BUILD_FLAG_ALLOW_COMPACTION,
            .operation = OPTIX_BUILD_OPERATION_BUILD,
            .motionOptions = {},
        };

        const auto count = groups.size();

        std::vector<BuildInfo> infos(count);

        // --- Phase 1: prepare build inputs, query memory sizes ---
        size_t max_scratch = 0;
        for (size_t i = 0; i < count; ++i) {
            max_scratch = std::max(max_scratch,
                                   prepare_group_inputs(context, accel_options, groups[i], infos[i]));
        }

        // --- Phase 2: submit all builds (shared scratch, per-BLAS output) ---
        // Stream ordering guarantees build N completes before N+1 starts,
        // so a single scratch buffer sized to the maximum is safe to reuse.
        CudaBuffer<uint8_t> scratch;
        scratch.alloc(max_scratch);

        CudaBuffer<uint64_t> compacted_sizes_buf;
        compacted_sizes_buf.alloc(count);

        for (size_t i = 0; i < count; ++i) {
            auto &info = infos[i];
            info.output_buffer.alloc(info.output_size);

            const OptixAccelEmitDesc emit_desc{
                .result = reinterpret_cast<CUdeviceptr>(compacted_sizes_buf.data() + i),
                .type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE,
            };

            OPTIX_CHECK(optixAccelBuild(context,
                nullptr,
                &accel_options,
                info.build_inputs.data(),
                static_cast<unsigned int>(info.build_inputs.size()),
                scratch.device_ptr(),
                scratch.size_bytes(),
                info.output_buffer.device_ptr(),
                info.output_buffer.size_bytes(),
                &info.uncompacted_handle,
                &emit_desc,
                1));
        }

        // --- Sync 1: all builds complete, compacted sizes available ---
        CUDA_CHECK(cudaStreamSynchronize(nullptr));
        scratch.destroy();

        std::vector<uint64_t> compacted_sizes(count);
        CUDA_CHECK(cudaMemcpy(compacted_sizes.data(),
            compacted_sizes_buf.data(),
            count * sizeof(uint64_t),
            cudaMemcpyDeviceToHost));
        compacted_sizes_buf.destroy();

        // --- Phase 3: submit all compactions ---
        const auto base_index = blas_handles_.size();
        blas_handles_.resize(base_index + count);

        for (size_t i = 0; i < count; ++i) {
            compact_as(context,
                       infos[i].uncompacted_handle,
                       infos[i].output_buffer,
                       compacted_sizes[i],
                       blas_handles_[base_index + i].buffer,
                       blas_handles_[base_index + i].handle);
        }

        // --- Sync 2: all compactions complete, uncompacted buffers can be freed ---
        CUDA_CHECK(cudaStreamSynchronize(nullptr));

        for (size_t i = 0; i < count; ++i) {
            log_blas_compaction(base_index + i, infos[i].output_size, compacted_sizes[i]);
        }
    }

    void AccelStructure::build_tlas(OptixDeviceContext context, const std::span<const OptixInstance> instances) {
        const auto instance_count = static_cast<unsigned int>(instances.size());

        // --- Upload instance array to device ---
        CudaBuffer<OptixInstance> instance_buffer;
        instance_buffer.alloc(instance_count);
        CUDA_CHECK(cudaMemcpy(instance_buffer.data(),
            instances.data(),
            instance_buffer.size_bytes(),
            cudaMemcpyHostToDevice));

        // --- Build input ---
        OptixBuildInput build_input{};
        build_input.type = OPTIX_BUILD_INPUT_TYPE_INSTANCES;
        build_input.instanceArray.instances = instance_buffer.device_ptr();
        build_input.instanceArray.numInstances = instance_count;

        // --- Build options ---
        constexpr OptixAccelBuildOptions accel_options{
            .buildFlags = OPTIX_BUILD_FLAG_PREFER_FAST_TRACE | OPTIX_BUILD_FLAG_ALLOW_COMPACTION,
            .operation = OPTIX_BUILD_OPERATION_BUILD,
            .motionOptions = {},
        };

        // --- Query memory sizes ---
        OptixAccelBufferSizes buffer_sizes{};
        OPTIX_CHECK(optixAccelComputeMemoryUsage(context, &accel_options,&build_input, 1, &buffer_sizes));

        // --- Allocate temp and output buffers ---
        CudaBuffer<uint8_t> temp_buffer;
        temp_buffer.alloc(buffer_sizes.tempSizeInBytes);

        CudaBuffer<uint8_t> output_buffer;
        output_buffer.alloc(buffer_sizes.outputSizeInBytes);

        // --- Build, emitting compacted size ---
        CudaBuffer<uint64_t> compacted_size_buffer;
        compacted_size_buffer.alloc(1);

        const OptixAccelEmitDesc emit_desc{
            .result = compacted_size_buffer.device_ptr(),
            .type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE,
        };

        OPTIX_CHECK(optixAccelBuild(context,
            nullptr,
            &accel_options,
            &build_input,
            1,
            temp_buffer.device_ptr(),
            temp_buffer.size_bytes(),
            output_buffer.device_ptr(),
            output_buffer.size_bytes(),
            &tlas_.handle,
            &emit_desc,
            1));

        CUDA_CHECK(cudaStreamSynchronize(nullptr));

        temp_buffer.destroy();

        // --- Read back compacted size ---
        uint64_t compacted_size = 0;
        CUDA_CHECK(cudaMemcpy(&compacted_size,
            compacted_size_buffer.data(),
            sizeof(uint64_t),
            cudaMemcpyDeviceToHost));
        compacted_size_buffer.destroy();

        const auto output_size = output_buffer.size_bytes();
        compact_as(context,
                   tlas_.handle,
                   output_buffer,
                   compacted_size,
                   tlas_.buffer,
                   tlas_.handle);

        CUDA_CHECK(cudaStreamSynchronize(nullptr));

        if (compacted_size < output_size) {
            spdlog::info("TLAS: {} instances, {:.1f} KB -> {:.1f} KB (compacted {:.0f}%)",
                         instance_count,
                         static_cast<double>(output_size) / 1024.0,
                         static_cast<double>(compacted_size) / 1024.0,
                         (1.0 - static_cast<double>(compacted_size) / static_cast<double>(output_size)) * 100.0);
        } else {
            spdlog::info("TLAS: {} instances, {:.1f} KB (compaction skipped)",
                         instance_count,
                         static_cast<double>(output_size) / 1024.0);
        }
    }

    void AccelStructure::destroy() {
        for (auto &blas: blas_handles_) {
            blas.buffer.destroy();
            blas.handle = 0;
        }
        blas_handles_.clear();

        tlas_.buffer.destroy();
        tlas_.handle = 0;
    }

    OptixTraversableHandle AccelStructure::tlas_handle() const {
        return tlas_.handle;
    }

    const std::vector<BLASHandle> &AccelStructure::blas_handles() const {
        return blas_handles_;
    }
} // namespace qualquer::optix
