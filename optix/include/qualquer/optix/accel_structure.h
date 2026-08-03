#pragma once

/**
 * @file accel_structure.h
 * @brief OptiX layer: acceleration structure builder and owner.
 */

#include <cstdint>
#include <span>
#include <vector>

#include <optix.h>

#include <qualquer/optix/cuda_buffer.h>
#include <qualquer/optix/cuda_device_pool.h>

namespace qualquer::optix {
    /**
     * @brief Build input for a single geometry within a BLAS.
     *
     * Each BLASGeometry represents one triangle set (one glTF primitive).
     * Vertex format is FLOAT3 at offset 0; index format is UINT32.
     */
    struct BLASGeometry {
        /** @brief Device pointer to the vertex buffer. */
        CUdeviceptr vertex_buffer;

        /** @brief Device pointer to the index buffer. */
        CUdeviceptr index_buffer;

        /** @brief Number of vertices. */
        uint32_t vertex_count;

        /** @brief Number of indices (must be a multiple of 3). */
        uint32_t index_count;

        /** @brief Byte stride between consecutive vertices (sizeof(Vertex)). */
        uint32_t vertex_stride;

        /**
         * @brief Whether this geometry is fully opaque (no alpha testing).
         *
         * true  → OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT (hardware skips anyhit).
         * false → OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL (anyhit
         *         invoked at most once per primitive for alpha test).
         */
        bool opaque;
    };

    /**
     * @brief Owning handle for one bottom-level acceleration structure.
     *
     * The compacted AS data resides in a CudaDevicePool managed by the caller;
     * this struct only stores the traversable handle. The handle becomes invalid
     * when the pool is freed.
     */
    struct BLASHandle {
        /** @brief OptiX traversable handle for this BLAS; 0 when empty. */
        OptixTraversableHandle handle = 0;
    };

    /**
     * @brief Compacted-size result for one BLAS group, returned by
     *        query_blas_compacted_sizes so the caller can size the pool.
     */
    struct BLASBuildInfo {
        /** @brief Compacted size in bytes for this BLAS. */
        std::size_t compacted_size = 0;

        /** @brief Uncompacted traversable handle (valid until the temp buffer is freed). */
        OptixTraversableHandle uncompacted_handle = 0;
    };

    /**
     * @brief Builds and owns OptiX acceleration structures (BLAS + TLAS).
     *
     * The build pipeline is split into size-query and compaction phases so
     * that the caller can allocate a CudaDevicePool covering all AS data
     * (enabling a single cudaAccessPolicyWindow for L2 streaming policy).
     *
     * Typical call sequence:
     *   1. build_blas_uncompacted  → returns BLASBuildInfo with compacted sizes
     *   2. caller allocates pool   → using sizes from step 1 + TLAS estimate
     *   3. compact_blas_into_pool  → compacts into pool suballocations
     *   4. build_tlas_uncompacted  → returns uncompacted TLAS handle + size
     *   5. compact_tlas_into_pool  → compacts into pool suballocation
     */
    class AccelStructure {
    public:
        /**
         * @brief Builds all BLAS groups uncompacted and queries compacted sizes.
         *
         * Submits all builds with a shared scratch buffer, synchronizes once
         * to read compacted sizes. The uncompacted output buffers are stored
         * internally and must remain alive until compact_blas_into_pool is
         * called. Total: 1 host-device sync.
         *
         * @param context OptiX device context.
         * @param stream  CUDA stream for builds.
         * @param groups  Per-group geometry spans (one BLAS per non-empty span).
         * @return Per-BLAS compacted sizes and uncompacted handles.
         */
        std::vector<BLASBuildInfo> build_blas_uncompacted(
            OptixDeviceContext context, CUstream stream,
            const std::vector<std::span<const BLASGeometry>> &groups);

        /**
         * @brief Compacts all BLAS into pool suballocations.
         *
         * Each BLAS is compacted into a region obtained via pool.suballocate()
         * using the sizes returned by build_blas_uncompacted. Synchronizes
         * once, then frees the uncompacted output buffers. Total: 1 sync.
         *
         * @param context OptiX device context.
         * @param stream  CUDA stream for compactions.
         * @param pool    Device pool to suballocate from.
         * @param infos   Build info from build_blas_uncompacted.
         */
        void compact_blas_into_pool(OptixDeviceContext context, CUstream stream,
                                    CudaDevicePool &pool,
                                    const std::vector<BLASBuildInfo> &infos);

        /**
         * @brief Queries the TLAS uncompacted output size without building.
         *
         * Used by the caller to reserve space in the pool before building.
         * Only needs the instance count; the memory calculation is host-side.
         *
         * @param context        OptiX device context.
         * @param instance_count Number of TLAS instances.
         * @return Uncompacted TLAS output size in bytes.
         */
        std::size_t query_tlas_size(OptixDeviceContext context,
                                    unsigned int instance_count);

        /**
         * @brief Builds the TLAS and compacts it into the pool.
         *
         * Builds uncompacted into a temporary buffer, reads compacted size,
         * then compacts into a pool suballocation. The suballocated region
         * may be smaller than what the caller reserved (query_tlas_size
         * returns the uncompacted upper bound). Total: 2 syncs.
         *
         * @param context   OptiX device context.
         * @param stream    CUDA stream for build and compaction.
         * @param pool      Device pool to suballocate from.
         * @param instances OptixInstance array (one per scene instance).
         */
        void build_tlas_into_pool(OptixDeviceContext context, CUstream stream,
                                  CudaDevicePool &pool,
                                  std::span<const OptixInstance> instances);

        /**
         * @brief Resets handles; pool memory is owned externally.
         *
         * Clears traversable handles. Does NOT free pool memory — the caller
         * owns the CudaDevicePool and frees it separately.
         */
        void destroy();

        /** @brief TLAS traversable handle; 0 before build. */
        [[nodiscard]] OptixTraversableHandle tlas_handle() const;

        /** @brief All built BLAS handles, in build order. */
        [[nodiscard]] const std::vector<BLASHandle> &blas_handles() const;

    private:
        /** @brief One BLAS per group_id, appended in build order. */
        std::vector<BLASHandle> blas_handles_;

        /** @brief TLAS traversable handle. */
        OptixTraversableHandle tlas_handle_ = 0;

        /** @brief Uncompacted BLAS output buffers, held between build and compact. */
        std::vector<CudaBuffer<uint8_t>> blas_uncompacted_;
    };
} // namespace qualquer::optix
