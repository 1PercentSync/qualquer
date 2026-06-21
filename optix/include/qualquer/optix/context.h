#pragma once

/**
 * @file context.h
 * @brief OptiX layer root context: CUDA device selection and UUID query.
 */

#include <array>
#include <cstdint>

namespace qualquer::optix {
    /**
     * @brief OptiX layer root context owning the selected CUDA device.
     *
     * Selects the best CUDA-capable device during init(), activates its primary
     * context via cudaSetDevice, and exposes the device UUID for Vulkan
     * physical-device matching. Lifetime is managed explicitly via init() and
     * destroy().
     */
    class Context {
    public:
        /** @brief Selects and activates the best CUDA device. */
        void init();

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