# 技术选型与决策

> 渲染器当前使用的技术方案、选型理由和关键实现决策。
> 架构层面的设计见 `architecture.md`。
> 本文件按关注域组织；条目为某时刻最优解快照，可按 `architecture.md`「决策可更新性」更新。

---

## 平台与初始化

### IR 加载方式

**决策**：构建期 nvcc `--optix-ir` 编译为 .optixir 文件，运行时从文件加载，不嵌入可执行文件。

**理由**：

- 嵌入（bin2c 转 C 数组）引入 bin2c 的 size 符号命名、stdint 依赖、padding 等复杂度；对单一 module
  的项目，其收益（免去运行时文件路径管理）不足以抵消——违反 architecture.md「复杂度与收益成正比」
- 不嵌入下，改 .cu 只需 CMake 重编 .optixir（incremental），不重新链接整个 exe，开发迭代快
- 本地学习项目不分发，.optixir 部署到可执行目录旁的路径管理开销可控

**备选（已排除）**：

- **嵌入（bin2c）**：bin2c `--length` 生成的符号（`uint32_t <name>Length`）与设计约定的 `programs_optixir_size` 不符，需额外
  wrapper 拼接，得不偿失
- **运行时 NVRTC 编译**：OptiX `.cu` 需运行时访问 OptiX headers 路径，且单 module 用不上磁盘缓存收益，复杂度高于构建期 nvcc

**部署**：app 目标用 dependency-tracked copy（`OUTPUT` + `DEPENDS` 到可执行目录 `shaders/`）， **不用** `POST_BUILD`——仅改
device 代码时 host 不 relink，`POST_BUILD` 会继续部署陈旧 IR。形态上仍是「构建期编译 + 部署后运行时加载」；himalaya 用
POST_BUILD 复制源文件再 运行时编译，Qualquer 编译留在构建期（nvcc），运行时只读已编译的 .optixir。

### 初始化顺序

**决策**：三段式（Vulkan pre-init → CUDA init → Vulkan init）。

Vulkan pre-init 创建 instance/surface 后枚举支持 present 的物理设备并查 UUID；CUDA 在该候选列表约束内打分选设备；Vulkan
init 按选中 UUID 完成 device/queue/resource 创建。

**理由**：

- present 约束前置，排除不可呈现的 CUDA 设备（如 TCC 计算卡）
- CUDA 与 Vulkan 通过 UUID 对齐到同一物理 GPU
- 早期“CUDA → Vulkan 单向”是软理由（省 CUDA 失败的 Vulkan init），但该场景选出的 CUDA 设备可能不可
  present，被三段式推翻——依据“决策可更新性”原则

---

## 帧管线

### 内存所有权

**决策**：Vulkan 分配，CUDA 导入。

Vulkan 创建 VkImage（启用 external memory 标志），导出 Win32 HANDLE，CUDA 通过 `cudaImportExternalMemory` 映射后写入。

**理由**：

- 主流方案，NVIDIA 官方示例和文档均采用此方式
- Vulkan 原生拥有 VkImage，布局转换和验证层检查完整
- 格式兼容性由 Vulkan 明确定义

备选方案（CUDA 分配，Vulkan 导入）文档稀缺、验证层对外部导入内存检查有限，排除。

### Buffer 架构

**决策**：FrameSlot 双缓冲（每槽 color + aux + metadata，共 2 槽，CUDA 独占）+ DLSS 中间 HDR（显示分辨率；仅 DLSS-RR 开启时使用）+
单份 LDR 显示 buffer（Vulkan 分配、CUDA 导入，blit 到 swapchain）。display 消费上一帧 write 的 read slot；HDR→LDR 在 CUDA
tonemap，Vulkan 只 blit。不直接写入 swapchain image。

MUSTREAD:8

**理由（独立中间 buffer，不写 swapchain）**：

- Swapchain image 由 WSI 创建，无法添加 external memory 标志，不能导出给 CUDA
- Swapchain 格式通常是 8-bit sRGB，不适合 HDR 累积（需要浮点格式避免量化误差）
- Blit 是后处理的天然边界（tone mapping、gamma 校正）

**同槽所有权**：color / aux / host metadata 必须同 slot 生产消费；evaluate 由 read slot validity 驱动。单份 aux 在双 stream
下会与 display 竞态，且 color 与 aux/jitter/矩阵易帧错配。

