/**
 * @file accel_structure.cpp
 * @brief OptiX acceleration structure builder implementation.
 */

#include <qualquer/optix/accel_structure.h>

#include <qualquer/optix/cuda_check.h>
#include <qualquer/optix/optix_check.h>

#include <spdlog/spdlog.h>

namespace qualquer::optix {
    OptixTraversableHandle AccelStructure::build_blas(
        // ReSharper disable CppParameterMayBeConst
        OptixDeviceContext context, CUstream stream,
        // ReSharper restore CppParameterMayBeConst
        const std::span<const BLASGeometry> geometries) {
        const auto geom_count = static_cast<unsigned int>(geometries.size());

        // --- Build inputs: one OptixBuildInput per geometry ---

        std::vector<unsigned int> geometry_flags(geom_count);
        std::vector<OptixBuildInput> build_inputs(geom_count);
        for (unsigned int i = 0; i < geom_count; ++i) {
            const auto &geom = geometries[i];

            geometry_flags[i] = geom.opaque
                                    ? OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT
                                    : OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL;

            auto &input = build_inputs[i];
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
            tri.flags = &geometry_flags[i];
            tri.numSbtRecords = 1;
        }

        // --- Build options ---

        constexpr OptixAccelBuildOptions accel_options{
            .buildFlags = OPTIX_BUILD_FLAG_PREFER_FAST_TRACE
                          | OPTIX_BUILD_FLAG_ALLOW_COMPACTION,
            .operation = OPTIX_BUILD_OPERATION_BUILD,
            .motionOptions = {},
        };

        // --- Query memory sizes ---

        OptixAccelBufferSizes buffer_sizes{};
        OPTIX_CHECK(optixAccelComputeMemoryUsage(
            context, &accel_options,
            build_inputs.data(), geom_count,
            &buffer_sizes));

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

        OptixTraversableHandle uncompacted_handle = 0;
        OPTIX_CHECK(optixAccelBuild(
            context, stream,
            &accel_options,
            build_inputs.data(), geom_count,
            temp_buffer.device_ptr(), temp_buffer.size_bytes(),
            output_buffer.device_ptr(), output_buffer.size_bytes(),
            &uncompacted_handle,
            &emit_desc, 1));

        CUDA_CHECK(cudaStreamSynchronize(stream));

        temp_buffer.free();

        // --- Read back compacted size ---

        uint64_t compacted_size = 0;
        CUDA_CHECK(cudaMemcpy(&compacted_size, compacted_size_buffer.data(),
                              sizeof(uint64_t), cudaMemcpyDeviceToHost));
        compacted_size_buffer.free();

        // --- Compact ---

        const auto blas_index = blas_handles_.size();
        BLASHandle result;

        if (compacted_size < output_buffer.size_bytes()) {
            result.buffer.alloc(compacted_size);

            OPTIX_CHECK(optixAccelCompact(
                context, stream,
                uncompacted_handle,
                result.buffer.device_ptr(), result.buffer.size_bytes(),
                &result.handle));

            CUDA_CHECK(cudaStreamSynchronize(stream));

            spdlog::info("BLAS #{}: {:.1f} KB -> {:.1f} KB (compacted {:.0f}%)",
                         blas_index,
                         static_cast<double>(output_buffer.size_bytes()) / 1024.0,
                         static_cast<double>(compacted_size) / 1024.0,
                         (1.0 - static_cast<double>(compacted_size)
                              / static_cast<double>(output_buffer.size_bytes())) * 100.0);
        } else {
            result.buffer = std::move(output_buffer);
            result.handle = uncompacted_handle;

            spdlog::info("BLAS #{}: {:.1f} KB (compaction skipped)",
                         blas_index,
                         static_cast<double>(result.buffer.size_bytes()) / 1024.0);
        }

        const auto handle = result.handle;
        blas_handles_.push_back(std::move(result));
        return handle;
    }

    void AccelStructure::build_tlas(
        // ReSharper disable CppParameterMayBeConst
        OptixDeviceContext context, CUstream stream,
        // ReSharper restore CppParameterMayBeConst
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

        // --- Build options ---

        constexpr OptixAccelBuildOptions accel_options{
            .buildFlags = OPTIX_BUILD_FLAG_PREFER_FAST_TRACE
                          | OPTIX_BUILD_FLAG_ALLOW_COMPACTION,
            .operation = OPTIX_BUILD_OPERATION_BUILD,
            .motionOptions = {},
        };

        // --- Query memory sizes ---

        OptixAccelBufferSizes buffer_sizes{};
        OPTIX_CHECK(optixAccelComputeMemoryUsage(
            context, &accel_options,
            &build_input, 1,
            &buffer_sizes));

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

        OPTIX_CHECK(optixAccelBuild(
            context, stream,
            &accel_options,
            &build_input, 1,
            temp_buffer.device_ptr(), temp_buffer.size_bytes(),
            output_buffer.device_ptr(), output_buffer.size_bytes(),
            &tlas_.handle,
            &emit_desc, 1));

        CUDA_CHECK(cudaStreamSynchronize(stream));

        temp_buffer.free();

        // --- Read back compacted size ---

        uint64_t compacted_size = 0;
        CUDA_CHECK(cudaMemcpy(&compacted_size, compacted_size_buffer.data(),
                              sizeof(uint64_t), cudaMemcpyDeviceToHost));
        compacted_size_buffer.free();

        // --- Compact ---

        if (compacted_size < output_buffer.size_bytes()) {
            tlas_.buffer.alloc(compacted_size);

            const auto uncompacted_handle = tlas_.handle;
            OPTIX_CHECK(optixAccelCompact(
                context, stream,
                uncompacted_handle,
                tlas_.buffer.device_ptr(), tlas_.buffer.size_bytes(),
                &tlas_.handle));

            CUDA_CHECK(cudaStreamSynchronize(stream));

            spdlog::info("TLAS: {} instances, {:.1f} KB -> {:.1f} KB (compacted {:.0f}%)",
                         instance_count,
                         static_cast<double>(output_buffer.size_bytes()) / 1024.0,
                         static_cast<double>(compacted_size) / 1024.0,
                         (1.0 - static_cast<double>(compacted_size)
                              / static_cast<double>(output_buffer.size_bytes())) * 100.0);
        } else {
            tlas_.buffer = std::move(output_buffer);

            spdlog::info("TLAS: {} instances, {:.1f} KB (compaction skipped)",
                         instance_count,
                         static_cast<double>(tlas_.buffer.size_bytes()) / 1024.0);
        }
    }

    void AccelStructure::destroy() {
        for (auto &blas: blas_handles_) {
            blas.buffer.free();
            blas.handle = 0;
        }
        blas_handles_.clear();

        tlas_.buffer.free();
        tlas_.handle = 0;
    }

    OptixTraversableHandle AccelStructure::tlas_handle() const {
        return tlas_.handle;
    }

    const std::vector<BLASHandle> &AccelStructure::blas_handles() const {
        return blas_handles_;
    }
} // namespace qualquer::optix
