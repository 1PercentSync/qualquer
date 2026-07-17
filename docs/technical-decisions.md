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

**决策**：FrameSlot ping-pong（color + aux + metadata × 2）+ DLSS 中间 HDR buffer（1）+ 显示 buffer（1）

| Buffer | 数量 | 格式 | 所有权 | 用途 |
|--------|------|------|--------|------|
| FrameSlot color | 2 | RGBA32F（CudaArrayBuffer） | CUDA 独占 | DLSS OFF：Separate Sum 累积；DLSS ON：单帧 noisy HDR |
| FrameSlot aux | 2 套 × 6 | depth/MV/albedo/normal/roughness | CUDA 独占 | DLSS-RR guide；与同槽 color/metadata 同帧产出 |
| DLSS 中间 HDR | 1 | RGBA32F（CudaArrayBuffer，显示分辨率） | CUDA 独占 | DLSS-RR 输出 → tonemap 输入；仅 DLSS ON |
| 显示 buffer | 1 | R8G8B8A8_UNORM | Vulkan 分配，CUDA 导入 | LDR 输出，Vulkan blit 到 swapchain |
MUSTREAD:8

**帧流程（DLSS OFF）**：

| 帧 | raygen（compute_stream） | tonemap（display_stream） | Vulkan |
|----|------|--------------|--------|
| N | 读 slot A，写 slot B | 读 slot A → 显示 buffer | blit 显示 buffer |
| N+1 | 读 slot B，写 slot A | 读 slot B → 显示 buffer | blit 显示 buffer |

**帧流程（DLSS ON）**：

| 帧 | raygen（compute_stream） | display_stream | Vulkan |
|----|------|--------------|--------|
| N | 写 slot B（noisy + aux + metadata） | 读 slot A → DLSS-RR → 中间 HDR → tonemap → 显示 buffer | blit 显示 buffer |
| N+1 | 写 slot A | 读 slot B → DLSS-RR → … | blit 显示 buffer |

- display 始终消费上一帧 write 的 read slot（显示延迟一帧）
- raygen 与 display 在不同 stream 上并行（操作不同 slot，FrameSlot production/consumption event 同步）
- Tone mapping 在 CUDA 侧完成（HDR→LDR），Vulkan 只做 blit

**帧循环顺序**：

```
1. CUDA：submit_cuda 向两条 stream 提交工作
   compute_stream: raygen
   display_stream: (DLSS-RR evaluate)? + tonemap + signal
2. Vulkan：acquire + blit + ImGui + present
```

先 CUDA 后 acquire，使 acquire 等待时间被 raygen 隐藏。

**为什么提交顺序重要（异步 ≠ 重叠）**：异步提交只保证 CPU 不阻塞，不保证 GPU 引擎何时启动。submit_CUDA 一调用，GPU 的 CUDA 引擎立刻开始算；若 acquire 在前，CUDA 引擎在 CPU 等 acquire 期间空转，CUDA 计算时间全额串行追加。正确性由 external semaphore 保证（与提交先后无关），但重叠收益取决于提交顺序——先 CUDA 让 CUDA 引擎在 acquire 等待期间就在算。

**性能特征**：

```
T_呈现 = T_acquire + T_blit + T_ImGui + T_present
每帧时间 = max(T_raygen, T_display + T_呈现)
```

- 双 stream 下 T_raygen 与 T_display（DLSS+tonemap 或仅 tonemap）重叠（不相加），瓶颈判断基于 raygen 单项
- 若 T_raygen > T_display + T_呈现：瓶颈在 path tracing，其余全部被隐藏
- 若 T_raygen < T_display + T_呈现：瓶颈在呈现侧，无法完全隐藏

**理由**：
- color/aux 不需要 interop，简化同步
- 显示 buffer 是 LDR（R8G8B8A8），带宽最小
- 单显示 buffer 在 T_raygen > T_呈现 时足够（典型 PT 场景）
- DLSS 中间 HDR 仅在 ON 时使用，与 tonemap 同 stream 顺序执行

**同槽所有权（FrameSlot）**：color、全部 aux 与 host metadata 必须同一 ping-pong slot 生产/消费。evaluate 由 read slot validity 驱动，而非「固定延迟一帧」或单份 aux + 额外串行化。单份 aux 会在双 stream 下被 compute 覆盖 display 正在读的 guide data；即便消除物理竞争，previous color 与 current aux/jitter/矩阵仍构成帧错配。