**Separate Sum + per-slot 计数**：tonemap 除数与所读 buffer 同槽。全局 count + 清零在 reset 帧与 tonemap 跨 stream 竞态；改
`chain_count = 0` 覆写 write slot。`frame_counter_` 永不 reset（选 slot + device `frame_index`），不单设 `frame_seed_`。

**提交顺序**：先 `submit_cuda` 再 acquire。异步只保证 CPU 不阻塞；先 CUDA 才能在 acquire 等待期间启动计算（正确性靠 external
semaphore，重叠靠顺序）。

### 显示 Buffer

**决策**：中间图 `R16G16B16A16_SFLOAT` + `OPTIMAL` tiling；export 内存手写 `vkAllocateMemory`（dedicated + Win32 HANDLE），不走
VMA。Swapchain 为 `B8G8R8A8_SRGB`（blit 时硬件 swizzle + 线性→sRGB 编码）。tonemap 写 **线性 LDR** float/half，不在 CUDA 侧做
8-bit 量化或 sRGB 编码。

**理由**：`R8G8B8A8_UNORM` 在 sRGB 编码前对线性 LDR 做 8-bit 量化，阴影易出现色阶断层。RGBA16F 保留线性中间精度；最终 8-bit
量化与 sRGB 编码发生在 blit 到 swapchain 的一步。与 HDR 累积同为 float 通道序、免 kernel swizzle；LINEAR tiling 限制多；带
`VkExportMemoryAllocateInfo` 的分配不宜走 VMA 常规路径。

### Interop 同步

**决策**：Binary semaphore，forward 方向 per-frame-in-flight（2 个），reverse 方向单个（共 3 个）。

| 方向                   | 数量           | CUDA 侧                              | Vulkan 侧                 | 保护的依赖                              |
|------------------------|----------------|--------------------------------------|---------------------------|-----------------------------------------|
| CUDA→Vulkan（forward） | per-frame（2） | signal（display_stream，tonemap 后） | wait（submit，blit 前）   | blit 读 display_surface 前 tonemap 写完 |
| Vulkan→CUDA（reverse） | 单个（1）      | wait（display_stream，tonemap 前）   | signal（submit，blit 后） | tonemap 写 display_surface 前 blit 读完 |

**理由**：

- 显示 buffer 只有一份，CUDA tonemap 写、Vulkan blit 读，存在 write-after-read 依赖，reverse 不可或缺
- forward 必须 per-frame：signal 端在 CUDA display_stream、wait 端在 Vulkan submit2，分属两个独立 engine；2 frames in flight
  下共用单个 forward semaphore 时，GPU 上两次 CUDA signal 之间无法保证夹着一次 Vulkan wait，会违反 binary semaphore
  约束。Per-frame 分开后，同一 slot 的 fence wait 保证上一次 wait 完成才允许下一次 signal
- reverse 可单个：reverse 的每次 signal 都在 submit2 的「wait forward_sem → blit → signal reverse_sem」序列中，而 forward 是
  per-frame 的，这条链把 reverse 的 signal/wait 强制成严格交替——每个 reverse signal 之前必有一个 reverse wait（上帧消费），不会连续
  signal。因此 reverse 无需 per-frame
- 单向（仅 forward）时，display_surface 的 write-after-read 靠 T_blit << T_raygen 的隐式间隙——形式上不正确
- reverse 在 T_blit << T_raygen 时不损失并行度：blit 在 raygen 期间完成，reverse signal 远早于 raygen_done event，tonemap
  启动时机仍由 raygen_done 决定
- 首帧由 init 中的 pre-signal 闭合（reverse_sem 预先 signal，使首帧 CUDA wait 立即通过）
- acquire 失败时 Vulkan submit 被跳过，drain submit 需代为 signal reverse semaphore（否则下一帧 CUDA wait 挂起）

### CUDA Stream 架构

**决策**：双显式 stream（`compute_stream` + `display_stream`），均由 `optix::Context` 持有。

| Stream           | 每帧工作                                                      |
|------------------|---------------------------------------------------------------|
| `compute_stream` | params upload + optixLaunch（raygen）                         |
| `display_stream` | DLSS-RR evaluate（ON 时）+ tonemap + signal interop semaphore |

