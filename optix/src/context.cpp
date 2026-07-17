/**
 * @file context.cpp
 * @brief OptiX layer root context implementation.
 */

#include <qualquer/optix/context.h>
#include <qualquer/optix/cuda_check.h>
#include <qualquer/optix/optix_check.h>

#include <algorithm>
#include <array>
#include <cuda_runtime.h>
#include <optix_stubs.h>
#include <spdlog/spdlog.h>

namespace qualquer::optix {
    namespace {
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

        // Routes OptiX internal diagnostics to spdlog by severity level.
        // OptiX levels: 1=fatal, 2=error, 3=warning, 4=print (informational).
        void optix_log_callback(const unsigned int level,
                                [[maybe_unused]] const char *tag,
                                const char *message,
                                [[maybe_unused]] void *cb_data) {
            switch (level) {
                case 1: spdlog::critical("[OptiX] {}", message); break;
                case 2: spdlog::error("[OptiX] {}", message); break;
                case 3: spdlog::warn("[OptiX] {}", message); break;
                default: spdlog::info("[OptiX] {}", message); break;
            }
        }

        /**
         * @brief OptiX validation mode, an overlay on the standard build types.
         *
         * Enabled only by the QUALQUER_OPTIX_VALIDATE CMake option, so it composes
         * with Debug/Release without a separate build type. When on, OptiX checks
         * SBT layout, handles, params, and builtin exceptions through the log
         * callback at a notable runtime cost.
         */
#ifdef QUALQUER_OPTIX_VALIDATE
        constexpr OptixDeviceContextValidationMode kValidationMode = OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_ALL;
#else
        constexpr OptixDeviceContextValidationMode kValidationMode = OPTIX_DEVICE_CONTEXT_VALIDATION_MODE_OFF;
#endif
    } // namespace

    void Context::init(const std::vector<std::array<std::uint8_t, 16> > &presentable_uuids) {
        int device_count = 0;
        CUDA_CHECK(cudaGetDeviceCount(&device_count));
        if (device_count == 0) {
            spdlog::critical("No CUDA-capable devices available");
            std::abort();
        }

        // Restrict to presentable_uuids to exclude compute-only devices (e.g. a
        // TCC GPU) that Vulkan could not match for presentation. Compute
        // capability is the primary key; rate_device breaks ties among equal
        // capability, mirroring the Vulkan layer's heuristic.
        // cudaComputeModeProhibited devices are skipped — matches NVIDIA's
        // simpleVulkan interop example's eligibility filter.
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

        // Compute stays independent for PT/display overlap. Display participates
        // in legacy default-stream ordering because NGX's CUDA path may enqueue
        // internal work there before tonemap and completion events.
        CUDA_CHECK(cudaStreamCreateWithFlags(&compute_stream, cudaStreamNonBlocking));
        CUDA_CHECK(cudaStreamCreate(&display_stream));

        spdlog::info("CUDA device {}: \"{}\" with compute capability {}.{}",
                     best_device, prop.name, best_major, best_minor);

        // OptiX init loads the library from the driver and populates the function
        // table (optix_stubs.h). Must precede any other optix* call.
        OPTIX_CHECK(optixInit());

        // CUcontext 0 = use the current CUDA context (set by cudaSetDevice above).
        // Log level 4 includes all messages (fatal through informational).
        constexpr OptixDeviceContextOptions ctx_options{
            .logCallbackFunction = optix_log_callback,
            .logCallbackData = nullptr,
            .logCallbackLevel = 4,
            .validationMode = kValidationMode,
        };
        OPTIX_CHECK(optixDeviceContextCreate(nullptr, &ctx_options, &device_context));

        spdlog::info("OptiX device context created");
    }

