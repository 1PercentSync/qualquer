#pragma once

/**
 * @file context.h
 * @brief OptiX layer root context: CUDA device selection and UUID query.
 */

#include <array>
#include <cstdint>

namespace qualquer::optix {
    /**
     * @brief Size of a CUDA device UUID in bytes.
     *
     * Matches cudaUUID_t (16 bytes) and VK_UUID_SIZE. Kept as a raw byte
     * array rather than cudaUUID_t so upper layers can compare against the
     * Vulkan device UUID without depending on the CUDA runtime headers.
     */
    constexpr std::size_t kDeviceUuidSize = 16;

    /** @brief Raw CUDA device UUID, comparable byte-for-byte with a Vulkan device UUID. */
    using DeviceUuid = std::array<std::uint8_t, kDeviceUuidSize>;

    /**
     * @brief OptiX layer root context owning the selected CUDA device.
     *
     * Selects the best CUDA-capable device during init(), activates its
     * primary context via cudaSetDevice, and exposes the device UUID for
     * Vulkan physical-device matching (CUDA is initialized before Vulkan,
     * which then matches by UUID). Lifetime is managed explicitly via
     * init() and destroy().
     */
    class Context {
    public:
        /** @brief Selects and activates the best CUDA device. */
        void init();

        /** @brief Releases CUDA resources associated with the selected device. */
        void destroy() const;

        /** @brief Selected device UUID, for Vulkan physical-device matching. */
        DeviceUuid device_uuid{};

    private:
        /** @brief Index of the selected CUDA device, for subsequent runtime calls. */
        int device_id_ = -1;
    };
} // namespace qualquer::optix