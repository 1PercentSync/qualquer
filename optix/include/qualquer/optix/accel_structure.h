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
     * Holds the traversable handle and the device buffer backing the AS data.
     * Move-only because CudaBuffer is move-only.
     */
    struct BLASHandle {
        /** @brief OptiX traversable handle for this BLAS; 0 when empty. */
        OptixTraversableHandle handle = 0;

        /** @brief Device buffer backing the acceleration structure data. */
        CudaBuffer<uint8_t> buffer;
    };

    /**
     * @brief Owning handle for the top-level acceleration structure.
     *
     * Holds the traversable handle and the device buffer backing the AS data.
     * Move-only because CudaBuffer is move-only.
     */
    struct TLASHandle {
        /** @brief OptiX traversable handle for the TLAS; 0 when empty. */
        OptixTraversableHandle handle = 0;

        /** @brief Device buffer backing the acceleration structure data. */
        CudaBuffer<uint8_t> buffer;
    };

    /**
     * @brief Builds and owns OptiX acceleration structures (BLAS + TLAS).
     *
     * Provides methods to build individual BLAS (with compaction) and a TLAS.
     * The caller handles scene-level grouping (by group_id) and instance
     * deduplication; this class handles the OptiX API calls and owns the
     * resulting device resources. destroy() releases all of them.
     */
    class AccelStructure {
    public:
        /**
         * @brief Builds one multi-geometry BLAS with compaction.
         *
         * Constructs a GAS from the supplied geometries using
         * PREFER_FAST_TRACE | ALLOW_COMPACTION, queries the compacted size,
         * and compacts into a tighter buffer. Synchronizes on the stream
         * (init-time operation). The caller invokes this once per group_id;
         * each call appends to the internal BLAS list.
         *
         * @param context    OptiX device context.
         * @param stream     CUDA stream for build and compaction.
         * @param geometries Geometries to include (one per primitive in the group).
         * @return Traversable handle of the built (and compacted) BLAS.
         */
        OptixTraversableHandle build_blas(OptixDeviceContext context, CUstream stream,
                                          std::span<const BLASGeometry> geometries);

        /**
         * @brief Builds the TLAS from pre-assembled instance descriptions.
         *
         * Uploads the instance array to the device and builds an IAS with
         * PREFER_FAST_TRACE. Synchronizes on the stream (init-time operation).
         *
         * @param context   OptiX device context.
         * @param stream    CUDA stream for the build.
         * @param instances OptixInstance array (one per deduplicated scene instance).
         */
        void build_tlas(OptixDeviceContext context, CUstream stream,
                        std::span<const OptixInstance> instances);

        /**
         * @brief Destroys all owned BLAS and TLAS handles, releasing device memory.
         *
         * Idempotent: members are reset so a repeat call is a no-op.
         */
        void destroy();

        /** @brief TLAS traversable handle; 0 before build_tlas. */
        [[nodiscard]] OptixTraversableHandle tlas_handle() const;

        /** @brief All built BLAS handles, in build order. */
        [[nodiscard]] const std::vector<BLASHandle> &blas_handles() const;

    private:
        /** @brief One BLAS per group_id, appended in build order. */
        std::vector<BLASHandle> blas_handles_;

        /** @brief Single TLAS for the scene. */
        TLASHandle tlas_;
    };
} // namespace qualquer::optix