`compute_stream` 为 non-blocking stream；`display_stream` 为 blocking stream。

**理由**：

- 默认流会与所有显式流隐式同步，等于全局序列化点，堵死重叠空间
- raygen 写 buf[X] 而 tonemap 读 buf[Y]（ping-pong），无数据依赖，可并行
- 单 stream 下 tonemap 被迫串行等 raygen 完成，raygen 连续执行的间隙 = T_tonemap；双 stream 下间隙 ≈ 0
- **display 必须 blocking**：DLSS-RR feature 绑在 display stream 上，其 CUDA 路径仍可能提交 legacy default-stream
  work；non-blocking display 的前置等待与 completion event 不覆盖该顺序，SER + 高帧率会放大时序敏感性。blocking display 恢复与
  default stream 的排序，且不取消 compute/display 并行。若后续再出现偶发崩溃，用标准 `optixTrace` 做稳定性对照后再决定是否关
  SER。

### Vulkan 帧同步与 in-flight

**决策**：frames in flight = 2。`image_available` per-frame（FrameData）；`render_finished` **per-swapchain-image**
（Swapchain）。

**理由**：Vulkan 侧极轻，2 帧足够重叠。`render_finished` 若 per-frame，在 image 数 > frames in flight 时可能对仍被 present
占用的 binary semaphore 再次 signal（违反 spec）；per-image 下同一 index 在 present 完成前不会被再次 acquire。

### Renderer 内拆分

**决策**：Renderer 暴露 `submit_cuda` 与 `record_vulkan`；Application 编排先 CUDA 后 acquire，并保留 submit/present 骨架。

**理由**：渲染内容归 renderer，但 CUDA 提交相对 acquire 的时机属驱动时序，须由 app 掌握才能吃到重叠。

---

## Path Tracer 内核

### Kernel 架构

**决策**：Megakernel + SER。演进：需要 ReSTIR 等全局同步时迁 Multi-launch；体积渲染再评估 Wavefront。代码上以 `PathState`
、payload pack/unpack helpers、bounce loop body 隔离为 device 函数做零开销预留——Megakernel 下 PathState 为局部变量，迁
Multi-launch 时改为全局 buffer 元素即可。

**理由**：SER 消除 bounce 间 divergence，零中间 ray state buffer。Multi-launch / Wavefront 的全局 buffer 与实现复杂度在当前无
ReSTIR 时收益不足。

### Payload

**决策**：全信息 payload registers，不走 global hit buffer。当前 15 槽（typed payload，per-register 读写语义声明；bounce/sample_s/primary_aux 打包在 p14）；Ray Cone
LOD 使用时扩至 17（p15/p16 = cone_width / cone_spread）。Shadow ray 零 payload（`optixTraverse` +
`optixHitObjectIsHit()`），编译器跨 shadow trace 不保存/恢复 bounce 寄存器。

**理由**：SER 重排后 payload 随线程移动、零全局内存；global buffer 在重排后访问变为 non-coalesced，反而更差。OptiX 上限 32
registers。Typed payload 让编译器区分 bounce（16 reg）和 shadow（0 reg），消除跨 shadow trace 的保守寄存器保存。

### Device 代码组织

**决策**：单 `programs.cu` + 多 `.cuh`（`__forceinline__ __device__`），不做多 Module linking。

**理由**：全局内联对 megakernel 性能关键；多 module 跨模块调用受限且构建复杂；单 module 千行级编译不构成瓶颈。

### 每帧多 Sample

**决策**：raygen 内 sample 循环（单次 `optixLaunch`，寄存器局部累积）。UI 将 `samples_per_frame` 限制在 1..64（TDR 安全）；引擎路径不另做
clamp。

**理由**：相对每 sample 一次 launch 或 3D launch，带宽与 launch 开销最优；寄存器仅多约 3 floats + counter。上限由 UI
约定，非独立硬编码常量。

**潜在优化点（多 spp primary 复用）**：DLSS ON 时帧内所有 sample 共享 primary ray 方向，理论上可缓存 primary hit 数据跳过后续
sample 的遍历。缓存方式有三种权衡维度，需 profiling 数据判断净收益：

