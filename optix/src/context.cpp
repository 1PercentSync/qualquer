/**
 * @file context.cpp
 * @brief OptiX layer root context implementation.
 */

#include <qualquer/optix/context.h>

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
         * @param device Device index to score.
         * @return Non-negative score; higher is preferred.
         */
        int rate_device(int device) {
            int score = 1;
            if (get_device_attribute(cudaDevAttrIntegrated, device) == 0) {
                score += 1000;
            }
            cudaDeviceProp prop{};
            CUDA_CHECK(cudaGetDeviceProperties(&prop, device));
            constexpr std::size_t kGiga = 1024 * 1024 * 1024;
            score += static_cast<int>(prop.totalGlobalMem / kGiga);
            return score;
        }
    } // namespace

    void Context::init() {
        int device_count = 0;
        CUDA_CHECK(cudaGetDeviceCount(&device_count));
        if (device_count == 0) {
            spdlog::critical("No CUDA-capable devices available");
            std::abort();
        }

        // Select the best device that permits compute. Compute capability is the
        // primary key (major then minor, both strictly greater); among devices
        // sharing the same compute capability, rate_device breaks the tie
        // (discrete + VRAM), mirroring the Vulkan layer's heuristic. Skips
        // cudaComputeModeProhibited (device locked for CUDA by another process
        // or WDDM display mode without compute) — matches NVIDIA's simpleVulkan
        // interop example's eligibility filter. Per-device queries go through
        // cudaDeviceGetAttribute, the API NVIDIA recommends over reading
        // cudaDeviceProp fields (cudaDeviceProp has shed several fields in
        // recent versions, including computeMode).
        int best_device = -1;
        int best_major = 0;
        int best_minor = 0;
        int best_score = 0;
        for (int i = 0; i < device_count; ++i) {
            if (get_device_attribute(cudaDevAttrComputeMode, i) ==
                cudaComputeModeProhibited) {
                continue;
            }
            const int major = get_device_attribute(cudaDevAttrComputeCapabilityMajor, i);
            const int minor = get_device_attribute(cudaDevAttrComputeCapabilityMinor, i);
            const bool higher_cc = major > best_major || (major == best_major && minor > best_minor);
            const bool same_cc = major == best_major && minor == best_minor;
            if (const int score = rate_device(i); higher_cc || (same_cc && score > best_score)) {
                best_device = i;
                best_major = major;
                best_minor = minor;
                best_score = score;
            }
        }
        if (best_device < 0) {
            spdlog::critical("No CUDA device permits compute (all prohibited)");
            std::abort();
        }

        CUDA_CHECK(cudaSetDevice(best_device));

        // UUID has no attribute-API equivalent, so cudaDeviceProp is still
        // fetched once for the selected device to read .uuid and .name.
        cudaDeviceProp prop{};
        CUDA_CHECK(cudaGetDeviceProperties(&prop, best_device));
        device_id_ = best_device;
        std::memcpy(device_uuid.data(), &prop.uuid, kDeviceUuidSize);

        spdlog::info("CUDA device {}: \"{}\" with compute capability {}.{}",
                     best_device, prop.name, best_major, best_minor);
    }

    void Context::destroy() const {
        // The CUDA primary context is managed by the runtime; explicit reset
        // (cudaDeviceReset) would tear down state still referenced by upper
        // layers during their own destruction. Intentionally empty.
    }
} // namespace qualquer::optix
