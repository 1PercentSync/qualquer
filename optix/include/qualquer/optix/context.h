#pragma once

/**
 * @file context.h
 * @brief OptiX layer root context: CUDA device selection and UUID query.
 */

#include <array>
#include <cstdint>
#include <vector>

namespace qualquer::optix {
    /**
     * @brief OptiX layer root context owning the selected CUDA device.
     *
     * Selects the best CUDA-capable device during init() from a presentability-
     * constrained candidate list (supplied by the Vulkan layer's pre_init),
     * activates its primary context via cudaSetDevice, and exposes the device
     * UUID for Vulkan physical-device matching. Lifetime is managed explicitly
     * via init() and destroy().
     */
    class Context {
    public:
        /**
         * @brief Selects and activates the best CUDA device among the presentable candidates.
         *
         * The candidate list comes from Vulkan pre_init — devices that cannot
         * present are excluded before CUDA scores them, so the selection never
         * lands on a compute-only device (e.g. a TCC GPU) that Vulkan could not
         * match for presentation.
         * @param presentable_uuids UUIDs of devices the Vulkan layer confirmed presentable.
         */
        void init(const std::vector<std::array<std::uint8_t, 16>> &presentable_uuids);

        /** @brief Releases CUDA resources associated with the selected device. */
        void destroy() const;

        /**
         * @brief Selected device UUID, for Vulkan physical-device matching.
         *
         * Raw std::array<uint8_t,16> (not cudaUUID_t) so the Vulkan layer can
         * compare it without depending on CUDA headers. No alias is defined —
         * the raw type is used directly at every layer.
         */
        std::array<std::uint8_t, 16> device_uuid{};

    private:
        /** @brief Index of the selected CUDA device, for subsequent runtime calls. */
        int device_id_ = -1;
    };
} // namespace qualquer::optix