- **全局内存 G-buffer**（缓存着色结果）：跳过遍历 + closesthit，但写入量（70+ MB）流过 L2，可能驱逐 BVH/纹理 page
- **寄存器缓存 hit 数据**：跳过遍历，closesthit 仍执行，但 hit 数据跨 sample 循环 live 增加每 bounce continuation 开销
- **local memory 缓存**（28 字节/像素，L1 栈）+ `optixMakeHitObject`：零 per-bounce 寄存器开销，跳过遍历，closesthit 仍执行

Step 15 adaptive spp 落地后，若 profiling 显示 primary 遍历是瓶颈，可从此处重新评估。

### SBT / Pipeline

**决策**：1 miss（`__miss__env`）+ **1 hitgroup**（CH+AH）；单 hitgroup record（`hitgroupRecordCount = 1`，无 per-material SBT
索引）；BLAS per-geometry flag 控制 anyhit（opaque: `DISABLE_ANYHIT`，non-opaque: `REQUIRE_SINGLE_ANYHIT_CALL`）。Shadow ray
用零 payload `optixTraverse` + `optixHitObjectIsHit()` 判遮挡，不调 `optixInvoke`，不需要 miss 程序。

**理由**：否定 2 hitgroup + multi-record SBT 方案——CH/AH 独立入口与寄存器分配，opaque 不链 AH 无寄存器收益；SER hint 已用
material 信息；BLAS geometry flag 是 OptiX 控制 anyhit 的原生机制。Shadow ray 只需 hit/miss 判定，`optixHitObjectIsHit()`
直接回答， 无需通过 payload 寄存器或 miss 程序传递可见性标志。

---

## 着色与光照

### 多散射能量补偿

**决策**：Turquin 对 specular 乘增益；`E_ss` 用 Sforza 19 系数有理多项式（零 LUT）。Diffuse 权重为
`(1 - metallic) × (1 - E_glossy)`，`E_glossy` 为 Sforza 39 系数多项式。Specular 微面元原语收 **alpha**（= roughness²），
`E_ss` 收 **linear roughness**；closesthit 单点算 alpha 并 clamp，原语内不重复。Visibility 命名 `V_SmithGGXCorrelated`。

**理由**：Heitz 随机游走过慢；Kulla-Conty 需改采样；Turquin 不改采样，单向 PT 不要求互易。多项式精度足够且无预计算资源。
`(1-F)` 忽略微面元多次弹跳后进入 diffuse 的能量，高 roughness + 掠射偏差大。alpha 与 linear r 来自不同数学源头（Heitz vs
多项式拟合），无法统一；相对 Himalaya「全收 roughness、函数内平方」，集中算 alpha 避免重复平方并让 NaN 防护可见。命名标明
height-correlated G2，避免与 separable 混淆。

### Diffuse 模型

**决策**：EON（Energy-Preserving Oren–Nayar）+ CLTC 采样，不用 Lambertian / Burley。EON / E_FON / CLTC 收 **linear
roughness**（与 specular 的 alpha 参数域不同，见上节）。

**理由**：能量守恒且保持、互易、粗糙回射、OpenPBR 采纳；CLTC 在掠射角相对 cosine 半球大幅降方差。GPU PT 上相对 Lambertian
开销可忽略。

### Fresnel

**决策**：仅标准 `F_Schlick(VdotH, F0)`（F90=1 隐含）。不做 F82-tint；不暴露 F90 参数重载。

**理由**：F82-tint 依赖未解析的 KHR_materials_specular；涂层与 F90≠1 场景不在范围内；金属能量补偿走独立 Turquin 增益。

### 光源范围

**决策**：仅 IBL + emissive 三角形 NEE + MIS（power heuristic）。不实现 glTF punctual lights。Env alias table 降采样至目标上限
**1024×512**：自动计算最小 power-of-2 因子使两个维度都不超过目标（8K→÷8, 4K→÷4, 2K→÷2, ≤1K×512→不降采样）。Cubemap 保持全分辨率。

**理由**：

- 解析光源与 IBL/emissive 重复覆盖照明
- Alias table 随机访问模式下 L2 利用率极低，与 BVH 遍历、cubemap 纹理缓存等共享 L2 预算；全分辨率（4K×2K ≈ 96 MB）独占大量
  L2，降采样至 ~6 MB 后为其他消费者释放空间
