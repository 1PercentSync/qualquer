# Phase 1 任务清单

> 目标：Vulkan 基础设施 + CUDA-Vulkan Interop + ImGui + 测试图案
> 详细设计见 `docs/current-phase.md`，技术决策见 `docs/technical-decisions.md`。
MUSTREAD:4
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

---

## Step 1：项目骨架

- [x] 创建 `vcpkg.json`（glfw3、spdlog、imgui[vulkan-binding, glfw-binding]、vulkan-memory-allocator）
- [x] 更新 `vulkan/CMakeLists.txt`（PUBLIC 链接 spdlog，上层自动继承）
- [x] 更新 `app/src/main.cpp`（spdlog 初始化）
- [x] 请求用户在 CLion 中编译验证

## Step 2：Application 框架 + 窗口

- [x] 创建 `app/include/qualquer/app/application.h`（Application 类声明：init/run/destroy）
- [x] 创建 `app/src/application.cpp`（空的 init/run/destroy 骨架）
- [x] 将 spdlog 初始化从 `main.cpp` 移至 `Application::init()`（修正 Step 1）
- [x] 更新 `main.cpp`（创建 Application → init → run → destroy）
- [x] 更新 `vulkan/CMakeLists.txt`（PUBLIC 链接 glfw3）
- [x] `Application::init()` 中添加 GLFW 初始化和窗口创建
- [x] `Application::run()` 中添加主循环（事件轮询、最小化处理）
- [x] `Application::destroy()` 中添加窗口和 GLFW 销毁
- [x] 请求用户在 CLion 中编译验证（出现可关闭的窗口）

## Step 3：Vulkan Instance

- [x] 更新 `vulkan/CMakeLists.txt`（find_package Vulkan、PUBLIC 链接 Vulkan::Vulkan）
- [x] 创建 `vulkan/include/qualquer/vulkan/context.h`（Context 类声明）
- [x] 创建 `vulkan/src/context.cpp`
- [x] Instance 创建（应用信息、Vulkan 1.4、启用 validation layer）
- [x] Debug Messenger 回调（validation 错误输出到 spdlog）
- [x] `destroy()` 方法（按反序销毁）
- [x] Application 中持有 Context，init/destroy 中调用
- [x] 请求用户在 CLion 中编译验证（控制台无 validation 报错）

## Step 4：Vulkan Device

- [x] 更新 `vulkan/CMakeLists.txt`（PUBLIC 链接 VMA）
- [x] Context 中添加 Surface 创建（通过 GLFW）
- [x] 物理设备枚举与选择（优先 discrete GPU、检查 queue family 支持）
- [x] Queue family 查询（graphics + present）
- [x] 逻辑设备创建（启用 VK_KHR_swapchain、Vulkan 1.4 核心特性）
- [x] Queue 获取
- [x] VMA Allocator 初始化
- [x] 更新 `destroy()` 方法
- [x] 请求用户在 CLion 中编译验证（控制台输出 GPU 名称）

## Step 5：Swapchain

- [x] 创建 `vulkan/include/qualquer/vulkan/swapchain.h`（Swapchain 类声明：init/destroy、PresentMode 枚举、成员字段）
- [x] 创建 `vulkan/src/swapchain.cpp` 并实现创建链路（init、format/present mode/extent 选择、create_resources 建 swapchain 取 images、create_image_views）
- [x] 实现 `destroy(const Context&)`（按反序销毁 image views 与 swapchain）
- [x] Application 持有 Swapchain，init/destroy 中按序调用
- [x] 请求用户在 CLion 中编译验证（MAILBOX / 1920x1080 / 3 images，无 validation 报错）

## Step 6：帧同步与命令录制

- [x] Context 帧资源（FrameData 结构、create_frame_data 创建 pool/buffer/fence/semaphore、destroy 销毁、current_frame/advance_frame 轮换）
- [x] 将 render_finished_semaphore 从 FrameData（per-frame）移至 Swapchain（per-swapchain-image）
- [x] 请求用户在 CLion 中编译验证

## Step 7：帧循环与呈现

- [x] 主循环框架（wait fence → acquire → reset fence → begin cmd → end cmd → submit → present）
- [x] 单帧渲染录制（barrier undefined→color attachment + begin_rendering/clear 黑/end_rendering + barrier color attachment→present）
- [x] Swapchain 重建（acquire/present 返回值 + resize 轮询触发 recreate）
- [x] 窗口最小化处理（extent 为 0 时暂停渲染）
- [x] 请求用户在 CLion 中编译验证（窗口显示黑色背景，resize 不崩溃）

## Step 8：ImGui 集成

- [x] 更新 `renderer/CMakeLists.txt`（find_package imgui，链接 imgui::imgui PRIVATE）
- [x] 创建 `renderer/include/qualquer/renderer/imgui_backend.h`（ImGuiBackend 类：init/destroy/begin_frame/render）
- [x] 创建 `renderer/src/imgui_backend.cpp`（Context + GLFW backend + Vulkan backend 初始化，一并移除 placeholder.cpp）
- [x] 帧循环集成（Application 持有 ImGuiBackend；begin_frame 后 begin_frame；render_frame 中 begin/end rendering 间 render(cmd)；destroy 中销毁）
- [x] 请求用户在 CLion 中编译验证（ImGui 空面板正常显示）