**Separate Sum + per-slot 计数**：累积用 `sum_new = sum_old + frame_total`，tonemap 除以与 buffer 同槽的 sample count。全局 count + 清零两 buffer 在 reset 帧会与同帧 tonemap 读产生跨 stream 竞态并导致黑帧。reset 时 `chain_count = 0` 使 raygen 覆写 write slot（不读 read），等效清零且无 memset。`frame_counter_` 永不 reset（%2 选 slot，并上传为 device `frame_index` 作 RNG 时域源），不单设 `frame_seed_`。

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
| `display_stream` | DLSS-RR evaluate（ON 时）+ tonemap + signal interop semaphore |

`compute_stream` 为 non-blocking stream；`display_stream` 为 blocking stream。

**理由**：
- 默认流会与所有显式流隐式同步，等于全局序列化点，堵死重叠空间
- raygen 写 buf[X] 而 tonemap 读 buf[Y]（ping-pong），无数据依赖，可并行
- 单 stream 下 tonemap 被迫串行等 raygen 完成，raygen 连续执行的间隙 = T_tonemap；双 stream 下间隙 ≈ 0
- **display 必须 blocking**：DLSS-RR feature 绑在 display stream 上，其 CUDA 路径仍可能提交 legacy default-stream work；non-blocking display 的前置等待与 completion event 不覆盖该顺序，SER + 高帧率会放大时序敏感性。blocking display 恢复与 default stream 的排序，且不取消 compute/display 并行。若后续再出现偶发崩溃，用标准 `optixTrace` 做稳定性对照后再决定是否关 SER。

**跨 stream 同步**：FrameSlot 内 production event（compute_stream raygen 完成后 record）和 consumption event（display_stream tonemap 完成后 record），按 ping-pong slot 双缓冲，保护跨帧读写依赖。

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
- Renderer 作为使用方不缓存 Application 拥有的引用（项目所有权原则：使用方不在成员中缓存所有者拥有的引用），故帧循环依赖（cuda_context/display_buffer/swapchain/imgui）均走 RenderInput / SceneRenderInput，不在成员中缓存。Renderer 自有状态包括 frame_counter_、FrameSlot ping-pong、pipeline/SBT、DlssRR 生命周期与 timing events 等——这些是渲染内容的所有者资源，不是对 Application 句柄的缓存。与 Himalaya 缓存指针成员的做法不同——Qualquer 的所有权原则更严格

### CUDA 每帧工作

**参数传输**：`cudaMemcpyAsync`

每帧更新的参数（相机矩阵、帧计数等）使用异步传输。同步版本会隐式等待 GPU 空闲，强制串行化。

**Kernel 启动**：每帧 launch

不使用 persistent kernel。launch 开销（~5-10 微秒）相比累积时间（~1ms）可忽略，不值得增加复杂度。

**隐藏条件**：

只要 T_CPU工作 < T_累积（CPU 的 memcpy 排队 + launch + Vulkan 录制在累积时间内完成），CPU 工作被完全隐藏。

---

## Path Tracer 内核

### Kernel 架构

**决策**：Megakernel + SER。演进：需要 ReSTIR 等全局同步时迁 Multi-launch；体积渲染再评估 Wavefront。

**理由**：SER 消除 bounce 间 divergence，零中间 ray state buffer。Multi-launch / Wavefront 的全局 buffer 与实现复杂度在当前无 ReSTIR 时收益不足。

### Payload

**决策**：全信息 payload registers（当前 18：含 ray cone 预留 p16/p17），不走 global hit buffer。

**理由**：SER 重排后 payload 随线程移动、零全局内存；global buffer 在重排后访问变为 non-coalesced，反而更差。OptiX 上限 32 registers。

### Device 代码组织

**决策**：单 `programs.cu` + 多 `.cuh`（`__forceinline__ __device__`），不做多 Module linking。

**理由**：全局内联对 megakernel 性能关键；多 module 跨模块调用受限且构建复杂；单 module 千行级编译不构成瓶颈。

### 每帧多 Sample

**决策**：raygen 内 sample 循环（单次 `optixLaunch`，寄存器局部累积）。硬上限 `max_samples_per_frame = 64`（TDR 安全）。

**理由**：相对每 sample 一次 launch 或 3D launch，带宽与 launch 开销最优；寄存器仅多约 3 floats + counter。

### Multi-launch 迁移预留

**决策**：三项零运行时开销的代码组织——`PathState` 结构体、payload pack/unpack helpers、bounce loop body 隔离为 device 函数。

**理由**：Megakernel 阶段 PathState 是局部变量；迁 Multi-launch 时变为全局 buffer 元素类型，改布局与调用形态只动封装点。