- 1024×512 确保 8K 输入下 0.5°（~11 px）特征覆盖 ≥1 alias block（block = 8 px），太阳级亮源采样效率不受影响
- 夜景小亮源由 emissive NEE 负责，env 侧低分辨率影响极小
- PDF 按 block 粒度构建与求值，自洽无偏；实际 radiance 从全分辨率 cubemap 采样
- Power-of-2 因子对非 power-of-2 输入维度产生非最优降采样比（如 3072×1536 → ÷4 → 768×384 而非理想的 1024×512），此为已知
  限制，可接受；后续有需求时可改为任意整数因子

**备选（已排除）**：

- **全分辨率**：L2 共享预算下代价过大，importance 精度的边际收益不足以抵消
- **Hierarchical mipmap sampling**：存储最优且天然适配任意分辨率，但需重构采样与 PDF 路径，留待 M2 更复杂光源策略时评估

### 环境贴图表示

**决策**：equirect 加载后转 cubemap，`texCubemap` 采样。

**理由**：硬件选面、无极地畸变、边界 filtering 优于 equirect 直采；代价为一次性转换与约 1.5× 显存。

---

## 几何与透明度

### Stochastic Alpha 采样

**决策**：`alpha_mode == Blend` 时用 **hash**（pixel / sample / primitive）与 texel alpha 比较决定
`optixIgnoreIntersection`，不用 Sobol 维度。

**理由**：anyhit 调用次数与顺序不确定，Sobol 维度分配模式不适用。

---

## 采样与 RNG

### RNG

**决策**：Sobol（128 维 direction numbers）+ per-pixel **加法** Cranley-Patterson rotation， **不加** per-frame temporal
offset；dim ≥ 128 降级 xxhash32。 **不用** blue noise 纹理。表以 `uint32_t sobol_directions[4096]` **内嵌 LaunchParams**，经
`optixLaunch` 进入 `__constant__`。

**理由**：DLSS-RR 文档将 blue noise 列为应避免；加法 CP 保持低差异性（XOR 破坏分层）。OptiX 无公开 API 初始化非 LaunchParams
的 `__constant__`，内嵌是唯一把自定义表放进 constant memory 的方式。

**Temporal offset 已移除（A/B 验证）**：golden-ratio 逐帧漂移（`frame_index * 2654435769u`）破坏 Sobol 指数和的精确零因子——K≠0
Fourier 分量的阻尼从 O (1/n) 退化到 ~exp (c·√log n)/n，实用范围内收敛率从 ~n⁻¹ 打回 ~n⁻⁰·⁵（n=65536 时 RMSE 最大差
589×）。帧间去相关由 `sequence_index = frame_index * spp + s` 的前进保证，偏移无残余职责。K=0 反对角型病态投影（本表实测最卡模式）偏移严格无作用（比值恒
1.00）。

### 多 spp 与 jitter

**决策**：DLSS ON 时帧内所有 sample **共享**同一 subpixel jitter（仅跨帧变化）；DLSS OFF 时 per-sample jitter。BRDF/NEE 维度始终
per-sample。

**理由**：DLSS 每帧只收一组 aux 与一个 `InJitterOffset`；共享 jitter 使 primary 一致、aux 写一次。OFF 时 per-sample jitter
加速 MC 收敛。

### Path sequence index

**决策**：Sobol / xxhash 的 path sequence index 为 `frame_index * samples_per_frame + s`（两模式统一）。与 Separate Sum 的
`sample_count`（per-slot 链长；DLSS ON 每帧为 0）解耦。首 sample 写 aux 的门控改为
`sample_s == 0`（payload 打包的帧内样本序号），不再与 `sample_count` 比较。

**理由**：DLSS 清零 `sample_count` 后若仍作 sequence index，每帧 path 维序列重复，跨维相关噪声被时域历史加深。
`frame_index` 永不 reset，与 host 全局 jitter 同源，保证跨帧序号前进。

### Firefly Clamping

**决策**：对每条 path 完成的 sample radiance 做 **亮度等比缩放**（非 per-bounce、非按通道 min）。
`lum = (r+g+b)/3`；若 `max_clamp > 0` 且 `lum > max_clamp`，则 `radiance *= max_clamp / lum`。默认
`max_clamp = 10`（对齐 vk_gltf_renderer 静态默认）；`0` 关闭。阈值变更是 content-defining，触发累积 / DLSS history reset。UI
范围 0..100（上界为调节便利，不构成与对照项目的对齐约束）。

