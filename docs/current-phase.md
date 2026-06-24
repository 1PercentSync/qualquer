# 当前阶段

> 目标：Vulkan 基础设施 + CUDA-Vulkan Interop + ImGui + 测试图案
> 任务清单见 `tasks/phase1.md`。

---

## 实现步骤

### 依赖关系

```
Step 1 → Step 2 → Step 3 → Step 4 → Step 5 → Step 6 → Step 7 → Step 8 → Step 9 → Step 10 → Step 10.5 → Step 10.6 → Step 11 → Step 12 → Step 13 → Step 14
```

### 总览

| Step | 主题 | 验证标准 |
|------|------|----------|
| 1 | 项目骨架 | CMake 构建通过 |
| 2 | Application 框架 + 窗口 | GLFW 窗口显示，可关闭 |
| 3 | Vulkan Instance | validation layer 正常输出 |
| 4 | Vulkan Device | 物理设备信息打印 |
| 5 | Swapchain | 无报错创建 |
| 6 | 帧同步与命令录制 | 编译通过 |
| 7 | 帧循环与呈现 | 黑色背景显示，resize 不崩溃 |
| 8 | ImGui 集成 | 空面板显示 |
| 9 | 调试面板 | 自定义面板显示 FPS、GPU、分辨率、VRAM、Present Mode / Log Level 下拉框 |
| 10 | optix 层 + CUDA Context | 编译通过，CUDA 设备信息打印 |
| 10.5 | 三段式初始化重构 | CUDA 与 Vulkan 选同一支持 present 的 GPU |
| 10.6 | Application 接线与收尾 | 控制台输出 CUDA 设备名称和 compute capability |
| 11 | External 扩展启用 | 扩展启用无 validation 报错 |
| 12 | Vulkan Interop 资源 | 编译通过 |
| 13 | CUDA 导入 + 测试 Kernel | 编译通过 |
| 14 | 帧循环集成（CUDA blit） | 窗口显示渐变色 + ImGui，resize 不崩溃 |

---

## 技术要点

### Vulkan 版本与扩展

- Vulkan 1.4（synchronization2、dynamic_rendering 为核心特性）
- VK_KHR_swapchain

### Swapchain

- Format：B8G8R8A8_SRGB + VK_COLOR_SPACE_SRGB_NONLINEAR_KHR（硬要求，不支持则报错退出，无回退）
- Present mode：单一回退 FIFO（请求的 mode 不支持则退 FIFO，不级联到 Immediate）

### 帧同步

- Frames in flight：2
- Per-frame 资源（FrameData）：command pool、command buffer、render fence、image_available semaphore
MUSTREAD:8
- Per-swapchain-image 资源（Swapchain）：render_finished semaphore

### 帧循环（编排与同步归 app 层）

- 流程：`wait fence → acquire → reset fence → [CUDA 提交 + Vulkan 录制 by renderer] → submit2 → present`
- 录制与 CUDA 提交归 renderer（`Renderer::render_frame`，内部拆 submit_cuda / record_vulkan）；begin_frame（含 ImGui begin_frame，需 GLFW/DeltaTime）与 submit/present 的时序骨架留 app
- Clear 用 dynamic rendering：`vkCmdBeginRendering` + attachment `loadOp` → `vkCmdEndRendering`。blit 介入后 `loadOp=LOAD` 保留 blit 结果；此前为 `CLEAR`（oldLayout=UNDEFINED 无内容可 LOAD）
- swapchain image 的 layout 流转（blit 介入后）：`UNDEFINED → TRANSFER_DST_OPTIMAL →（blit）→ COLOR_ATTACHMENT_OPTIMAL → PRESENT_SRC_KHR`，手写 `vkCmdPipelineBarrier2`（dynamic rendering 不自动管 layout）

### ImGui

- 使用 Dynamic Rendering（无 render pass）
- 直接渲染到 swapchain image
- 封装类（`ImGuiBackend`）所有权归 Application（绑定窗口/输入生命周期），Renderer 经非拥有指针引用（与 Himalaya 同构）；begin_frame 留 Application（需 GLFW/DeltaTime）
- Descriptor Pool 由 ImGui backend 自建自销（`DescriptorPoolSize > 0`），不手动管理
- 版本 1.92.6 的 Vulkan backend API 与 Himalaya 参考的 1.91.9 不同：渲染管线信息在 `PipelineInfoMain.PipelineRenderingCreateInfo` 下、`Init` 返回 bool 需检查、需设 `ApiVersion`

### CUDA-Vulkan Interop