### SBT / Pipeline

**决策**：2 miss（`__miss__env` + `__miss__shadow`）+ **1 hitgroup**（CH+AH）；stride=0；BLAS per-geometry flag 控制 anyhit（opaque: `DISABLE_ANYHIT`，non-opaque: `REQUIRE_SINGLE_ANYHIT_CALL`）。Shadow ray 用 `DISABLE_CLOSESTHIT`，与 bounce 共享 hitgroup。

**理由**：否定 2 hitgroup + stride 方案——CH/AH 独立入口与寄存器分配，opaque 不链 AH 无寄存器收益；SER hint 已用 material 信息；BLAS geometry flag 是 OptiX 控制 anyhit 的原生机制。

---

## 着色与光照

### BRDF 参数约定

**决策**：specular 微面元原语接收 **alpha**（= roughness²）；EON / E_ss / E_FON / CLTC 接收 **linear roughness**。closesthit 单点算 alpha 并 clamp，原语内不重复 clamp。Visibility 命名为 `V_SmithGGXCorrelated`。

**理由**：两套惯例来自不同数学源头（Heitz 微面元 vs 多项式拟合变量），无法统一。与 Himalaya「全收 roughness、函数内平方」不同：集中算 alpha 避免重复平方并让 NaN 防护可见。命名显式标注 height-correlated G2，避免与 separable 混淆。

### 多散射能量补偿

**决策**：Turquin 对 specular 乘增益；`E_ss` 用 Sforza 19 系数有理多项式（零 LUT）。Diffuse 权重为 `(1 - metallic) × (1 - E_glossy)`，`E_glossy` 为 Sforza 39 系数多项式。

**理由**：Heitz 随机游走过慢；Kulla-Conty 需改采样；Turquin 不改采样，单向 PT 不要求互易。多项式精度足够且无预计算资源。`(1-F)` 忽略微面元多次弹跳后进入 diffuse 的能量，高 roughness + 掠射偏差大。

### Diffuse 模型

**决策**：EON（Energy-Preserving Oren–Nayar）+ CLTC 采样，不用 Lambertian / Burley。

**理由**：能量守恒且保持、互易、粗糙回射、OpenPBR 采纳；CLTC 在掠射角相对 cosine 半球大幅降方差。GPU PT 上相对 Lambertian 开销可忽略。

### Fresnel

**决策**：仅标准 `F_Schlick(VdotH, F0)`（F90=1 隐含）。不做 F82-tint；不暴露 F90 参数重载。

**理由**：F82-tint 依赖未解析的 KHR_materials_specular；涂层与 F90≠1 场景不在范围内；金属能量补偿走独立 Turquin 增益。

### 光源范围

**决策**：仅 IBL + emissive 三角形 NEE + MIS（power heuristic）。不实现 glTF punctual lights。Env alias table **全分辨率**（equirect 像素级）。

**理由**：解析光源与 IBL/emissive 重复覆盖照明；全分辨率保持角度精度与 PDF 一致（Himalaya ÷2 是 VRAM 取舍，非业界默认）。hot pixel 过度集中时再评估降采样。

### 环境贴图表示

**决策**：equirect 加载后转 cubemap，`texCubemap` 采样。

**理由**：硬件选面、无极地畸变、边界 filtering 优于 equirect 直采；代价为一次性转换与约 1.5× 显存。

### 明确不纳入

| 项 | 理由 |
|----|------|
| Firefly clamping | 有 bias；降噪/DLSS-RR 可处理极亮点 |
| `indirect_intensity` 乘数 | 非物理艺术控制 |
| Target sample count / auto-stop | DLSS-RR 内部管理时域历史，无有意义的收敛停止点 |

### Stochastic Alpha 采样

**决策**：`alpha_mode == Blend` 时用 **hash**（pixel / sample / primitive）与 texel alpha 比较决定 `optixIgnoreIntersection`，不用 Sobol 维度。

**理由**：anyhit 调用次数与顺序不确定，Sobol 维度分配模式不适用。

---

## 采样与 RNG

### RNG

**决策**：Sobol（128 维 direction numbers）+ per-pixel **加法** Cranley-Patterson rotation + golden-ratio temporal offset（`frame_index * 2654435769u`）；dim ≥ 128 降级 xxhash32。**不用** blue noise 纹理。表以 `uint32_t sobol_directions[4096]` **内嵌 LaunchParams**，经 `optixLaunch` 进入 `__constant__`。