**理由**：极亮低概率路径主导方差，在实时 / 低 spp 与 DLSS-RR 时域历史下形成稳定 firefly。亮度缩放保色相；默认 10 与
vk_gltf_renderer 静态默认一致。有偏，故允许关闭以做无偏对照。与「数值正确性策略」不冲突——后者禁止把最终 clamp 当作数值 bug
兜底；本项是显式方差控制旋钮。

**相对早期决策**：Phase 4 D21 曾排除本项（偏置 + 降噪可处理）。审查中可见 firefly 后纳入，默认开启。

---

## 滤波与抗锯齿

### Normal Map Specular AA

**决策**：用 ray cone footprint 估计法线贴图像素内方差，叠加到 `roughness²`（Kaplanyan 2016）。 **不做** LEAN / LEADR。

**理由**：接缝处 specular 闪烁来自法线在像素 footprint 内的变化，方差→roughness 与现有 cone LOD 数据同源、增量小。LEAN/LEADR
需额外预处理与存储，复杂度与收益不符。

---

## 降噪、分辨率与帧率

### DLSS-RR

**决策**：DLSS Ray Reconstruction（CUDA API）替代 OptiX Denoiser。管线：raygen → DLSS-RR → tonemap。Separate Sum 为 DLSS OFF
的长期 fallback。

**理由**：DLSS-RR 同时做时域累积、去噪、放大，直接到达实时 PT 终态；OptiX Denoiser 只做去噪。

### DLSS guide 关键约定

**决策**：

- specular albedo = 逐通道 `E_glossy`（与 Turquin 补偿后 specular 能量自洽）
- specular hit distance 传 **nullptr**（不分配）
- diffuse albedo 暂传 raw `base_color`（是否预乘 `1-metallic` 存疑；出现相关伪影时优先排查）

**理由**：EnvBRDFApprox2 与项目多散射 specular 不一致时，guide 与 color 自洽优先于匹配训练分布。nullptr 比填 infinity
更诚实；SDK 在无 hit distance 时回退 2D MV 推导 specular motion。

### 渲染 / 显示分辨率

**决策**：渲染分辨率独立于 swapchain；display buffer 始终 swapchain 大小。缩放在 tonemap kernel 内、 **线性 HDR（mean）**
上完成，再 exposure + tonemap 写全幅 display；VK blit 保持 1:1。

**理由**：blit 滤波发生在 tonemap 后的 LDR，无法对缩小做 footprint 滤波，也无法在线性空间高质量放大。DLSS ON 时输出已是显示分辨率；OFF
时本路径为长期 fallback。

### 自适应帧率（策略）

**决策**：三级降级，通过调节 `samples_per_frame` 逼近目标帧率；与呈现模式（MAILBOX/FIFO）解耦。

| Mode | 架构           | 条件          | 目标帧率                                |
|------|----------------|---------------|-----------------------------------------|
| 1    | ping-pong 并行 | 1spp 足够快   | min { n×refresh ≥ 150 }                 |
| 2    | 串行           | 无法达 Mode 1 | refresh（<150Hz）或 refresh/2（≥150Hz） |
| 3    | 串行           | 无法达 Mode 2 | 反复减半；低于 60fps 则 1spp 放开跑     |

**理由**：ping-pong 多一帧显示延迟，需 ≥150fps 才不可感知；达不到则串行换更低目标以消除额外延迟。整数倍刷新率避免微卡顿。

---

## 呈现与色调

### Tonemap 与 Exposure

**决策**：Khronos PBR Neutral。`RenderSettings` 存 EV；`Renderer::submit_cuda` 转为线性倍率 `pow(2, ev)` 再传入
tonemap；kernel 只做 `color * exposure`。

**理由**：PBR Neutral 在峰值下 1:1 还原 baseColor、仅压高光。EV 作 UI 单位；转换集中在 submit 边界，device 侧只收线性倍率。

---

## UI 与层边界

### ImGuiBackend 归属

**决策**：实现在 vulkan 层；所有权在 Application；Renderer 经 `RenderInput` 传入，不缓存。UI 面板逻辑（debug_ui）在 renderer。

