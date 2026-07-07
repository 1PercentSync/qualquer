# 技术选型与决策

> 渲染器当前使用的技术方案、选型理由和关键实现决策。
> 架构层面的设计见 `architecture.md`。

---

## OptiX

### IR 加载方式

**决策**：构建期 nvcc `--optix-ir` 编译为 .optixir 文件，运行时从文件加载，不嵌入可执行文件。

**理由**：
- 嵌入（bin2c 转 C 数组）引入 bin2c 的 size 符号命名、stdint 依赖、padding 等复杂度；对单一 module 的项目，其收益（免去运行时文件路径管理）不足以抵消——违反 architecture.md「复杂度与收益成正比」
- 不嵌入下，改 .cu 只需 CMake 重编 .optixir（incremental），不重新链接整个 exe，开发迭代快
- 本地学习项目不分发，.optixir 部署的路径管理（POST_BUILD 复制到运行目录）开销可控

**备选（已排除）**：
- **嵌入（bin2c）**：bin2c `--length` 生成的符号（`uint32_t <name>Length`）与设计约定的 `programs_optixir_size` 不符，需额外 wrapper 拼接，得不偿失
- **运行时 NVRTC 编译**：OptiX `.cu` 需运行时访问 OptiX headers 路径，且单 module 用不上磁盘缓存收益，复杂度高于构建期 nvcc

**部署参考**：himalaya 的 POST_BUILD copy 模式——himalaya 复制 shader 源文件、运行时 shaderc 编译；Qualquer 借其「部署 + 运行时加载」形态，编译留在构建期（nvcc），运行时只读已编译的 .optixir。

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
MUSTREAD:8

**帧流程**：

| 帧 | 累积（compute_stream） | Tone mapping（display_stream） | Vulkan |
|----|------|--------------|--------|
| N | 读 A，写 B | 读 A → 显示 buffer | blit 显示 buffer |
| N+1 | 读 B，写 A | 读 B → 显示 buffer | blit 显示 buffer |

- Tone mapping 读取上一帧的累积结果（显示延迟一帧）
- 累积写入和 tone mapping 读取在不同 stream 上并行（操作不同 buffer，CUDA event 同步）
- Tone mapping 在 CUDA 侧完成（HDR→LDR），Vulkan 只做 blit

**帧循环顺序**：

```
1. CUDA：submit_cuda 向两条 stream 提交工作（compute_stream: raygen, display_stream: tonemap + signal）
2. Vulkan：acquire + blit + ImGui + present
```

先 CUDA 后 acquire，使 acquire 等待时间被累积隐藏。

**为什么提交顺序重要（异步 ≠ 重叠）**：异步提交只保证 CPU 不阻塞，不保证 GPU 引擎何时启动。submit_CUDA 一调用，GPU 的 CUDA 引擎立刻开始算；若 acquire 在前，CUDA 引擎在 CPU 等 acquire 期间空转，CUDA 计算时间全额串行追加。正确性由 external semaphore 保证（与提交先后无关），但重叠收益取决于提交顺序——先 CUDA 让 CUDA 引擎在 acquire 等待期间就在算。

**性能特征**：

```
T_呈现 = T_acquire + T_blit + T_ImGui + T_present
每帧时间 = max(T_raygen, T_tonemap + T_呈现)
```

- 双 stream 下 T_raygen 与 T_tonemap 重叠（不相加），瓶颈判断基于 raygen 单项
- 若 T_raygen > T_tonemap + T_呈现：瓶颈在累积，其余全部被隐藏
- 若 T_raygen < T_tonemap + T_呈现：瓶颈在呈现侧，无法完全隐藏

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

**决策**：Binary semaphore，forward 方向 per-frame-in-flight（2 个），reverse 方向单个（共 3 个）。

| 方向 | 数量 | CUDA 侧 | Vulkan 侧 | 保护的依赖 |
|------|------|---------|-----------|-----------|
| CUDA→Vulkan（forward） | per-frame（2） | signal（display_stream，tonemap 后） | wait（submit，blit 前） | blit 读 display_surface 前 tonemap 写完 |
| Vulkan→CUDA（reverse） | 单个（1） | wait（display_stream，tonemap 前） | signal（submit，blit 后） | tonemap 写 display_surface 前 blit 读完 |

