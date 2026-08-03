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
        constexpr OptixAccelBuildOptions kAccelOptions{
            .buildFlags = OPTIX_BUILD_FLAG_PREFER_FAST_TRACE
                          | OPTIX_BUILD_FLAG_ALLOW_COMPACTION,
            .operation = OPTIX_BUILD_OPERATION_BUILD,
            .motionOptions = {},
        };

        /** @brief OptiX AS output buffers require 128-byte alignment. */
        constexpr std::size_t kAsAlignment = OPTIX_ACCEL_BUFFER_BYTE_ALIGNMENT;
    }

    std::vector<BLASBuildInfo> AccelStructure::build_blas_uncompacted(
        // ReSharper disable CppParameterMayBeConst
        OptixDeviceContext context, CUstream stream,
        // ReSharper restore CppParameterMayBeConst
        const std::vector<std::span<const BLASGeometry>> &groups) {

        if (groups.empty()) {
            return {};
        }

        const auto count = groups.size();

        // Per-BLAS state for the build phase.
        struct BuildState {
            std::vector<unsigned int> geometry_flags;
            std::vector<OptixBuildInput> build_inputs;
            std::size_t output_size = 0;
        };
        std::vector<BuildState> states(count);

        // --- Phase 1: prepare build inputs, query memory sizes ---

        std::size_t max_scratch = 0;
        for (std::size_t i = 0; i < count; ++i) {
            auto &state = states[i];
            const auto &geoms = groups[i];
            const auto geom_count = static_cast<unsigned int>(geoms.size());

            state.geometry_flags.resize(geom_count);
            state.build_inputs.resize(geom_count);

            for (unsigned int j = 0; j < geom_count; ++j) {
                const auto &geom = geoms[j];
                state.geometry_flags[j] = geom.opaque
                    ? OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT
                    : OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL;

                auto &input = state.build_inputs[j];
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
                tri.flags = &state.geometry_flags[j];
                tri.numSbtRecords = 1;
            }

            OptixAccelBufferSizes sizes{};
            OPTIX_CHECK(optixAccelComputeMemoryUsage(
                context, &kAccelOptions,
                state.build_inputs.data(), geom_count, &sizes));

            state.output_size = sizes.outputSizeInBytes;
            max_scratch = std::max(max_scratch, sizes.tempSizeInBytes);
        }

        // --- Phase 2: submit all builds (shared scratch, per-BLAS output) ---

        CudaBuffer<uint8_t> scratch;
        scratch.alloc(max_scratch);

        CudaBuffer<uint64_t> compacted_sizes_buf;
        compacted_sizes_buf.alloc(count);

        blas_uncompacted_.resize(count);
        std::vector<OptixTraversableHandle> uncompacted_handles(count);

        for (std::size_t i = 0; i < count; ++i) {
            auto &state = states[i];
            blas_uncompacted_[i].alloc(state.output_size);

            const OptixAccelEmitDesc emit_desc{
                .result = reinterpret_cast<CUdeviceptr>(compacted_sizes_buf.data() + i),
                .type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE,
            };

            OPTIX_CHECK(optixAccelBuild(
                context, stream,
                &kAccelOptions,
                state.build_inputs.data(),
                static_cast<unsigned int>(state.build_inputs.size()),
                scratch.device_ptr(), scratch.size_bytes(),
                blas_uncompacted_[i].device_ptr(), blas_uncompacted_[i].size_bytes(),
                &uncompacted_handles[i],
                &emit_desc, 1));
        }

        // --- Sync: all builds complete, compacted sizes available ---

        CUDA_CHECK(cudaStreamSynchronize(stream));
        scratch.free();

        std::vector<uint64_t> compacted_sizes(count);
        CUDA_CHECK(cudaMemcpy(compacted_sizes.data(), compacted_sizes_buf.data(),
                              count * sizeof(uint64_t), cudaMemcpyDeviceToHost));
        compacted_sizes_buf.free();

        // Build the result vector.
        std::vector<BLASBuildInfo> infos(count);
        for (std::size_t i = 0; i < count; ++i) {
            infos[i] = {
                .compacted_size = compacted_sizes[i],
                .uncompacted_handle = uncompacted_handles[i],
            };
            spdlog::info("BLAS #{}: {:.1f} KB uncompacted, {:.1f} KB compacted ({:.0f}%)",
                         i,
                         static_cast<double>(blas_uncompacted_[i].size_bytes()) / 1024.0,
                         static_cast<double>(compacted_sizes[i]) / 1024.0,
                         (1.0 - static_cast<double>(compacted_sizes[i])
                              / static_cast<double>(blas_uncompacted_[i].size_bytes())) * 100.0);
        }
        return infos;
    }

    void AccelStructure::compact_blas_into_pool(
        // ReSharper disable CppParameterMayBeConst
        OptixDeviceContext context, CUstream stream,
        // ReSharper restore CppParameterMayBeConst
        CudaDevicePool &pool,
        const std::vector<BLASBuildInfo> &infos) {

        const auto count = infos.size();
        const auto base_index = blas_handles_.size();
        blas_handles_.resize(base_index + count);

        for (std::size_t i = 0; i < count; ++i) {
            auto *dest = pool.suballocate(infos[i].compacted_size, kAsAlignment);
            const auto dest_ptr = reinterpret_cast<CUdeviceptr>(dest);

            OPTIX_CHECK(optixAccelCompact(
                context, stream,
                infos[i].uncompacted_handle,
                dest_ptr, infos[i].compacted_size,
                &blas_handles_[base_index + i].handle));
        }

        // --- Sync: all compactions complete, uncompacted buffers can be freed ---

        CUDA_CHECK(cudaStreamSynchronize(stream));

        // Free uncompacted output buffers.
        blas_uncompacted_.clear();
    }

    std::size_t AccelStructure::query_tlas_size(
        const OptixDeviceContext context,
        const unsigned int instance_count) {

        // optixAccelComputeMemoryUsage is a host-side calculation that only
        // reads numInstances for IAS sizing. The instances pointer is not
        // dereferenced, so a placeholder suffices.
        OptixBuildInput build_input{};
        build_input.type = OPTIX_BUILD_INPUT_TYPE_INSTANCES;
        // Placeholder must satisfy OPTIX_INSTANCE_BYTE_ALIGNMENT (16).
        // The pointer is validated but not dereferenced for size queries.
        build_input.instanceArray.instances = OPTIX_INSTANCE_BYTE_ALIGNMENT;
        build_input.instanceArray.numInstances = instance_count;

        OptixAccelBufferSizes sizes{};
        OPTIX_CHECK(optixAccelComputeMemoryUsage(
            context, &kAccelOptions, &build_input, 1, &sizes));

        return sizes.outputSizeInBytes;
    }

    void AccelStructure::build_tlas_into_pool(
        // ReSharper disable CppParameterMayBeConst
        OptixDeviceContext context, CUstream stream,
        // ReSharper restore CppParameterMayBeConst
        CudaDevicePool &pool,
        const std::span<const OptixInstance> instances) {

        const auto instance_count = static_cast<unsigned int>(instances.size());

        // --- Upload instance array to device ---

        CudaBuffer<OptixInstance> instance_buffer;
        instance_buffer.alloc(instance_count);
        CUDA_CHECK(cudaMemcpy(instance_buffer.data(), instances.data(),
                              instance_buffer.size_bytes(), cudaMemcpyHostToDevice));

        // --- Build input ---

        OptixBuildInput build_input{};
        build_input.type = OPTIX_BUILD_INPUT_TYPE_INSTANCES;
        build_input.instanceArray.instances = instance_buffer.device_ptr();
        build_input.instanceArray.numInstances = instance_count;

        // --- Query memory sizes ---

        OptixAccelBufferSizes buffer_sizes{};
        OPTIX_CHECK(optixAccelComputeMemoryUsage(
            context, &kAccelOptions, &build_input, 1, &buffer_sizes));

        // --- Build uncompacted into temp buffer ---

        CudaBuffer<uint8_t> scratch;
        scratch.alloc(buffer_sizes.tempSizeInBytes);

        CudaBuffer<uint8_t> output_buffer;
        output_buffer.alloc(buffer_sizes.outputSizeInBytes);

        CudaBuffer<uint64_t> compacted_size_buffer;
        compacted_size_buffer.alloc(1);

        const OptixAccelEmitDesc emit_desc{
            .result = compacted_size_buffer.device_ptr(),
            .type = OPTIX_PROPERTY_TYPE_COMPACTED_SIZE,
        };

        OptixTraversableHandle uncompacted_handle = 0;
        OPTIX_CHECK(optixAccelBuild(
            context, stream,
            &kAccelOptions,
            &build_input, 1,
            scratch.device_ptr(), scratch.size_bytes(),
            output_buffer.device_ptr(), output_buffer.size_bytes(),
            &uncompacted_handle,
            &emit_desc, 1));

        CUDA_CHECK(cudaStreamSynchronize(stream));
        scratch.free();

        // --- Read compacted size ---

        uint64_t compacted_size = 0;
        CUDA_CHECK(cudaMemcpy(&compacted_size, compacted_size_buffer.data(),
                              sizeof(uint64_t), cudaMemcpyDeviceToHost));
        compacted_size_buffer.free();

        // --- Compact into pool ---

        if (compacted_size < output_buffer.size_bytes()) {
            auto *dest = pool.suballocate(compacted_size, kAsAlignment);
            const auto dest_ptr = reinterpret_cast<CUdeviceptr>(dest);

            OPTIX_CHECK(optixAccelCompact(
                context, stream,
                uncompacted_handle,
                dest_ptr, compacted_size,
                &tlas_handle_));

            CUDA_CHECK(cudaStreamSynchronize(stream));

            spdlog::info("TLAS: {} instances, {:.1f} KB -> {:.1f} KB (compacted {:.0f}%)",
                         instance_count,
                         static_cast<double>(output_buffer.size_bytes()) / 1024.0,
                         static_cast<double>(compacted_size) / 1024.0,
                         (1.0 - static_cast<double>(compacted_size)
                              / static_cast<double>(output_buffer.size_bytes())) * 100.0);
        } else {
            // Compaction did not shrink: copy uncompacted data into pool.
            auto *dest = pool.suballocate(output_buffer.size_bytes(), kAsAlignment);
            CUDA_CHECK(cudaMemcpy(dest, output_buffer.data(),
                                  output_buffer.size_bytes(), cudaMemcpyDeviceToDevice));

            tlas_handle_ = uncompacted_handle;

            spdlog::info("TLAS: {} instances, {:.1f} KB (compaction skipped, copied into pool)",
                         instance_count,
                         static_cast<double>(output_buffer.size_bytes()) / 1024.0);
        }
    }

    void AccelStructure::destroy() {
        for (auto &blas : blas_handles_) {
            blas.handle = 0;
        }
        blas_handles_.clear();
        tlas_handle_ = 0;

        // Free any leftover uncompacted buffers (e.g. if compact was never called).
        blas_uncompacted_.clear();
    }

    OptixTraversableHandle AccelStructure::tlas_handle() const {
        return tlas_handle_;
    }

    const std::vector<BLASHandle> &AccelStructure::blas_handles() const {
        return blas_handles_;
    }
} // namespace qualquer::optix