- 初始化顺序：三段式（Vulkan pre-init → CUDA → Vulkan 完成）。CUDA→Vulkan 单向初始化下，CUDA 打分可能选中无法 present 的设备（TCC 计算卡等），导致 Vulkan 按 UUID 匹配后无法 present。改为三段式：Vulkan 先查支持 present 的物理设备列表（pre-init），CUDA 在该列表约束内打分选中，再回传 Vulkan 完成初始化。present 约束前置，从根上排除不可呈现设备。CUDA→Vulkan 属软理由（省 CUDA 失败时的 Vulkan 初始化），被此场景推翻即更新——依据"决策可更新性"原则（`docs/architecture.md`）
- 设备匹配：CUDA 与 Vulkan 通过设备 UUID 对齐到同一物理 GPU。CUDA `cudaDeviceProp.uuid` 与 Vulkan `VkPhysicalDeviceIDProperties.deviceUUID` 字节一致，逐字节比较
- UUID 类型：跨层用裸 `std::array<uint8_t,16>`（不用 `cudaUUID_t`，使 vulkan 层比对 UUID 时不依赖 CUDA 头；不在任一层定义别名，各层直接用裸类型）
- 设备扩展：`VK_KHR_external_memory_win32`、`VK_KHR_external_semaphore_win32`
- 显示 buffer：`R8G8B8A8_UNORM`、OPTIMAL tiling、手动 `vkAllocateMemory`（不走 VMA）
- CUDA 侧通过 `cudaSurfaceObject_t` 写入显示 buffer（路径：`cudaImportExternalMemory` → `cudaExternalMemoryGetMappedMipmappedArray`（single-mip，numLevels=1）→ `cudaGetMipmappedArrayLevel` 取 level 0 `cudaArray_t` → `cudaCreateSurfaceObject`；dedicated allocation 需设 `cudaExternalMemoryDedicated` 标志）
- 同步：per-frame binary external semaphore × 2，CUDA signal → Vulkan wait（路径：`cudaImportExternalSemaphore`，OPAQUE_WIN32 binary 语义）
- Blit：`vkCmdBlitImage`（显示 buffer → swapchain image）。src `R8G8B8A8_UNORM` → dst `B8G8R8A8_SRGB`，blit 硬件自动完成：UNORM→float→通道重排（RGBA→BGRA）→sRGB 编码（spec 明确支持 unorm/snorm 互转 + sRGB 目标编码）
- 测试 kernel：UV 渐变 + 帧号驱动动画
- 测试 kernel 归 renderer 层（渲染内容属 renderer，optix 层只提供封装能力）；帧号计数器由 renderer 层持有（与 Himalaya `Renderer::frame_counter_` 一致）
- 显式 CUDA stream：`optix::Context` 持有 `cudaStream_t`，kernel launch 与 external semaphore signal 提交其上。本步验证 stream 机制，为未来重叠计算（累积/tone map 并行）铺路
- `CUDA_CHECK` 宏定义于 `optix/include/qualquer/optix/cuda_check.h`，optix/renderer 层复用；.cu kernel 文件用内联 `CUDA_CHECK_KERNEL`（fprintf+abort，不拉 spdlog/fmt）

### CUDA 设备打分

- 选择主键：compute capability（major 优先，minor 次之，严格 `>`）
- 同 compute capability 的 tiebreaker：独显（`integrated == 0`）`+1000`，每 GB 显存 `+1`——与 vulkan 层 `rate_device` 同款启发式，保证多层启发式一致
- 显存查询用 `cudaDeviceProp.totalGlobalMem`。Windows WDDM 下它含 shared system memory（绝对值虚高），但 VidMm 对每个 WDDM adapter 计算 shared memory = System RAM / 2，该部分对所有候选卡是相同常数，相对比较中抵消，剩余决定因素仍是专用 VRAM
- 资格过滤：跳过 `cudaComputeModeProhibited`（设备被其他进程锁死 / WDDM 无 compute），与 NVIDIA `simpleVulkan` interop 示例一致
- compute capability 与 compute mode 经 `cudaDeviceGetAttribute`（`cudaDevAttrComputeCapabilityMajor/Minor`、`cudaDevAttrComputeMode`）查询，UUID 与设备名经 `cudaDeviceProp`（无 attribute API 等价物）

---

## 完成标准

- 窗口显示 CUDA 渲染的渐变测试图案
- ImGui 面板可交互
- 窗口可调整大小（swapchain + 显示 buffer 重建）
- 关闭窗口正常退出
- CUDA-Vulkan 同步正确（无 validation 报错）