**理由**：
- 显示 buffer 只有一份，CUDA tonemap 写、Vulkan blit 读，存在 write-after-read 依赖，reverse 不可或缺
- forward 必须 per-frame：signal 端在 CUDA display_stream、wait 端在 Vulkan submit2，分属两个独立 engine；2 frames in flight 下共用单个 forward semaphore 时，GPU 上两次 CUDA signal 之间无法保证夹着一次 Vulkan wait，会违反 binary semaphore 约束。Per-frame 分开后，同一 slot 的 fence wait 保证上一次 wait 完成才允许下一次 signal
- reverse 可单个：reverse 的每次 signal 都在 submit2 的「wait forward_sem → blit → signal reverse_sem」序列中，而 forward 是 per-frame 的，这条链把 reverse 的 signal/wait 强制成严格交替——每个 reverse signal 之前必有一个 reverse wait（上帧消费），不会连续 signal。因此 reverse 无需 per-frame
- 单向（仅 forward）时，display_surface 的 write-after-read 靠 T_blit << T_raygen 的隐式间隙——形式上不正确
- reverse 在 T_blit << T_raygen 时不损失并行度：blit 在 raygen 期间完成，reverse signal 远早于 raygen_done event，tonemap 启动时机仍由 raygen_done 决定
- 首帧由 init 中的 pre-signal 闭合（reverse_sem 预先 signal，使首帧 CUDA wait 立即通过）
- acquire 失败时 Vulkan submit 被跳过，drain submit 需代为 signal reverse semaphore（否则下一帧 CUDA wait 挂起）

### 测试 Kernel 归属

**决策**：测试 kernel 归 renderer 层，不归 optix 层。

测试 kernel 是“画什么”（渲染内容），renderer 层职责。optix 层是 OptiX/CUDA 封装层，只提供纯能力（如导入后的 surface object 句柄），不含渲染逻辑（遵循 `docs/architecture.md` 的层次约束）。renderer 拿 optix 提供的句柄 + 自己持有的帧号计数器去 launch kernel。

**理由**：
- optix 层不包含渲染逻辑是硬约束（编译期单向依赖的语义边界）
- 帧号计数器是渲染状态（时间累积/噪声变化用途），天然归 renderer 层，与 Himalaya `Renderer::frame_counter_` 一致

### CUDA Stream 架构

**决策**：双显式 stream（`compute_stream` + `display_stream`），均由 `optix::Context` 持有。

| Stream | 每帧工作 |
|--------|---------|
| `compute_stream` | params upload + optixLaunch（raygen） |
| `display_stream` | tonemap + signal interop semaphore |

**理由**：
- 默认流会与所有显式流隐式同步，等于全局序列化点，堵死重叠空间
- raygen 写 buf[X] 而 tonemap 读 buf[Y]（ping-pong），无数据依赖，可并行
- 单 stream 下 tonemap 被迫串行等 raygen 完成，raygen 连续执行的间隙 = T_tonemap；双 stream 下间隙 ≈ 0

**跨 stream 同步**：4 个 CUDA event（`renderer::Renderer` 持有，按 `frame_counter_ % 2` 双缓冲），保护 ping-pong buffer 的跨帧读写依赖（详见 `current-phase.md` 双 Stream 流水线节）。

### Renderer 内拆分

**决策**：Renderer 拆分 `submit_cuda`（CUDA launch + external semaphore signal）与 `record_vulkan`（Vulkan 命令录制 = blit + ImGui + layout 转换）两个公共方法，由 Application 在帧循环中按顺序编排（先 CUDA 后 acquire）。Application 保留 begin/end command buffer 与 submit/present 的时序骨架。

