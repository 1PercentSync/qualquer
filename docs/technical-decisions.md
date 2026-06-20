# 技术选型与决策

> 渲染器当前使用的技术方案、选型理由和关键实现决策。
> 架构层面的设计见 `architecture.md`。

---

## CUDA-Vulkan 互操作

### 内存所有权

**决策**：Vulkan 分配，CUDA 导入。

Vulkan 创建 VkImage（启用 external memory 标志），导出 Win32 HANDLE，CUDA 通过 `cudaImportExternalMemory` 映射后写入。

**理由**：
- 主流方案，NVIDIA 官方示例和文档均采用此方式
- Vulkan 原生拥有 VkImage，布局转换和验证层检查完整
- 格式兼容性由 Vulkan 明确定义

备选方案（CUDA 分配，Vulkan 导入）文档稀缺、验证层对外部导入内存检查有限，排除。

### 中间 Buffer

**决策**：使用独立的渲染 buffer，不直接写入 swapchain image。

渲染流程：OptiX 写入渲染 buffer → Vulkan blit 到 swapchain image。

**理由**：
- Swapchain image 由 WSI 创建，无法添加 external memory 标志，不能导出给 CUDA
- Swapchain 格式通常是 8-bit sRGB，不适合 HDR 累积（需要浮点格式避免量化误差）
- Blit 是后处理的天然边界（tone mapping、gamma 校正）

### Buffer 架构

**决策**：累积 buffer（2 个，ping-pong）+ 显示 buffer（1 个）

| Buffer | 数量 | 格式 | 所有权 | 用途 |
|--------|------|------|--------|------|
| 累积 buffer | 2 | RGBA32F | CUDA 独占 | HDR 累积，ping-pong 交替读写 |
| 显示 buffer | 1 | R8G8B8A8_UNORM | Vulkan 分配，CUDA 导入 | LDR 输出，Vulkan blit 到 swapchain |

**帧流程**：

| 帧 | 累积 | Tone mapping | Vulkan |
|----|------|--------------|--------|
| N | 读 A，写 B | 读 A → 显示 buffer | blit 显示 buffer |
| N+1 | 读 B，写 A | 读 B → 显示 buffer | blit 显示 buffer |

- Tone mapping 读取上一帧的累积结果（显示延迟一帧）
- 累积写入和 tone mapping 读取可并行（操作不同 buffer）
- Tone mapping 在 CUDA 侧完成（HDR→LDR），Vulkan 只做 blit

**帧循环顺序**：

```
1. CUDA：累积 + tone map
2. Vulkan：acquire + blit + ImGui + present
```

先 CUDA 后 acquire，使 acquire 等待时间被累积隐藏。

**性能特征**：

```
T_呈现 = T_acquire + T_blit + T_ImGui + T_present
每帧时间 = max(T_累积, T_tonemap + T_呈现)
```

- 若 T_累积 > T_tonemap + T_呈现：瓶颈在累积，其余全部被隐藏
- 若 T_累积 < T_tonemap + T_呈现：瓶颈在呈现侧，无法完全隐藏

**理由**：
- 累积 buffer 不需要 interop，简化同步
- 显示 buffer 是 LDR（R8G8B8A8），带宽最小
- 单显示 buffer 在 T_累积 > T_呈现 时足够（典型 PT 场景）

### 显示 Buffer 细节

**格式**：`VK_FORMAT_R8G8B8A8_UNORM`

与累积 buffer（RGBA32F）通道顺序一致，CUDA tone mapping kernel 按自然的 RGBA 顺序写入，无需手动 swizzle。blit 到 swapchain（B8G8R8A8_SRGB）时，通道重排 + sRGB 编码由硬件自动完成。

**Tiling**：`VK_IMAGE_TILING_OPTIMAL`

CUDA 侧通过 `cudaExternalMemoryGetMappedMipmappedArray` → `cudaArray_t` → `cudaSurfaceObject_t` 路径访问。LINEAR tiling 格式和尺寸限制多，不适用。

**内存分配**：手动 `vkAllocateMemory`