**理由**：DLSS-RR 文档将 blue noise 列为应避免；加法 CP 保持低差异性（XOR 破坏分层）。OptiX 无公开 API 初始化非 LaunchParams 的 `__constant__`，内嵌是唯一把自定义表放进 constant memory 的方式。

### 多 spp 与 jitter

**决策**：DLSS ON 时帧内所有 sample **共享**同一 subpixel jitter（仅跨帧变化）；DLSS OFF 时 per-sample jitter。BRDF/NEE 维度始终 per-sample。

**理由**：DLSS 每帧只收一组 aux 与一个 `InJitterOffset`；共享 jitter 使 primary 一致、aux 写一次。OFF 时 per-sample jitter 加速 MC 收敛。

---

## 降噪与分辨率

### DLSS-RR

**决策**：DLSS Ray Reconstruction（CUDA API）替代 OptiX Denoiser。管线：raygen → DLSS-RR → tonemap。Separate Sum 为 DLSS OFF 的长期 fallback。

**理由**：DLSS-RR 同时做时域累积、去噪、放大，直接到达实时 PT 终态；OptiX Denoiser 只做去噪。

### DLSS guide 关键约定

**决策**：
- specular albedo = 逐通道 `E_glossy`（与 Turquin 补偿后 specular 能量自洽）
- specular hit distance 传 **nullptr**（不分配）
- diffuse albedo 暂传 raw `base_color`（是否预乘 `1-metallic` 存疑；出现相关伪影时优先排查）

**理由**：EnvBRDFApprox2 与项目多散射 specular 不一致时，guide 与 color 自洽优先于匹配训练分布。nullptr 比填 infinity 更诚实；SDK 在无 hit distance 时回退 2D MV 推导 specular motion。

### 渲染 / 显示分辨率

**决策**：渲染分辨率独立于 swapchain；display buffer 始终 swapchain 大小。缩放在 tonemap kernel 内、**线性 HDR（mean）** 上完成，再 exposure + tonemap 写全幅 display；VK blit 保持 1:1。

**理由**：blit 滤波发生在 tonemap 后的 LDR，无法对缩小做 footprint 滤波，也无法在线性空间高质量放大。DLSS ON 时输出已是显示分辨率；OFF 时本路径为长期 fallback。

### 自适应帧率（策略）

**决策**：三级降级，通过调节 `samples_per_frame` 逼近目标帧率；与呈现模式（MAILBOX/FIFO）解耦。

| Mode | 架构 | 条件 | 目标帧率 |
|------|------|------|----------|
| 1 | ping-pong 并行 | 1spp 足够快 | min { n×refresh ≥ 150 } |
| 2 | 串行 | 无法达 Mode 1 | refresh（<150Hz）或 refresh/2（≥150Hz） |
| 3 | 串行 | 无法达 Mode 2 | 反复减半；低于 60fps 则 1spp 放开跑 |

**理由**：ping-pong 多一帧显示延迟，需 ≥150fps 才不可感知；达不到则串行换更低目标以消除额外延迟。整数倍刷新率避免微卡顿。

---

## 呈现与 UI

### Tonemap 与 Exposure

**决策**：Khronos PBR Neutral。Exposure 语义为线性倍率；app 存 EV 并 `pow(2, ev)` 后传入 renderer，renderer 只做 `color * exposure`。

**理由**：PBR Neutral 在阈值下 1:1 还原 baseColor、仅压高光。EV 摄影概念留在 app，renderer 职责单一。

### UI 领域结构

**决策**：`RenderSettings`（可调旋钮）+ `SceneStats`（只读资产快照），DebugUIContext 组合引用；参数变化由 Renderer 比对前帧值检测。

**理由**：避免 Context 散装标量膨胀。near/far 不暴露——perspective unproject 后 near/far 缩放在 normalize 中消失，PT 无深度缓冲消费 depth mapping。

### 异步上传生命周期

**决策**：host→device 异步 copy 按 host source **所有权批次**同步；同步点在任一局部 source 失效之前。不用 `cudaDeviceSynchronize`。

**理由**：局部加载缓冲若在 stream copy 完成前析构，违反 upload 契约。批内共享一次同步保留并行；加载路径不值得上持久 staging + event 回收。

### 数值正确性策略

**决策**：按责任域在源头修正并验证（资产输入 → geometry/shading frame → BRDF/能量 → importance distribution → 直接光/path estimator → 时域/guide data）。全部源头完成前，不以最终 radiance clamp / 非有限清零作为统一兜底。

**理由**：边界 sanitize 会掩盖真实错误，使单项修正无法验证。边界防御不承担修正物理模型的职责。

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