**理由**：
- 一帧要画什么是完整单元（CUDA 写 buffer + Vulkan blit + ImGui 叠加），其内容归 renderer；但 submit_cuda 与 acquire 的先后决定 GPU CUDA 引擎启动时机与重叠收益，属“驱动时序”。故 renderer 内拆两个方法、Application 掌握调度时机，各取职责。
- CUDA launch/signal 是异步提交，不在 command buffer 里，属“驱动时序”的一部分；但内容紧耦合、不能拆到 app。
- 与 Himalaya 一致：其 Renderer 同样封装渲染内容；Qualquer 应用层负责 CUDA 提交时序是其区别

### ImGuiBackend 归属

**决策**：`ImGuiBackend` 代码归 vulkan 层（呈现基础设施），所有权归 Application，Renderer 经 `RenderInput` 引用传入，不缓存为成员。

**理由**：
- ImGuiBackend 的职责是把 ImGui draw data 通过 Vulkan 管线渲染到 swapchain image——这是”呈现”，不是”决定画什么”。它不含任何渲染逻辑（不决定 UI 面板内容），只做 Vulkan 渲染后端 + GLFW 平台后端的生命周期管理和 draw data 录制
- UI 面板内容（debug_ui）归 renderer——它决定”画什么”（哪些滑块、哪些数据）
- ImGui 生命周期绑定窗口/输入（GLFW、DeltaTime、begin_frame 需事件后状态），所有权归 app
- Renderer 作为使用方不缓存 Application 拥有的引用（项目所有权原则：使用方不在成员中缓存所有者拥有的引用），故所有依赖（cuda_context/display_buffer/swapchain/imgui）均走 RenderInput，Renderer 唯一成员是自有的 frame_counter_。与 Himalaya 缓存指针成员的做法不同——Qualquer 的所有权原则更严格

### CUDA 每帧工作

**参数传输**：`cudaMemcpyAsync`

每帧更新的参数（相机矩阵、帧计数等）使用异步传输。同步版本会隐式等待 GPU 空闲，强制串行化。

**Kernel 启动**：每帧 launch

不使用 persistent kernel。launch 开销（~5-10 微秒）相比累积时间（~1ms）可忽略，不值得增加复杂度。

**隐藏条件**：

只要 T_CPU工作 < T_累积（CPU 的 memcpy 排队 + launch + Vulkan 录制在累积时间内完成），CPU 工作被完全隐藏。

### 后续改进方向

1. **单帧多 sample 累积**：Phase 4 已决策（D4），raygen 内 sample 循环，max 64 spp/frame
2. **自适应采样**：D16 决策仅手动 `samples_per_frame` 参数，自适应逻辑不纳入 Phase 4/4.5

---

## 纹理压缩

### BC6H 压缩方式

**决策**：ISPCTextureCompressor CPU ISPC 压缩，不使用 GPU compute shader。

**理由**：
- Qualquer 无 Vulkan compute 基础设施（无 compute pipeline、push descriptor、ShaderCompiler），为 BC6H 单一功能引入整套违背 KISS
- Himalaya 的 GPU BC6H 压缩（`texture_compress.cpp`）需 ~200 行 Vulkan 管线编排（pipeline 创建、descriptor layout、staging buffer、per-face × per-mip 循环、4 层 barrier），复杂度与收益不成比例
- GPU BC6H encoder 受 shared memory / register 限制，只尝试少量模式；ISPCTextureCompressor 的 `veryslow` profile 可做更多 refine iteration，质量更优
- IBL 纹理是离线预处理（一次压缩、永久缓存），CPU 耗时完全可接受
- 项目已有 ISPC 语言支持（bc7enc），ISPCTextureCompressor 集成为同类操作——复制 `kernel.ispc` + wrapper，无新构建依赖

**备选（已排除）**：
- **GPU compute shader（Himalaya 方式）**：需引入完整 Vulkan compute 管线 + `bc6h.comp` shader，复杂度远超收益
- **其他 CPU BC6H 编码器**（如 DirectXTex）：ISPCTextureCompressor 是 Intel 官方 ISPC 实现，与项目已有 ISPC 工具链一致，且有多档质量 profile

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

### 帧同步 Semaphore 归属

**image_available_semaphore**：per-frame-in-flight（2 个），在 FrameData 中。