带 `VkExportMemoryAllocateInfo` 的外部内存不走 VMA 常规路径。使用 dedicated allocation + Win32 HANDLE 导出。

**生命周期**：跟随 swapchain

窗口 resize 时，显示 buffer 的 Vulkan 侧（VkImage + VkDeviceMemory）和 CUDA 侧（external memory + surface object）均需重建。External semaphore 与分辨率无关，不需重建。

### 同步

**决策**：Binary semaphore，per-frame-in-flight 各一个（共 2 个）。

使用 Vulkan external semaphore（`VK_KHR_external_semaphore_win32`）协调 CUDA 和 Vulkan 对显示 buffer 的访问时序。CUDA signal → Vulkan wait，每帧一次。

**理由**：
- 简单的一对一同步，binary 足够
- 帧循环 + fence 保证同一 frame slot 的 signal/wait 严格配对
- Per-frame 分开避免 2 frames in flight 下 binary semaphore 连续 signal
- Timeline 的灵活性用不上

### CUDA 每帧工作

**参数传输**：`cudaMemcpyAsync`

每帧更新的参数（相机矩阵、帧计数等）使用异步传输。同步版本会隐式等待 GPU 空闲，强制串行化。

**Kernel 启动**：每帧 launch

不使用 persistent kernel。launch 开销（~5-10 微秒）相比累积时间（~1ms）可忽略，不值得增加复杂度。

**隐藏条件**：

只要 T_CPU工作 < T_累积（CPU 的 memcpy 排队 + launch + Vulkan 录制在累积时间内完成），CPU 工作被完全隐藏。

### 后续改进方向

1. **单帧多 sample 累积**：每帧累积多个 sample 以提高收敛速度
2. **自适应采样**：根据实际帧率动态调整每帧 sample 数

---

## Vulkan 基础设施

### Swapchain 格式

**决策**：B8G8R8A8_SRGB + VK_COLOR_SPACE_SRGB_NONLINEAR_KHR

**理由**：
- 与 Himalaya 一致
- sRGB 格式硬件自动 gamma 校正
- 显示 buffer（R8G8B8A8_UNORM）blit 时通道重排 + sRGB 编码由硬件自动完成

### Frames in Flight

**决策**：2

**理由**：
- 足够让 CPU 录制和 GPU 执行并行
- Vulkan 侧工作极轻（blit + ImGui），3 帧无额外收益
- 更少的资源占用和延迟

### ImGui 渲染

**决策**：直接渲染到 swapchain image。

ImGui 作为 UI overlay，在 blit 之后绘制。

### 命令录制

**决策**：不使用 CommandBuffer wrapper，直接使用裸 VkCommandBuffer。

Himalaya 有 CommandBuffer wrapper，因为上层（Render Graph、Pass）通过它录制几十种命令，wrapper 是架构边界。

Qualquer 不需要：
- Vulkan 层只做 blit + ImGui，录制命令的就是 vulkan 层自己，无跨层边界
- wrapper 方法（如 `pipeline_barrier`）的参数仍是 Vulkan 类型（`VkDependencyInfo`），没有隐藏细节
- ImGui backend 需要原始 `VkCommandBuffer`，wrapper 的 `handle()` 是多余的间接

### 错误处理

**决策**：VK_CHECK 宏，失败时打印错误并立即 abort。

**理由**：
- Vulkan 错误通常不可恢复
- 简单直接，错误发生时日志已打印
- 与 Himalaya 一致

---

## 初始化

### 初始化顺序

**决策**：CUDA → Vulkan

**理由**：
- CUDA 初始化自然检测 NVIDIA GPU 可用性
- 如果 CUDA 失败，无需继续初始化 Vulkan
- Vulkan 设备选择可匹配 CUDA 已选中的物理设备（通过 UUID）

### 设备检测

**决策**：CUDA 初始化时检测。

**理由**：
- CUDA 初始化自然检测 NVIDIA GPU 可用性和 compute capability
- Vulkan 设备选择通过 UUID 匹配 CUDA 已选中的物理设备

---
