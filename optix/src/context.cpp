/**
 * @file context.cpp
 * @brief OptiX layer root context implementation.
 */

#include <qualquer/optix/context.h>

#include <algorithm>
#include <array>
#include <cuda_runtime.h>
#include <spdlog/spdlog.h>

namespace qualquer::optix {
    namespace {
        /**
         * @brief Checks a cudaError_t and aborts on failure with diagnostic output.
         *
         * CUDA runtime errors during device selection are unrecoverable
         * configuration failures; mirroring VK_CHECK's fail-fast policy.
         */
#define CUDA_CHECK(x)                                                             \
            do {                                                                  \
                cudaError_t cuda_check_result_ = (x);                             \
                if (cuda_check_result_ != cudaSuccess) {                          \
                    spdlog::critical("CUDA_CHECK failed: {} returned {} at {}:{}", \
                                     #x,                                          \
                                     cudaGetErrorString(cuda_check_result_),      \
                                     __FILE__,                                    \
                                     __LINE__);                                   \
                    std::abort();                                                 \
                }                                                                 \
            } while (0)

        /**
         * @brief Queries an integer device attribute via the runtime attribute API.
         *
         * Wrapper over cudaDeviceGetAttribute so callers avoid repeating the
         * output-pointer and CUDA_CHECK boilerplate for each attribute query.
         * @param attr Attribute to query (e.g. cudaDevAttrComputeMode).
         * @param device Device index to query.
         * @return The attribute value.
         */
        int get_device_attribute(const cudaDeviceAttr attr, const int device) {
            int value = 0;
            CUDA_CHECK(cudaDeviceGetAttribute(&value, attr, device));
            return value;
        }

        /**
         * @brief Scores a CUDA device's suitability as a render GPU.
         *
         * Used only as a tiebreaker among devices sharing the same compute
         * capability, which is compared separately as the primary key. Mirrors
         * the Vulkan layer's device scoring so both layers prefer the same GPU
         * by the same heuristic. Scoring: discrete GPU +1000, +1 per GB of VRAM.
         *
         * totalGlobalMem is used as the VRAM figure. On Windows WDDM it includes
         * the shared system memory fallback pool, so its absolute value over-
         * reports a discrete GPU's dedicated VRAM. For device selection this is
         * harmless: VidMm computes shared memory as System RAM / 2 per WDDM
         * adapter, so the shared portion is an identical constant across all
         * candidate adapters and cancels out in the relative comparison, leaving
         * dedicated VRAM as the deciding factor.
         * @param prop Device properties of the candidate (integrated flag, VRAM).
         * @return Non-negative score; higher is preferred.
         */
        int rate_device(const cudaDeviceProp &prop) {
            int score = 1;
            if (prop.integrated == 0) {
                score += 1000;
            }
            constexpr std::size_t kGiga = 1024 * 1024 * 1024;
            score += static_cast<int>(prop.totalGlobalMem / kGiga);
            return score;
        }

        /**
         * @brief Reads a CUDA device's 16-byte UUID into a raw std::array.
         *
         * cudaDeviceProp.uuid (cudaUUID_t, char[16]) is copied into a uint8_t
         * array so it compares cleanly with the Vulkan device UUID (also
         * uint8_t[16]) without sign/alias concerns.
         * @param prop Device properties whose .uuid is read.
         * @return The device UUID as a raw byte array.
         */
        std::array<std::uint8_t, 16> get_device_uuid(const cudaDeviceProp &prop) {
            std::array<std::uint8_t, 16> uuid{};
            std::memcpy(uuid.data(), &prop.uuid, uuid.size());
            return uuid;
        }
    } // namespace

    void Context::init(const std::vector<std::array<std::uint8_t, 16> > &presentable_uuids) {
        int device_count = 0;
        CUDA_CHECK(cudaGetDeviceCount(&device_count));
        if (device_count == 0) {
            spdlog::critical("No CUDA-capable devices available");
            std::abort();
        }

        // Select the best device among those the Vulkan layer confirmed
        // presentable. Restricting to presentable_uuids excludes compute-only
        // devices (e.g. a TCC GPU) that Vulkan could not match for presentation.
        // Within that set, compute capability is the primary key (major then
        // minor, both strictly greater); among devices sharing a compute
        // capability, rate_device breaks the tie (discrete + VRAM), mirroring
        // the Vulkan layer's heuristic. Skips cudaComputeModeProhibited (device
        // locked for CUDA by another process or WDDM display mode without
        // compute) — matches NVIDIA's simpleVulkan interop example's eligibility
        // filter. cudaDeviceProp is fetched once per device to serve UUID
        // filtering and VRAM scoring from a single query.
        int best_device = -1;
        int best_major = 0;
        int best_minor = 0;
        int best_score = 0;
        for (int i = 0; i < device_count; ++i) {
            if (get_device_attribute(cudaDevAttrComputeMode, i) ==
                cudaComputeModeProhibited) {
                continue;
            }

            cudaDeviceProp prop{};
            CUDA_CHECK(cudaGetDeviceProperties(&prop, i));
            if (const std::array<std::uint8_t, 16> uuid = get_device_uuid(prop);
                std::ranges::find(presentable_uuids, uuid) == presentable_uuids.end()) {
                continue;
            }

            const int major = get_device_attribute(cudaDevAttrComputeCapabilityMajor, i);
            const int minor = get_device_attribute(cudaDevAttrComputeCapabilityMinor, i);
            const bool higher_cc = major > best_major || (major == best_major && minor > best_minor);
            const bool same_cc = major == best_major && minor == best_minor;
            if (const int score = rate_device(prop); higher_cc || (same_cc && score > best_score)) {
                best_device = i;
                best_major = major;
                best_minor = minor;
                best_score = score;
            }
        }
        if (best_device < 0) {
            spdlog::critical("No presentable CUDA device found in the candidate list");
            std::abort();
        }

        CUDA_CHECK(cudaSetDevice(best_device));

        cudaDeviceProp prop{};
        CUDA_CHECK(cudaGetDeviceProperties(&prop, best_device));
        device_id_ = best_device;
        device_uuid = get_device_uuid(prop);

        spdlog::info("CUDA device {}: \"{}\" with compute capability {}.{}",
                     best_device, prop.name, best_major, best_minor);
    }

    void Context::destroy() const {
        // The CUDA primary context is managed by the runtime; explicit reset
        // (cudaDeviceReset) would tear down state still referenced by upper
        // layers during their own destruction. Intentionally empty.
    }
} // namespace qualquer::optix