acquire 时 signal，submit 时 wait。fence 保证同一 slot 重用时上一次的 signal/wait 已完成，配对成立。

**render_finished_semaphore**：per-swapchain-image（跟随 swapchain image 数量），在 Swapchain 中。

submit 时 signal，present 时 wait。`vkQueuePresentKHR` 的 semaphore wait 由 presentation engine 异步消费，不受 render fence 约束。如果用 per-frame（2 个），当 swapchain image 数量 > frames in flight 时，同一 slot 的 fence wait 通过后 presentation engine 可能仍未消费上一次的 semaphore，导致对 signaled 状态的 binary semaphore 再次 signal，违反 spec。Per-swapchain-image 下，同一 image index 在 presentation engine 释放前不会被 acquire 返回，保证 semaphore 已被消费。

与 Himalaya 一致。

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

### Swapchain 重建触发

**决策**：recreate 由两类信号源 OR 触发——主动尺寸比较（轮询）OR 驱动返回值（acquire/present）。

```
acquire 返回 OUT_OF_DATE                              → recreate，跳过本帧
present 返回 ∈ {OUT_OF_DATE, SUBOPTIMAL}              → present 后 recreate
或 本帧轮询发现 fb != swapchain.extent                → present 后 recreate
```

**理由**：
- 两类信号源覆盖不同失败场景，互补，缺一不可：
  - 主动尺寸比较能抓"窗口刚 resize，驱动还没报错"这一帧（否则画面拉伸/黑边）
  - 驱动返回值能抓"尺寸没变但 swapchain 因别的原因失效"（surface lost 等）
- 只靠驱动返回值不可靠：Windows 上 surface 尺寸变化不一定让当帧 acquire/present 立即报错
- 只靠尺寸比较会漏掉非尺寸原因的失效

### 尺寸变化检测机制：轮询 vs 回调

**决策**：轮询 `glfwGetFramebufferSize` 比对 `swapchain.extent`，不用 GLFW 回调设标志。

**理由**：
- 最小化检测**必须轮询**尺寸（`while fb==0 glfwWaitEvents`），这条路径每帧已查询 framebuffer size
- resize 检测顺手在同一次查询里比对即可，无需引入额外机制
- 回调+标志与轮询是同一信息源的两种实现，二选一；最小化已逼出轮询，回调就被吸收

### 窗口最小化处理

**决策**：整循环阻塞（poll events 后、begin_frame 前，`while fb==0 glfwWaitEvents` 阻塞）。最小化期间完全不进帧循环。

**理由**：
- `glfwWaitEvents` 是可中断睡眠，任何事件（含恢复窗口）都唤醒，不卡死
- 最小化时 GPU 不空转渲染、CPU 不忙循环轮询，省资源
- Qualquer 无"最小化时仍需每帧执行的非渲染逻辑"，非阻塞跳帧（轮询但跳过渲染）的灵活性用不上

### 错误处理

**决策**：VK_CHECK 宏，失败时打印错误并立即 abort。

**理由**：
- Vulkan 错误通常不可恢复
- 简单直接，错误发生时日志已打印
- 与 Himalaya 一致

---

## 初始化

### 初始化顺序

**决策**：三段式（Vulkan pre-init → CUDA init → Vulkan init）。

Vulkan pre-init 创建 instance/surface 后枚举支持 present 的物理设备并查 UUID；CUDA 在该候选列表约束内打分选设备；Vulkan init 按选中 UUID 完成 device/queue/resource 创建。

**理由**：
- present 约束前置，排除不可呈现的 CUDA 设备（如 TCC 计算卡）
- CUDA 与 Vulkan 通过 UUID 对齐到同一物理 GPU
- 早期“CUDA → Vulkan 单向”是软理由（省 CUDA 失败的 Vulkan init），但该场景选出的 CUDA 设备可能不可 present，被三段式推翻——依据“决策可更新性”原则

### 设备检测

**决策**：CUDA 初始化时检测。

**理由**：
- CUDA init 自然检测 NVIDIA GPU 可用性和 compute capability
- Vulkan 设备经 UUID 匹配 CUDA 选中的设备

---
