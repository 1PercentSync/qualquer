# Phase 1 任务清单

> 目标：Vulkan 基础设施 + ImGui + 黑色背景
> 详细设计见 `docs/current-phase.md`，技术决策见 `docs/technical-decisions.md`。
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
- [ ] 更新 `vulkan/CMakeLists.txt`（PUBLIC 链接 glfw3）
- [ ] `Application::init()` 中添加 GLFW 初始化和窗口创建
- [ ] `Application::run()` 中添加主循环（事件轮询、最小化处理）
- [ ] `Application::destroy()` 中添加窗口和 GLFW 销毁
- [ ] 请求用户在 CLion 中编译验证（出现可关闭的窗口）

## Step 3：Vulkan Instance

- [ ] 更新 `vulkan/CMakeLists.txt`（find_package Vulkan、PUBLIC 链接 Vulkan::Vulkan）
- [ ] 创建 `vulkan/include/qualquer/vulkan/context.h`（Context 类声明）
- [ ] 创建 `vulkan/src/context.cpp`
- [ ] Instance 创建（应用信息、Vulkan 1.4、启用 validation layer）
- [ ] Debug Messenger 回调（validation 错误输出到 spdlog）
- [ ] `destroy()` 方法（按反序销毁）
- [ ] Application 中持有 Context，init/destroy 中调用
- [ ] 请求用户在 CLion 中编译验证（控制台无 validation 报错）

## Step 4：Vulkan Device

- [ ] 更新 `vulkan/CMakeLists.txt`（PUBLIC 链接 VMA）
- [ ] Context 中添加 Surface 创建（通过 GLFW）
- [ ] 物理设备枚举与选择（优先 discrete GPU、检查 queue family 支持）
- [ ] Queue family 查询（graphics + present）
- [ ] 逻辑设备创建（启用 VK_KHR_swapchain、Vulkan 1.4 核心特性）
- [ ] Queue 获取
- [ ] VMA Allocator 初始化
- [ ] 更新 `destroy()` 方法
- [ ] 请求用户在 CLion 中编译验证（控制台输出 GPU 名称）

## Step 5：Swapchain

- [ ] 创建 `vulkan/include/qualquer/vulkan/swapchain.h`（Swapchain 类声明）
- [ ] 创建 `vulkan/src/swapchain.cpp`
- [ ] Swapchain 创建（format B8G8R8A8_SRGB、present mode 选择、extent）
- [ ] Image View 获取
- [ ] `destroy()` 方法
- [ ] 请求用户在 CLion 中编译验证

## Step 6：帧同步与命令录制

- [ ] Context 中添加 FrameData 结构（command pool、command buffer、fence、semaphore × frames in flight）
- [ ] Command pool / buffer 创建（per-frame）
- [ ] Fence / Semaphore 创建（per-frame）
- [ ] 帧索引轮换逻辑
- [ ] 创建 `vulkan/include/qualquer/vulkan/commands.h`（CommandBuffer wrapper）
- [ ] 创建 `vulkan/src/commands.cpp`（begin/end、pipeline_barrier）
- [ ] 请求用户在 CLion 中编译验证

## Step 7：帧循环与呈现

- [ ] 主循环框架（wait fence → acquire image → begin cmd → end cmd → submit → present）
- [ ] Swapchain image layout transition（undefined → color attachment → present）
- [ ] Clear 命令录制（begin_rendering + clear color + end_rendering）
- [ ] Swapchain 重建（窗口 resize 处理）
- [ ] 窗口最小化处理（extent 为 0 时暂停渲染）
- [ ] 请求用户在 CLion 中编译验证（窗口显示黑色背景，resize 不崩溃）

## Step 8：ImGui 集成

- [ ] 更新 `app/CMakeLists.txt`（链接 imgui）
- [ ] 创建 ImGui 专用 Descriptor Pool
- [ ] ImGui Vulkan backend 初始化（适配 Dynamic Rendering）
- [ ] ImGui GLFW backend 初始化
- [ ] 每帧 ImGui 渲染集成（NewFrame → demo window → Render → 录制到 command buffer）
- [ ] 请求用户在 CLion 中编译验证（ImGui demo window 正常显示）

## Step 9：调试面板

- [ ] 替换 demo window 为自定义面板
- [ ] 面板内容：FPS、GPU 名称、窗口分辨率
- [ ] 请求用户在 CLion 中编译验证