## Step 9：调试面板

- [x] 创建 `renderer/include/qualquer/renderer/debug_ui.h`（DebugUI 类、DebugUIContext、DebugUIActions、FrameStats）
- [x] 创建 `renderer/src/debug_ui.cpp`（FrameStats 计算 + draw 面板绘制；面板内容：FPS + 1% Low、GPU、分辨率、VRAM、Present Mode、Log Level、Error Banner）
- [x] Context 可选启用 `VK_EXT_memory_budget`（检查支持 → `memory_budget_supported` 标志 → `query_vram_usage()`）
- [x] Application 接线 DebugUI：持有 DebugUI + `user_present_mode_`/`error_message_` 成员，构造 DebugUIContext 调 draw，处理 actions（present_mode_changed → recreate + 回写、error_dismissed → 清错误），同步移除 ImGuiBackend::show_panel
- [x] 请求用户在 CLion 中编译验证

## Step 10：optix 层骨架 + CUDA Context

- [x] 创建 `optix/CMakeLists.txt`（启用 CUDA 语言，static library，链接 CUDA::cudart）
- [x] 更新顶层 `CMakeLists.txt`（add_subdirectory(optix)，更新 renderer 依赖链）
- [x] 创建 `optix/include/qualquer/optix/context.h`（Context 类声明：init/destroy，设备 UUID 查询）
- [x] 创建 `optix/src/context.cpp`（设备枚举、compute capability 检查、最佳设备选择、UUID 获取，init 中打印设备名 + compute capability）

### Step 10.5：三段式初始化重构

- [x] `vulkan::Context` 拆 `init(window)` 为 `pre_init(window)` + `init(uuid)`（pre_init：instance+surface+枚举支持 present 的物理设备+查 UUID，输出候选 UUID 列表；init：按传入 UUID 重枚举匹配+完成 device/queue/allocator/frame_data）
- [x] `optix::Context::init` 签名改为接收候选 UUID 列表（`std::vector<std::array<uint8_t,16>>`），设备选择加"UUID ∈ 候选列表"过滤
- [x] UUID 类型统一为裸 `std::array<uint8_t,16>`（删除 `optix::DeviceUuid` 别名与 `kDeviceUuidSize` 常量，各层直接用裸类型）
- [x] `Application::init` 编排三段（pre_init → optix init(候选列表) → vulkan init(选中UUID)）
- [x] 请求用户在 CLion 中编译验证（三段式初始化流程通过，CUDA 与 Vulkan 选同一支持 present 的 GPU）

### Step 10.6：Application 接线与收尾

- [x] Application 接线 optix::Context（持有成员，destroy 反序销毁）
- [x] 请求用户在 CLion 中编译验证（控制台输出 CUDA 设备名称和 compute capability）

## Step 11：External 扩展启用

- [x] `Context::create_device()` 启用 `VK_KHR_external_memory_win32`、`VK_KHR_external_semaphore_win32` 扩展
- [x] 请求用户在 CLion 中编译验证（扩展启用无 validation 报错）

## Step 12：Vulkan Interop 资源

- [x] 创建 `vulkan/include/qualquer/vulkan/interop.h`（InteropImage、InteropSemaphore 类声明）
- [x] 实现 InteropImage（R8G8B8A8_UNORM VkImage + OPTIMAL tiling + 手动 vkAllocateMemory with VkExportMemoryAllocateInfo + Win32 HANDLE 导出 + destroy）
- [x] 实现 InteropSemaphore（VkSemaphore with external + Win32 HANDLE 导出 + destroy）
- [x] Application 中创建 display buffer InteropImage（跟随 swapchain extent）和 2 个 per-frame InteropSemaphore
- [x] 请求用户在 CLion 中编译验证

## Step 13：CUDA 导入 + 测试 Kernel

- [x] 创建 `optix/include/qualquer/optix/cuda_check.h`（可复用 CUDA_CHECK 宏）
- [x] CudaContext 添加 external memory 导入（HANDLE → cudaExternalMemory → cudaMipmappedArray → cudaArray → cudaSurfaceObject）
- [x] CudaContext 添加 external semaphore 导入（HANDLE → cudaExternalSemaphore）
- [ ] renderer 层启用 CUDA，创建 `renderer/src/test_kernel.cu`（UV 渐变 + 帧号驱动动画，surf2Dwrite 写入 R8G8B8A8）
- [ ] Application 初始化中从 Vulkan InteropImage/InteropSemaphore 取 HANDLE → 传给 CudaContext 导入
- [ ] 请求用户在 CLion 中编译验证

## Step 14：帧循环集成（CUDA blit）

- [ ] 帧循环中集成 CUDA test kernel launch + cudaSignalExternalSemaphoresAsync
- [ ] Vulkan submit 中添加 external semaphore wait
- [ ] 命令录制中添加 display buffer → swapchain image 的 vkCmdBlitImage（含 layout transition）
- [ ] Swapchain 重建时同步重建 display buffer（Vulkan InteropImage destroy/recreate + CUDA 侧 reimport）
- [ ] 请求用户在 CLion 中编译验证（窗口显示渐变色 + ImGui 面板，resize 不崩溃）
