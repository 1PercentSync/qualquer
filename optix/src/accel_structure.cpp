/**
 * @file accel_structure.cpp
 * @brief OptiX acceleration structure builder implementation.
 */

#include <qualquer/optix/accel_structure.h>

#include <spdlog/spdlog.h>

namespace qualquer::optix {
    OptixTraversableHandle AccelStructure::build_blas(
        // ReSharper disable CppParameterMayBeConst
        OptixDeviceContext context, CUstream stream,
        // ReSharper restore CppParameterMayBeConst
        const std::span<const BLASGeometry> geometries) {
        // Skeleton — implementation in Step 5 checkbox 2.
        (void)context;
        (void)stream;
        (void)geometries;
        return 0;
    }

    void AccelStructure::build_tlas(
        // ReSharper disable CppParameterMayBeConst
        OptixDeviceContext context, CUstream stream,
        // ReSharper restore CppParameterMayBeConst
        const std::span<const OptixInstance> instances) {
        // Skeleton — implementation in Step 5 checkbox 3.
        (void)context;
        (void)stream;
        (void)instances;
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