    void Context::import_display_buffer(void *win32_handle, const uint32_t width, const uint32_t height,
                                        const uint64_t size) {
        // Dedicated flag is required because the Vulkan side used a dedicated
        // allocation (VkMemoryDedicatedAllocateInfo). size must match vkAllocateMemory's size.
        cudaExternalMemoryHandleDesc mem_handle_desc{};
        mem_handle_desc.type = cudaExternalMemoryHandleTypeOpaqueWin32;
        mem_handle_desc.handle.win32.handle = win32_handle;
        mem_handle_desc.size = size;
        mem_handle_desc.flags = cudaExternalMemoryDedicated;
        CUDA_CHECK(cudaImportExternalMemory(&external_memory_, &mem_handle_desc));

        // CUDA's Vulkan interop exposes only the mipmapped-array mapping path
        // (GetMappedBuffer is for 1D). For a single-mip image this is numLevels=1,
        // then level 0 is extracted as a plain cudaArray to back the surface object.
        // flags include cudaArrayColorAttachment because the Vulkan image is a color
        // target (interop requires the CUDA array flags to match the image usage).
        // Channel layout matches VK_FORMAT_R16G16B16A16_SFLOAT (linear LDR intermediate).
        // cudaCreateChannelDescHalf4 is the documented descriptor for half4 arrays.
        const cudaChannelFormatDesc fmt_desc = cudaCreateChannelDescHalf4();

        cudaExtent cuda_ext{};
        cuda_ext.width = width;
        cuda_ext.height = height;
        cuda_ext.depth = 0;

        cudaExternalMemoryMipmappedArrayDesc mip_desc{};
        mip_desc.offset = 0;
        mip_desc.formatDesc = fmt_desc;
        mip_desc.extent = cuda_ext;
        mip_desc.numLevels = 1;
        mip_desc.flags = cudaArrayColorAttachment;
        CUDA_CHECK(cudaExternalMemoryGetMappedMipmappedArray(&mipmap_array_, external_memory_, &mip_desc));

        CUDA_CHECK(cudaGetMipmappedArrayLevel(&array_, mipmap_array_, 0));

        cudaResourceDesc res_desc{};
        res_desc.resType = cudaResourceTypeArray;
        res_desc.res.array.array = array_;
        CUDA_CHECK(cudaCreateSurfaceObject(&display_surface, &res_desc));

        spdlog::info("Display buffer imported ({}x{})", width, height);
    }

    void Context::import_semaphores(const std::array<void *, kMaxFramesInFlight> win32_handles) {
        for (uint32_t i = 0; i < kMaxFramesInFlight; ++i) {
            cudaExternalSemaphoreHandleDesc sem_handle_desc{};
            sem_handle_desc.type = cudaExternalSemaphoreHandleTypeOpaqueWin32;
            sem_handle_desc.handle.win32.handle = win32_handles[i];
            CUDA_CHECK(cudaImportExternalSemaphore(&external_semaphores[i], &sem_handle_desc));
        }
        spdlog::info("Imported {} external semaphores", kMaxFramesInFlight);
    }

    void Context::import_reverse_semaphore(void *win32_handle) {
        cudaExternalSemaphoreHandleDesc sem_handle_desc{};
        sem_handle_desc.type = cudaExternalSemaphoreHandleTypeOpaqueWin32;
        sem_handle_desc.handle.win32.handle = win32_handle;
        CUDA_CHECK(cudaImportExternalSemaphore(&reverse_external_semaphore, &sem_handle_desc));
        spdlog::info("Imported reverse external semaphore");
    }

    void Context::release_display_buffer() {
        // Reverse creation order: the surface wraps array_, so it must go before
        // mipmap_array_, which is mapped onto external_memory_.
        if (display_surface != 0) {
            CUDA_CHECK(cudaDestroySurfaceObject(display_surface));
        }
        // array_ is a level view obtained from cudaGetMipmappedArrayLevel —
        // not allocated by cudaMallocArray, so must not be passed to
        // cudaFreeArray. It is invalidated when mipmap_array_ is freed.
        if (mipmap_array_ != nullptr) {
            CUDA_CHECK(cudaFreeMipmappedArray(mipmap_array_));
        }
        if (external_memory_ != nullptr) {
            CUDA_CHECK(cudaDestroyExternalMemory(external_memory_));
        }
        display_surface = 0;
        array_ = nullptr;
        mipmap_array_ = nullptr;
        external_memory_ = nullptr;
    }

    void Context::destroy() {
        // Display-buffer import first (surface wraps the imported image memory), then
        // the independent semaphores, then OptiX device context, then the streams last
        // — cudaStreamDestroy waits for pending work, so destroying them last drains
        // any in-flight kernel/signal. display_stream before compute_stream mirrors
        // the pipeline dependency (display waits on compute). device_id_ needs no
        // cleanup — the runtime-managed primary context is left intact for other holders.
        release_display_buffer();
        for (auto &sem: external_semaphores) {
            if (sem != nullptr) {
                CUDA_CHECK(cudaDestroyExternalSemaphore(sem));
                sem = nullptr;
            }
        }
        if (reverse_external_semaphore != nullptr) {
            CUDA_CHECK(cudaDestroyExternalSemaphore(reverse_external_semaphore));
            reverse_external_semaphore = nullptr;
        }
        if (device_context != nullptr) {
            OPTIX_CHECK(optixDeviceContextDestroy(device_context));
            device_context = nullptr;
        }
        if (display_stream != nullptr) {
            CUDA_CHECK(cudaStreamDestroy(display_stream));
            display_stream = nullptr;
        }
        if (compute_stream != nullptr) {
            CUDA_CHECK(cudaStreamDestroy(compute_stream));
            compute_stream = nullptr;
        }
    }
} // namespace qualquer::optix