**理由**：后端是呈现基础设施；生命周期绑窗口/输入故归 app；符合使用方不缓存所有者句柄。

### UI 领域结构

**决策**：`RenderSettings`（可调旋钮）+ `SceneStats`（只读资产快照），DebugUIContext 组合引用；参数变化由 Renderer 比对前帧值检测。

**理由**：避免 Context 散装标量膨胀。near/far 不暴露——perspective unproject 后 near/far 缩放在 normalize 中消失，PT 无深度缓冲消费
depth mapping。

---

## 资产管线

### BC6H 压缩方式

**决策**：ISPCTextureCompressor CPU ISPC 压缩，不使用 GPU compute shader。

**理由**：

- Qualquer 无 Vulkan compute 基础设施（无 compute pipeline、push descriptor、ShaderCompiler），为 BC6H 单一功能引入整套违背
  KISS
- Himalaya 的 GPU BC6H 压缩（`texture_compress.cpp`）需 ~200 行 Vulkan 管线编排（pipeline 创建、descriptor layout、staging
  buffer、per-face × per-mip 循环、4 层 barrier），复杂度与收益不成比例
- GPU BC6H encoder 受 shared memory / register 限制，只尝试少量模式；ISPCTextureCompressor 的 `veryslow` profile 可做更多
  refine iteration，质量更优
- IBL 纹理是离线预处理（一次压缩、永久缓存），CPU 耗时完全可接受
- 项目已有 ISPC 语言支持（bc7enc），ISPCTextureCompressor 集成为同类操作——复制 `kernel.ispc` + wrapper，无新构建依赖

**备选（已排除）**：

- **GPU compute shader（Himalaya 方式）**：需引入完整 Vulkan compute 管线 + `bc6h.comp` shader，复杂度远超收益
- **其他 CPU BC6H 编码器**（如 DirectXTex）：ISPCTextureCompressor 是 Intel 官方 ISPC 实现，与项目已有 ISPC 工具链一致，且有多档质量
  profile

### 异步上传生命周期

**决策**：host→device 异步 copy 按 host source **所有权批次**同步；同步点在任一局部 source 失效之前。不用
`cudaDeviceSynchronize`。

**理由**：局部加载缓冲若在 stream copy 完成前析构，违反 upload 契约。批内共享一次同步保留并行；加载路径不值得上持久
staging + event 回收。

---

## 窗口与 Swapchain

### 命令录制

**决策**：裸 `VkCommandBuffer`，无 CommandBuffer wrapper。

**理由**：无 Render Graph/多 Pass 边界；wrapper 仍透传 `Vk*` 类型且 ImGui 要原始 handle，无收益。

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

- 最小化检测 **必须轮询**尺寸（`while fb==0 glfwWaitEvents`），这条路径每帧已查询 framebuffer size
- resize 检测顺手在同一次查询里比对即可，无需引入额外机制
- 回调+标志与轮询是同一信息源的两种实现，二选一；最小化已逼出轮询，回调就被吸收

### 窗口最小化处理

**决策**：整循环阻塞（poll events 后、begin_frame 前，`while fb==0 glfwWaitEvents` 阻塞）。最小化期间完全不进帧循环。

**理由**：

- `glfwWaitEvents` 是可中断睡眠，任何事件（含恢复窗口）都唤醒，不卡死
- 最小化时 GPU 不空转渲染、CPU 不忙循环轮询，省资源
- Qualquer 无"最小化时仍需每帧执行的非渲染逻辑"，非阻塞跳帧（轮询但跳过渲染）的灵活性用不上

---

## 工程策略

### 数值正确性策略

**决策**：按责任域在源头修正并验证（资产输入 → geometry/shading frame → BRDF/能量 → importance distribution → 直接光/path
estimator → 时域/guide data）。全部源头完成前，不以最终 radiance clamp / 非有限清零作为统一兜底。

**理由**：边界 sanitize 会掩盖真实错误，使单项修正无法验证。边界防御不承担修正物理模型的职责。

### 明确不纳入

| 项                              | 理由                                           |
|---------------------------------|------------------------------------------------|
| `indirect_intensity` 乘数       | 非物理艺术控制                                 |
| Target sample count / auto-stop | DLSS-RR 内部管理时域历史，无有意义的收敛停止点 |
