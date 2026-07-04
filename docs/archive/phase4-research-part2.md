# Phase 4 研究 — Part 2：PT 架构选项清单

> 临时讨论文档。列出 PT 渲染器总体架构中的关键决策点和潜在选项。
> 不做最终选择，仅罗列供讨论。
> 技术栈：CUDA/OptiX（非 Vulkan RT）。目标：从 M1 基础 PT 平滑演进到 M2 现代游戏 PT。

---

## 1. Kernel 架构：Megakernel vs Wavefront

### 选项 A：Megakernel（Himalaya 的做法）

- 单次 `optixLaunch`，所有 bounce 在 raygen 的 loop 内完成
- closesthit 做全部材质着色 + NEE + BRDF 采样
- 优势：简单直接；无中间 buffer 开销；单次 launch 减少 kernel launch overhead
- 劣势：寄存器压力大（closesthit 存储所有 BRDF/NEE 状态）；warp divergence（不同路径长度的线程等待最长路径完成）；不利于未来 ReSTIR 等需要分离 pass 的技术

### 选项 B：Multi-launch 迭代（每 bounce 一次 launch）

- 每帧对每个 bounce depth 做一次 `optixLaunch`
- 中间 ray state 存储在全局 buffer 中
- 优势：每次 launch 的 divergence 更小；可以在 bounce 间插入额外逻辑（如 ReSTIR resample）
- 劣势：多次 launch overhead；需要全局 ray state buffer（带宽消耗）；ray compaction 逻辑更复杂

### 选项 C：Wavefront（CUDA kernel 做着色，OptiX 仅做 traversal）

- OptiX 用于 ray-TLAS intersection（closesthit 只写 hit data），CUDA kernel 做材质着色
- 完全分离 traversal 和 shading
- 优势：最大 occupancy；可精确控制 warp divergence（材质分类 sorting）；M2 ReSTIR 的自然基础
- 劣势：实现复杂度最高；需要 hit buffer + ray buffer + shadow ray buffer

### 选项 D：Hybrid — Megakernel 起步，为 Wavefront 预留空间

- Phase 4 用 Megakernel（OptiX raygen loop），但 ray state 和 launch params 的设计为未来拆分预留接口
- M2 时重构为 wavefront 或 multi-launch
- 优势：Phase 4 开发速度快；不过早优化
- 劣势：重构时可能有数据布局不兼容

### 讨论点

- **OptiX 的 payload 限制**：OptiX 最多 32 个 32-bit payload registers。Himalaya 的 `PrimaryPayload` 是 72 bytes（18 个 registers）— 已经很紧
- **M2 的 ReSTIR 需求**：ReSTIR DI 需要在 primary hit 后做 reservoir 操作，然后再 trace shadow ray。天然倾向 multi-launch 或 wavefront
- **SER (Shader Execution Reordering)**：OptiX 9.x 特性，对 megakernel + large payload 有优化（重排 hit shader 执行以提高 cache locality）
- **每帧多 Sample 与 kernel 架构的交互**：Megakernel 下 N samples 是 raygen 内的外层循环，每个 thread 生命周期 ∝ N × max_bounces。这实际上**帮助** occupancy — 减少 idle threads（一个 sample 提前终止，thread 继续下一个 sample）
- **OptiX maxTraceDepth 与 megakernel 的关系**：megakernel 的迭代式 bounce loop 中，raygen 每次 `optixTrace` 都是独立调用（不是递归），所以 bounce 数不影响 trace depth。但 closesthit 内的 shadow ray 是递归 trace（depth=1），所以 `maxTraceDepth=1` 已足够。当前 pipeline 已设为 1，Phase 4 不需要修改

---

## 2. 每帧多 Sample 的执行策略

> Himalaya 没有的新需求。这是一个独立于 §1 的决策，因为即使选了 Megakernel，仍然有多种执行 N samples 的方式。

### 选项 A：Raygen 内 sample 循环

```
// raygen:
float3 frame_total = {0, 0, 0};
for (int s = 0; s < samples_this_frame; s++) {
    frame_total += trace_path(..., total_sample_count + s, ...);
}
float3 frame_avg = frame_total / samples_this_frame;
// merge with old accumulation (read-modify-write via ping-pong)
```

- 每帧 1 次 `optixLaunch`
- 优势：launch overhead 只付一次；局部累积在寄存器中（无全局内存 read-modify-write per sample）；warp 内无 divergence（所有 thread 执行相同 N）
- 劣势：GPU 线程生命周期长（N × bounces traces per thread）

### 选项 B：多次 `optixLaunch`（每 sample 一次 launch）

- 每帧 N 次 `optixLaunch`
- 优势：每次 launch 的行为与 Himalaya 单 spp 相同，最小改动
- 劣势：launch overhead × N（`optixLaunch` 有非平凡开销：SBT 绑定、pipeline 状态设置）；每次 launch 都做全局 buffer read-modify-write

### 选项 C：3D launch（width × height × N）

- `optixLaunch(width, height, N)` — 第三维度是 sample index
- 优势：所有 samples 并行执行
- 劣势：所有 samples 同时写同一 pixel 的 accumulation buffer → 需要原子操作或 per-sample 临时 buffer → 复杂且开销大

### 讨论点

- **选项 A vs B 的关键差异**：A 对 accumulation buffer 做 1 次 read + 1 次 write/pixel/frame；B 做 N 次 read-modify-write/pixel/frame。A 的带宽优势随 N 增大越来越显著
- **寄存器压力**：选项 A 的 raygen 需要额外 3 floats（`frame_total`）+ 1 loop counter。这很少，不会影响 occupancy
- **TDR 安全**：Windows TDR 默认 2s（TdrDelay 注册表值）。实际瓶颈是总 trace 次数 / GPU throughput：2M pixels × N samples × avg_bounces traces total。经验值：RTX 4070 约 1-2 Gtrace/s。N=100, avg_bounces=8 → 1.6G traces → ~1-2s，接近 TDR。建议安全上限 max_samples_per_frame=64

---

## 3. 自适应 Sample 数的反馈架构

> Himalaya 没有的新需求。

### 3.1 帧时间测量方式

#### 选项 A：CUDA timing events

- 在 `optixLaunch` 前后插入 timing events（`cudaEventCreate` 不带 `DisableTiming`）
- 下一帧查询上一帧的 elapsed time：`cudaEventElapsedTime(&ms, start[prev], end[prev])`
- 优势：精确测量 GPU 端 PT 计算时间；不包含 CPU 开销和 VK present 延迟
- 劣势：`cudaEventElapsedTime` 阻塞直到两个 event 都完成 — 对上一帧的 event 这应该是立即返回（fence wait 已保证 GPU 完成）；多 2 个 timing events

#### 选项 B：CPU wall clock

- `std::chrono::steady_clock` 测量帧间时间
- 优势：简单；反映用户实际感受的帧率
- 劣势：包含 CPU 开销、VK present、可能的 vsync 等待；不准确反映 PT 本身的 cost

#### 选项 C：混合 — GPU timing 决定 PT 预算，CPU wall clock 做安全限制

- GPU timing：调整 samples_per_frame 以使 PT 耗时适应帧预算
- CPU timing：如果总帧时间超标（即使 PT 本身没超），也减少 N
- 优势：两层保障
- 劣势：逻辑稍复杂

### 讨论点

- **现有 events 是 `cudaEventDisableTiming`**：需要添加新的 timing events，或改现有 events（改为 timing-enabled 会增加少量 per-event overhead）
- **反馈延迟**：timing event 查询至少有 1-2 帧延迟（record → GPU 执行 → fence wait → query）。这对自适应控制可接受（场景不会帧间剧变）
- **tonemap 时间是否计入**：tonemap 在 1080p 下 ~0.1ms，可忽略。PT 占绝对主导

### 3.2 自适应控制策略

#### 选项 A：比例控制

```
N_new = N_old × (target_ms / actual_ms)
N_new = clamp(N_new, 1, max_N)
```

- 优势：响应快；简单
- 劣势：可能振荡（actual_ms 噪声大时 N 跳来跳去）

#### 选项 B：EWMA 平滑 + 比例控制

```
smoothed_ms = alpha * actual_ms + (1 - alpha) * smoothed_ms
N_new = N_old × (target_ms / smoothed_ms)
```

- 优势：平滑；减少振荡
- 劣势：响应略慢（alpha 控制）

#### 选项 C：带迟滞的比例控制

- 只有 actual_ms 偏离 target_ms 超过 ±10% 时才调整
- 优势：稳态不抖动
- 劣势：在阈值边界可能有突变

#### 选项 D：增量控制（逐 1 调整）

```
if (actual_ms > target_ms) N -= 1;
if (actual_ms < target_ms * 0.9) N += 1;
```

- 优势：最不可能过冲
- 劣势：从低 N 到高 N 收敛很慢

### 讨论点

- **场景切换/相机移动时的行为**：累积重置时帧时间可能突变（新视角可能更重或更轻）。控制器应能快速适应
- **max_samples_per_frame 上限**：TDR 安全 + 内存预算约束。建议硬上限 128 或 256
- **最低 1 spp/frame**：无论如何不能低于 1
- **累积重置时的策略**：reset 后第一帧可能用旧的 N（基于旧场景的 timing） → 一帧的过冲/欠冲可接受

---

## 4. 渲染分辨率与呈现分辨率解耦

> Himalaya 没有的新需求。当前所有 buffer 紧跟 swapchain extent。

### 4.1 解耦范围（哪些资源需要分离？）

| 资源 | 当前 | 解耦后 |
|------|------|--------|
| Accumulation buffers (ping-pong) | swapchain extent | **渲染分辨率** |
| Display buffer (InteropImage) | swapchain extent | 可选（见 §4.2） |
| Tonemap kernel launch grid | swapchain extent | **渲染分辨率** |
| Camera aspect ratio | swapchain extent | **渲染分辨率**（同 aspect ratio 则等价） |
| optixLaunch dimensions | swapchain extent | **渲染分辨率** |
| VK blit src extent | swapchain extent | **渲染分辨率** |
| VK blit dst extent | swapchain extent | **swapchain extent**（不变） |

### 4.2 Display Buffer 尺寸策略

#### 选项 A：Display buffer 保持 swapchain 大小（最大尺寸预分配）

- 渲染分辨率 ≤ swapchain → tonemap 写入 display surface 的左上角子区域 → VK blit 用 srcExtent={render_w, render_h} 做 LINEAR 上采样
- 优势：InteropImage 只在 swapchain resize 时重建（不因渲染分辨率变化而重建）；避免频繁 interop object 重建
- 劣势：display buffer 内存按最大尺寸分配（1080p → 8MB，4K → 32MB，可接受）
- **关键优势**：如果未来做自适应渲染分辨率（每帧变），display buffer 不需要任何变化

#### 选项 B：Display buffer 跟随渲染分辨率

- 渲染分辨率变 → 重建 InteropImage + CUDA 重新 import
- 优势：精确的内存使用
- 劣势：InteropImage 重建涉及 VK image + VK memory + NT handle export + CUDA import — 成本较高（Phase 4 UI 触发可接受，未来自适应不可接受）

### 讨论点

- **选项 A 的 surface 写入**：`surf2Dwrite` 写入 larger image 的 sub-rectangle 是合法 CUDA 操作。未写入区域的像素内容无关紧要（VK blit 只读 srcExtent 区域）
- **Aspect ratio 约束**：渲染分辨率必须与 swapchain 保持相同 aspect ratio（否则画面变形）。实现为 `render_scale ∈ (0.0, 1.0]` 统一缩放宽高
- **分辨率变化触发累积重置**：渲染分辨率改变 → 旧累积无效 → 必须 reset sample_count + clear accumulation buffers
- **Accum buffer resize 成本**：纯 CUDA device memory (cudaMalloc/cudaFree) — 无 interop 开销，远比 InteropImage 重建廉价

### 4.3 现有代码的紧耦合点（需要改造）

1. `Application::init()` — `renderer_.init(... swapchain_.extent.width, swapchain_.extent.height ...)` → 需分离
2. `Application::resize()` — `renderer_.resize(... swapchain_.extent ...)` → 需区分 swapchain resize vs render resize
3. `Application::render_frame()` — `submit_cuda(..., swapchain_.extent.width, swapchain_.extent.height, ...)` → 传渲染分辨率
4. `Camera::aspect` — `swapchain width / height` → 改为 `render width / height`
5. `display_buffer_.init(... swapchain_.extent ...)` — 保持（选项 A 下）
6. `cuda_context_.import_display_buffer(... swapchain_.extent ...)` — 保持（选项 A 下）
7. `launch_tonemap(... width, height ...)` — 传渲染分辨率
8. `record_vulkan()` 中的 `VkCmdBlitImage` — srcExtent 用渲染分辨率，dstExtent 用 swapchain extent

---

## 5. Accumulation 策略

### 选项 A：Running Average via Ping-pong（推荐）

- raygen 读 buf[accum_index_]（旧累积值），写 buf[1-accum_index_]（新均值）
- 公式：`new = mix(old, frame_avg, N / (total + N))`
- Tonemap 读 buf[accum_index_]（与 raygen 并行读，无冲突）
- 帧末翻转 `accum_index_`

### 选项 B：Separate Accumulator + Counter

- `accumulator[pixel] += frame_total`; `count[pixel] += N`
- Display: `accumulator / count`
- 优势：精度更好（分离求和+除法在最后执行）
- 劣势：多一个 buffer（或把 count 编码进 alpha channel）

### 讨论点

- **已验证兼容性**：当前 ping-pong + 双 stream 架构与 running average 自然兼容（Part 1 备注 7）
- **Reset 语义**：`sample_count = 0` 时 raygen 不读旧值，直接写 frame_avg → 旧 buffer 内容无关紧要（无需 clear 两个 buffer，但 resize 时仍需 clear 以防 NaN/Inf）
- **LaunchParams 变化**：需要添加 `prev_accumulation`（读指针）、`sample_count`、`samples_this_frame`

### 5.1 首帧覆写模式（sample_count == 0 时的分支）

```cuda
if (sample_count == 0) {
    // 直接写 frame_avg，不读旧值
    accum_buffer_write[pixel] = make_float4(frame_avg.x, frame_avg.y, frame_avg.z, 1.0f);
} else {
    float4 old = accum_buffer_read[pixel];
    float weight = float(N) / float(sample_count + N);
    float3 result = mix(old.xyz, frame_avg, weight);
    accum_buffer_write[pixel] = make_float4(result.x, result.y, result.z, 1.0f);
}
```

省去 reset 时的 buffer clear（一次 memset），因为直接覆写。但 resize 仍需 clear 以防 NaN/Inf 残留。

### 5.2 累积重置触发条件与检测

当前项目完全没有累积重置机制。Phase 4 必须添加。

**需要检测的变化**：
- Camera：position, yaw, pitch, fov, aspect 任一变化 → reset
- PT 参数：max_bounces, max_clamp 等变化 → reset
- 渲染分辨率变化 → reset + resize accum buffers
- 场景切换 → reset（load_scene 内部）

**检测方式**：
- 在 `submit_cuda` 开头存储 prev_inv_view + prev_inv_projection，逐帧比较
- Himalaya 直接比较 view_projection 矩阵（exact float compare）。因 Camera 只在输入时更新矩阵，静止时矩阵不变，exact compare 有效
- 我们的 Camera::update_view/update_projection 同理——只在输入时调用
- PT 参数变化同理：比较 prev 值

**Reset 动作**：
- `sample_count_ = 0`
- `frame_seed_` 不 reset（保证蓝噪声 temporal scramble 不重复）

### 5.3 LaunchParams 扩展（具体影响）

当前：
```cpp
struct LaunchParams {
    float4 *accumulation_buffer;  // write target
    uint32_t width, height, frame_index;
    OptixTraversableHandle traversable;
    const GPUGeometryInfo *geometry_infos;
    const Material *materials;
    const cudaTextureObject_t *texture_objects;
    float4x4 inv_view, inv_projection;
};
```

需增加（最小集）：
```cpp
const float4 *prev_accumulation;   // read source (old accumulated value)
uint32_t sample_count;             // total samples accumulated so far
uint32_t samples_this_frame;       // N for this frame's inner loop
```

可能还需要（取决于其他决策）：
```cpp
// RNG 相关
const uint32_t *sobol_directions;  // or embed in __constant__
cudaTextureObject_t blue_noise_tex;

// PT 参数
uint32_t max_bounces;
float max_clamp;                   // firefly clamping threshold
uint32_t frame_seed;               // temporal scramble seed (不随累积 reset)
// ... env sampling, emissive count 等
```

### 5.4 frame_counter_ / sample_count / frame_seed 语义分离

当前 `frame_counter_` 承担两个角色（slot 索引 + frame_index）。Phase 4 需拆分为三个独立语义：

| 字段 | 用途 | Reset 行为 |
|------|------|-----------|
| `frame_counter_` | 双 stream 的 slot 索引（%2） | 永不 reset |
| `sample_count_` | 总累积 sample 数（= Σ samples_this_frame） | camera/config 变化时 reset = 0 |
| `frame_seed_` | RNG 时间变异种子（Sobol temporal scramble） | 永不 reset（保证蓝噪声不重复） |

Himalaya 的对应关系：
- `sample_count` = 累积计数（reset 时归零）
- `frame_seed` = 独立递增计数（不 reset）
- Himalaya 没有 slot 索引（单 stream）

---

## 6. Payload 设计

### 选项 A：全信息 Payload（Himalaya 的做法移植到 OptiX payload registers）

Himalaya 的 PrimaryPayload 移植为 OptiX registers（18 × uint32）：

```
color (3) + next_origin (3) + next_direction (3) + throughput_update (3) +
hit_distance (1) + bounce (1) + env_mis_weight (1) + last_brdf_pdf (1) +
cone_width (1) + cone_spread (1) = 18 registers
```

- OptiX 上限 32 registers，用了 18 = 56%
- 优势：无全局内存访问；硬件优化路径
- 劣势：编码/解码每个 register 需要 `__float_as_uint / __uint_as_float`

### 选项 B：Minimal Payload + Global Buffer

```
struct { uint pixel_index; }  // 1 register
```

- closesthit 读写全局 buffer[pixel_index]
- 优势：容量无限
- 劣势：全局内存延迟；cache miss

### 选项 C：Hybrid（热数据 payload，冷数据 global）

- Payload 放核心路径数据（origin, direction, throughput, hit_distance） = 10 registers
- Global buffer 放冷数据（cone, MIS weights, aux output）
- 优势：payload 更小 = 更高 occupancy；冷数据不走 register
- 劣势：两种存储路径

### 讨论点

- **SER 优化**：OptiX SER 对 payload-based 模式有优化。全走 global buffer 可能削弱 SER 收益
- **18 registers vs 32 max**：还有 14 个 register 余量。如果需要 aux buffer 输出（albedo/normal for denoiser），可以 bounce==0 时走 global buffer 写 aux，不占 payload
- **OptiX payload 的编码开销**：`optixSetPayload_N(__float_as_uint(value))` + `__uint_as_float(optixGetPayload_N())` — 编译器会优化掉 reinterpret，零开销
- **ShadowPayload**：shadow ray 只需 1 register（visible flag）。OptiX `optixTrace` 支持不同 ray type 使用不同 payload 数量
- **OptiX 9.1 的 payload 语义**：payload registers 在 `optixTrace` 返回后有效。closesthit/miss 通过 `optixSetPayload` 写入，raygen 通过 `optixGetPayload` 读取（或直接用变量引用）
- **编码模式示例**：当前用 3 个 register（p0-p2=RGB）。扩展到 18 个是机械性但代码量大的工作，可用 helper inline 函数封装：

```cuda
// closesthit 写入（18 registers）：
optixSetPayload_0(__float_as_uint(color.x));
optixSetPayload_1(__float_as_uint(color.y));
// ... 共 18 个
// raygen 读取（optixTrace 返回后）：
float3 color = make_float3(__uint_as_float(p0), __uint_as_float(p1), __uint_as_float(p2));

// shadow ray（只需 1 register）：
uint32_t visible = 0;
optixTrace(..., OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT | OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT,
           0, 1, 1,  // missIndex=1 → shadow_miss
           visible);
// shadow_miss: optixSetPayload_0(1);
```

- **与 Himalaya 的差异**：Himalaya 的 GLSL `rayPayloadInEXT PrimaryPayload` 自动映射整个 struct 到 payload registers。OptiX 没有这层抽象，必须手动 pack/unpack

---

## 7. Random Number Generation 架构

### 选项 A：Sobol + Blue Noise（Himalaya 的做法）

- 128 维 Sobol direction numbers（4096 × uint32 = 16 KB）
- 128×128 blue noise 纹理（16 KB, R8Unorm）
- 维度分配：dims 0-1 = subpixel jitter, per-bounce base = 2 + bounce × DIMS_PER_BOUNCE (12)
- 128 dims 够 ~10 bounces，之后降级为 PCG hash
- 优势：低差异序列 + 空间去相关 = 极快收敛；业界标准
- 劣势：需上传两份数据到 GPU

### 选项 B：PCG per-pixel

- 每像素独立 PCG state（seed = pixel_index ^ frame_index）
- 优势：最简单；无外部数据；无维度限制
- 劣势：白噪声，收敛速度远不如低差异序列

### 选项 C：Owen-scrambled Sobol via hash

- Sobol + Laine-Karras style hash scrambling（替代 blue noise rotation）
- 优势：理论最优去相关
- 劣势：per-sample 多一次 hash 计算；实现复杂度略高

### 7.1 Sobol Table 存储方式

#### 选项 A：`__constant__` memory

- 放入 OptiX 的 `__constant__` 空间（与 LaunchParams 共享 64KB 限制）
- 16 KB table + LaunchParams ~300B → 远低于 64KB 限制
- 优势：constant cache 全 warp 广播；无额外 buffer 管理
- 劣势：与 LaunchParams 争夺 constant memory 容量

#### 选项 B：LaunchParams 指针引用 global memory

```cpp
struct LaunchParams {
    const uint32_t *sobol_directions;  // device pointer to 16KB buffer
    ...
};
```

- 优势：不占 constant memory；buffer 大小无限制
- 劣势：走 global memory（但 16KB 小表有较好 L2 cache hit rate）

#### 选项 C：嵌入 LaunchParams 结构体

```cpp
struct LaunchParams {
    uint32_t sobol_directions[4096];  // 16KB inline
    ...
};
```

- 优势：通过 constant memory 访问（LaunchParams 整体在 `__constant__`）
- 劣势：LaunchParams 膨胀到 ~16KB（仍在 64KB 限制内）；每帧 upload 16KB constant data

### 7.2 Blue Noise 存储方式

#### 选项 A：CUDA Texture Object

- 创建 cudaTextureObject_t 指向 128×128 R8Unorm 数据
- 通过 `tex2D<float>(tex, x, y)` 采样
- 优势：走 texture cache；硬件归一化 [0,1]；与其他纹理访问模式一致
- 劣势：多一个 texture object

#### 选项 B：Global memory buffer

- 16KB `uint8_t` 数组，手动 `data[y * 128 + x] / 255.0f`
- 优势：无 texture object 管理
- 劣势：无 texture cache 优势（但 16KB 几乎全在 L2）

### 讨论点

- **每帧多 Sample 时的维度消耗**：N spp/frame × 12 dims/bounce × max_bounces。如 N=10, max_bounces=16 → 最大 dim = 2 + 10 × 16 × 12 = 1922。远超 128 维 Sobol → 大量 fallback 到 PCG hash。但实际上，sample_index 维度分配是按 `total_sample_count + local_s` 索引 Sobol，dim 分配是固定的 per-bounce，与 N 无关。每个 sample 使用相同的 dim 分配（0-1 jitter, 2-13 bounce 0, 14-25 bounce 1, ...），但 sample_index 不同 → 128 dims 仍然够 ~10 bounces
- **嵌入 constant vs 指针**：Sobol table 每帧不变，嵌入 constant 导致每帧 upload 16KB。指针只 upload ~8 bytes。Trade-off 是 access latency vs upload bandwidth

---

## 8. 纹理 LOD 策略

### 选项 A：Ray Cone（Himalaya 的做法）

- 跟踪 cone_width + cone_spread per ray（2 floats in payload）
- 每次 bounce propagate: `cone_width += hit_distance * cone_spread`
- Curvature update: `cone_spread += 2 * curvature * cone_width`
- LOD = log2(cone_width × sqrt(uv_area / world_area)) + 0.5 × log2(tex_w × tex_h)
- 优势：物理准确；处理反射/折射后的 defocus
- 劣势：需 2 floats payload；curvature 估计是近似

### 选项 B：固定 LOD / 无 LOD

- 所有纹理采样 LOD 0（或 bounce 0 用 LOD 0，bounce > 0 用固定 bias）
- 优势：最简单；无额外状态
- 劣势：高频纹理 aliasing；无 anti-aliasing

### 选项 C：Hit distance heuristic

- LOD ∝ hit_distance × pixel_spread（Himalaya any-hit 用的近似）
- 不跟踪 per-ray cone state
- 优势：不占 payload
- 劣势：bounce 后不准确（反射 defocus 无法捕捉）

### 讨论点

- **Payload 开销**：2 floats = 2 registers。选项 A 的 payload 从 18 → 20 registers（仍在 32 限制内，如果 Himalaya 的设计已包含 cone）—— 实际上 Himalaya 的 18 registers 已经包含 cone_width + cone_spread
- **CUDA `tex2DLod`**：支持 explicit LOD 采样，语义与 Vulkan `textureLod` 等价
- **Ray Cone 的真正价值**：在 PT 中减少 texture aliasing noise → 用更少 sample 收敛到相同质量。对自适应 spp 系统尤其有价值（每个 sample 质量更高 = 更少 sample 达到目标画面）

---

## 9. 光源表示与采样数据结构

### 选项 A：Alias Table（Himalaya）

- Vose's algorithm O(N) 构建, O(1) 采样
- Env: luminance × sin(θ) 权重，half-res equirect
- Emissive: luminance(emissive_factor) × area 权重
- 优势：O(1) 采样；实现成熟；GPU 端高效（单次 buffer 查找）
- 劣势：固定权重（场景/光源变化需重建）

### 选项 B：BVH / Light Tree

- 空间 BVH 组织光源，hierarchical sampling
- 优势：支持海量光源（百万级 emissive 三角形）
- 劣势：实现复杂；per-sample 采样不再是 O(1)

### 选项 C：Uniform + Rejection

- 等概率选一个光源
- 优势：极其简单
- 劣势：高方差

### 讨论点

- **Alias table 大小**：4K HDR equirect → half-res 2K×1K = 2M entries × 12 bytes = 24 MB。可接受。Emissive 三角形：96 bytes × N_emissive + alias 8 bytes × N_emissive，万级三角形 ~1 MB
- **重建频率**：alias table 假设光源静态。场景切换时重建（一次性 CPU 计算 + GPU upload）。IBL rotation 不需要重建（shader 内旋转方向）

---

## 10. Shadow Ray 策略

### 选项 A：Standard Shadow Ray（OptiX）

- `optixTrace` with `OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT | OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT`
- miss program 设置 visible = 1
- 优势：简单可靠；硬件加速
- 劣势：每个光源样本一条 shadow ray

### 选项 B：Shadow Ray with Any-hit（透明阴影）

- 不用 `TERMINATE_ON_FIRST_HIT`，anyhit 做 alpha test
- 正确处理半透明几何体的阴影
- 优势：物理正确
- 劣势：更多 any-hit invocations

### 讨论点

- **OptiX flags**：`OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT` 跳过 closesthit 执行。加 `OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT` 在第一次不被 anyhit 忽略的 intersection 就停
- **SBT layout**：shadow ray 需要自己的 miss program（index 1），但可以共享 hitgroup（closesthit 被 disable，只有 anyhit 可能执行）
- **Himalaya 的透明阴影处理**：实际上 Himalaya 的 shadow ray 使用了 opaque flag 的 geometry，因此 alpha mask 物体挡住光源时报告 "遮挡"。严格来说，如果需要 alpha-tested shadow，shadow ray 也需要走 anyhit
- **tmin 与 offset_ray_origin 的交互**：Himalaya 的 shadow ray 使用 tmin=0.0，自交避免完全由 `offset_ray_origin`（Wächter & Binder）保证。closesthit 中计算一次 `offset_pos`，所有后续 ray（bounce + shadow）都从这个偏移点出发。当前我们的 primary ray tmin=0.0f 从 camera origin 出发无此问题，但 Phase 4 的 bounce ray 和 shadow ray 都必须实现 offset_ray_origin
- **Emissive shadow ray 的 tMax**：`dist * (1.0 - 1e-4)` 略短于到光源的距离，避免命中目标光源三角形本身

---

## 11. OptiX Pipeline / SBT 配置

### 选项 A：单 Hit Group + 全局 DISABLE_ANYHIT（当前）

- 1 raygen + 1 closesthit + 1 (空) anyhit + 1 miss
- 所有几何体标记 `OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT`
- SBT: 1 hitgroup record
- 劣势：alpha mask 不工作

### 选项 B：双 Hit Group（Opaque + Non-opaque）

- Opaque: `OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT`
- Non-opaque: `OPTIX_GEOMETRY_FLAG_NONE` + anyhit 做 alpha test
- SBT: 2 hitgroup records, `sbtIndexOffset` 区分
- 优势：opaque 硬件跳过 anyhit（重要优化）
- 劣势：SBT 管理更复杂；每几何体需正确 sbt offset

### 选项 C：双 Hit Group + 双 Miss

- 加 shadow_miss（index 1）用于 NEE shadow ray
- 可选：shadow ray 也有自己的 hitgroup（可共享 opaque/non-opaque 的 anyhit）
- 这是完整 PT 所需的最终配置

### 讨论点

- **SBT stride**：`optixTrace` 的 `SBToffset` 和 `SBTstride` 参数控制每个 ray type 如何索引 hitgroup record。multi-hitgroup 时需要 `stride = num_ray_types`，每个 geometry 的 SBT 区域 = `[instanceSbtOffset + geomIndex * stride + rayTypeOffset]`
- **当前 BLAS 构建**：`BLASGeometry::opaque` 字段已存在，当前硬编码为 `true`（renderer.cpp line 110）。Phase 4 改为 `opaque = (materials[mesh.material_id].alpha_mode == 0)`。SceneLoader 已有每个 Mesh 的 material_id，Material 有 alpha_mode — 数据链完整
- **Stride=1 是最小改动路径**：如果所有几何体 DISABLE_ANYHIT（当前状态），shadow ray 走 `TERMINATE_ON_FIRST_HIT | DISABLE_CLOSESTHIT` 就够了，不需要 stride=2。只有引入 non-opaque 后 shadow ray 是否需要走 anyhit 才成为问题。先 stride=1 + 2 个 miss record（env + shadow）是自然演进
- **Miss 程序扩展**：当前 1 个 miss。Phase 4 需要 2 个：`__miss__env`（环境光）+ `__miss__shadow`（设 visible=1）。SBT miss section 变为 2 records，`optixTrace` 的 missIndex 参数选择

---

## 12. Env Map 表示与 Miss Shader

### 选项 A：Cubemap（Himalaya）

- equirect HDR → CUDA cubemap texture → miss shader `texCubemap<float4>(tex, x, y, z)`
- 优势：硬件 cubemap filtering；无极地畸变
- 劣势：equirect → cubemap 需要一次 compute/CUDA kernel 转换

### 选项 B：Equirect 2D 纹理直接采样

- miss shader 计算 equirect UV from direction，采样 2D 纹理
- 优势：跳过 cubemap 转换步骤；保留原始精度
- 劣势：极地区域有 sampling 畸变；equirect filtering 质量不如 cubemap

### 讨论点

- **CUDA cubemap texture**：需要创建 `cudaTextureObject_t` with `cudaResourceTypeMipmappedArray` + layered 6-face。OptiX 不直接提供 cubemap 封装，需手动创建
- **Env alias table 的基础数据**：alias table 从 equirect 像素构建（不依赖 cubemap）。所以无论 miss shader 用 cubemap 还是 equirect 采样，alias table 都是从 equirect 数据构建
- **环境旋转**：无论哪种方式，都是对 ray direction 做 Y 轴旋转后采样

---

## 13. Denoiser 预留（Phase 6 的 Phase 4 影响）

Denoiser 是 Phase 6 内容，但 Phase 4 的设计必须不阻碍它。

### 需要在 Phase 4 预留的

1. **Aux buffer output 能力**：closesthit 在 bounce==0 时能写 diffuse_color 和 shading_normal 到 global buffer → 需要 LaunchParams 携带 aux buffer 指针
2. **Aux buffer 分配**：在 Renderer 中分配 2 个额外 CUDA buffer（albedo RGBA16F, normal RGBA16F） → 跟随渲染分辨率
3. **Accumulation reset 时 clear aux buffers**：miss ray（天空）不写 aux → denoiser 期望 miss pixel 为 (0,0,0)

### Phase 4 可以做的（但不是必须）

- 实际写入 aux data（作为 closesthit 的一个 conditional write）— 有利于后续 Phase 6 集成
- 暴露一个 "show aux buffer" debug view — 验证 aux 数据正确性

### Phase 4 不应该做的

- 不集成 OptiX Denoiser（Phase 6 的内容）
- 不做 temporal denoiser 的 motion vector 输出（M2 的内容）

---

## 14. 未来 M2 的架构兼容性

Phase 4 的架构选择应避免与以下 M2 技术矛盾：

| M2 技术 | 对 Phase 4 架构的影响 |
|---------|---------------------|
| **ReSTIR DI** | 需要在 primary hit 后独立执行 reservoir 操作。倾向 multi-launch。Megakernel 不阻碍（可在 bounce 0 后插入逻辑），但 wavefront 更自然 |
| **ReSTIR GI** | 每个 bounce 都需要 reservoir。倾向 wavefront |
| **SHaRC / NRC** | Neural radiance cache — 在 closesthit 内查询 cache。与 megakernel 兼容 |
| **NRD** | 需要 motion vectors, linear_z, normal, roughness 等 per-pixel AOV |
| **Temporal accumulation** | 需要 motion vectors + reprojection buffer |
| **SER** | OptiX 9.x feature。对 megakernel 最有利 |

### 建议预留

1. **LaunchParams 留出 aux buffer 指针位置**：即使 Phase 4 不输出 motion vector，预留字段避免未来改布局
2. **closesthit 能写 global buffer**：不仅 payload，还能写 per-pixel 数据（aux, reservoir 等）
3. **sample count 与 frame_index 分离**：ReSTIR 可能需要不同的 "sample count" 语义
4. **hit data 可重用**：closesthit 的 hit position / normal / material 数据结构化存储，便于后续 pass 读取

---

## 15. Ping-pong 与双 Stream 架构的演进

### 当前状态

```
compute_stream: wait prev tonemap done → params upload → optixLaunch → record raygen done
display_stream: wait reverse sem → wait prev raygen done → tonemap → record tonemap done → signal
```

accum_buffers_[2]: 乒乓切换，raygen 写一个，tonemap 读另一个。

### Phase 4 的变化

1. **Raygen 变长**：N samples × max_bounces traces → compute_stream 占用时间增加
2. **Running average 读写**：raygen 额外读 buf[accum_index_]（与 tonemap 并行读同一 buffer）
3. **Timing events**：需要在 optixLaunch 前后插入 timing events

### 需要确认的问题

- **Timing events 与 sync events 的关系**：现有 `event_raygen_done_` 是 `DisableTiming`。可以改为 timing-enabled（既做 sync 又做 timing），或新增独立 timing events
- **display_stream 的等待时间**：如果 raygen 变慢（多 sample），display_stream 等 `event_raygen_done_[prev_slot]` 的时间变长。这不影响正确性但降低流水线效率 — 不过 adaptive sample count 会自动限制 raygen 耗时
- **双 stream 在高 N 下是否仍有价值**：如果 raygen 占 95% 帧时间（N=50），tonemap 的重叠只省 5% 的帧时间。双 stream 的收益随 N 增大而递减。但代码复杂度不增加，保持即可
- **Tonemap 显示的是上一帧的累积**：Frame N 的 tonemap 读 buf[accum_index_] = Frame N-1 的 raygen 写入结果。显示总是比最新累积落后 1 帧。对 PT 渐进渲染完全可接受（第 99 帧显示 98 个 sample 的平均值，同时第 99 个 sample 正在计算）

---

## 16. Tonemap 的演进

### 当前状态

- 简单 `clamp01(hdr)` — 不是真正的 tonemap
- 直接写入 `cudaSurfaceObject_t`

### Phase 4 需要

- 实现真正的 HDR → LDR tonemap（ACES 或其他）
- 如果渲染分辨率 ≠ 呈现分辨率：tonemap kernel 只处理 render_width × render_height 区域
- 输入：accumulation buffer（render resolution）
- 输出：display surface 的 (0,0)-(render_w, render_h) 子区域

### 讨论点

- **Tonemap 函数选择**：ACES (Himalaya 用的)、Reinhard、Uncharted 2 Filmic、AgX。可以作为运行时参数暴露
- **Exposure control**：可以是固定值、auto-exposure（基于 average luminance），或 UI 可调。Himalaya 通过 `camera_position_and_exposure.w` 传递。Exposure 只在 tonemap 阶段应用（PT 累积是线性 HDR，不含 exposure）：
```cuda
float3 exposed = hdr * exposure;
float3 tonemapped = aces(exposed);
```
tonemap kernel 签名需扩展以接收 exposure 参数
- **Sub-rectangle write**：`surf2Dwrite` 写任意 (x,y) 坐标 — 渲染分辨率 < display surface 时只写左上角，完全合法

---

## 17. 总结：Phase 4 必须做出的决策

| # | 决策 | 关键影响 |
|---|------|---------|
| 1 | **Kernel 架构** (§1) | 代码结构的根基 |
| 2 | **多 Sample 执行策略** (§2) | raygen shader 结构；累积公式 |
| 3 | **自适应控制策略** (§3) | timing 基础设施；控制器算法 |
| 4 | **渲染分辨率解耦方式** (§4) | buffer sizing；display pipeline |
| 5 | **累积策略** (§5) | LaunchParams 设计；ping-pong 交互 |
| 6 | **Payload 设计** (§6) | closesthit 与 raygen 的数据传递 |
| 7 | **RNG 架构与存储** (§7) | 数据上传方式；constant vs global memory |
| 8 | **纹理 LOD** (§8) | payload 扩展；纹理采样质量 |
| 9 | **光源数据结构** (§9) | NEE 采样效率；GPU buffer layout |
| 10 | **Shadow ray 策略** (§10) | anyhit 处理；透明物体交互 |
| 11 | **SBT / Pipeline 配置** (§11) | BLAS 构建改造；shader program 数量 |
| 12 | **Env map 表示** (§12) | 纹理格式；miss shader |
| 13 | **Denoiser 预留** (§13) | aux buffer；LaunchParams 字段 |
| 14 | **Tonemap 实现** (§16) | HDR→LDR 映射；resolution 处理 |
| 15 | **Device 代码组织** (§18) | 代码拆分方式；编译流程 |
| 16 | **数据上传流程** (§19) | 新增数据资产（Sobol / blue noise / cubemap / emissive）的加载管线 |

### 可推迟的决策

- Light BVH（Phase 5 或 M2）
- Wavefront 重构（M2）
- Reservoir 格式（M2 ReSTIR）
- Temporal accumulation / motion vectors（M2）
- OptiX Denoiser 集成（Phase 6）
- 自适应渲染分辨率（Phase 4 暴露参数，不做自适应）
- 自适应 bounce depth（Phase 4 暴露参数，不做自适应）

---

## 18. Device 代码组织架构

> 当前 programs.cu ~307 行。完整 PT（BRDF、采样、NEE、ray cone）将膨胀到 1500+ 行。需要提前决定代码组织方式。

### 选项 A：单文件 + `__device__` inline 头文件

- 保持一个 `programs.cu`（含 `extern "C"` entry points），把 BRDF、采样、ray cone 等拆到 `.cuh` 头文件以 `__forceinline__ __device__` 函数形式 `#include` 进来
- 优势：OptiX 只需一个 module；编译器看到全部代码可做全局优化（内联 + 寄存器分配）
- 劣势：单次编译时间随代码量线性增长

### 选项 B：多 .cu → 多 Module → OptiX Module Linking

- OptiX 7.7+ 支持将多个 module 链接成一个 pipeline
- 每个 `.cu` 编译为一个 `.optixir`，link 时合并
- 优势：增量编译；模块隔离
- 劣势：实现复杂度高；跨 module 调用有限制（必须是 `__noinline__` callable 或链接时解析的普通函数）；CMake 集成更复杂

### 讨论点

- **Himalaya 的做法**：GLSL `#include "rt/pt_common.glsl"`——所有代码一起编译为一个 shader module（已确认：closesthit.rchit include pt_common.glsl，后者又 include brdf.glsl、transform.glsl）
- **当前构建流程**：`CompileOptiXIR.cmake` 的 `compile_optix_ir()` 函数已将单个 `.cu` 通过 nvcc `--optix-ir` 编译为 `.optixir`。选项 A 最自然——增加 `.cuh` 头文件即可，不改构建流程
- **编译时间估计**：~1500 行 CUDA + OptiX device code 编译时间约数秒（nvcc JIT 到 compute_89 虚拟架构），不构成开发瓶颈
- **推荐的 .cuh 拆分粒度**：
  - `brdf.cuh` — D_GGX / V_SmithGGX / F_Schlick / VNDF sampling / cosine hemisphere
  - `rng.cuh` — Sobol / blue noise / PCG hash / rand_pt
  - `ray_utils.cuh` — offset_ray_origin / ensure_normal_consistency / ray cone propagation
  - `nee.cuh` — alias table sampling / MIS / shadow ray helper
  - `math_utils.cuh` — 当前已有的 float3/float4 运算符 + dot/cross/normalize 等

---

## 19. 数据来源与上传流程

> 记录 PT 新增数据资产的来源、处理方式和 GPU 上传路径。非决策项，仅作实现参考。

### 19.1 Sobol Direction Numbers

- **来源**：Joe & Kuo 标准文件（Himalaya: `noise/new-joe-kuo-6.21201`，文本格式，21201 行/~1.9MB，包含 d=2 至 d=21202 的方向数）
- **解析**：CPU 端解析前 127 行（dim 2-128），加上 dim 1（Van der Corput，算法生成）= 128 dims × 32 bits direction numbers = 4096 个 `uint32_t`（16KB）
- **上传**：一次性 `cudaMalloc` + `cudaMemcpy`，不随帧变化
- **GPU 存储方式**：见 §7.1（`__constant__` vs LaunchParams 指针 vs embed）
- **Himalaya 验证**：`sobol_data.directions[dim * 32 + bit]`，4096 entries（pt_common.glsl:374-376）

### 19.2 Blue Noise 纹理

- **来源**：128×128 单通道灰度图（Himalaya: `noise/HDR_L_0.png`）
- **实际格式**：**16-bit** grayscale PNG（R16Unorm），不是 8-bit
- **上传选项**：
  - `cudaMipmappedArray`（单 mip level）+ `cudaTextureObject_t`（R16 → float 硬件归一化）→ 走 texture cache
  - 或加载为 raw `uint16_t` buffer（32KB），手动归一化
- **访问**：通过 LaunchParams 中的 `cudaTextureObject_t` 或 device pointer
- **Himalaya 验证**：`texelFetch(textures[blue_noise_index], noise_coord, 0).r` 按 float 读取（pt_common.glsl:452）

### 19.3 Equirect HDR → Cubemap

- **来源**：equirect HDR 纹理（已通过场景加载获得 `cudaTextureObject_t`）
- **转换方式**：CUDA kernel 对 cubemap 6 face × face_size² 的每个 texel，计算对应 3D 方向 → equirect UV → 采样 equirect 纹理
- **Himalaya 验证**：`shaders/ibl/equirect_to_cubemap.comp`，dispatch (ceil(size/16), ceil(size/16), 6)，使用 `ibl_common.glsl` 的 `cube_dir(face, uv)` 计算方向
- **替代**：不做 cubemap 转换，miss shader 直接采样 equirect 2D 纹理（见 §12 选项 B）

### 19.4 Emissive 三角形数据

- **来源**：场景加载时扫描所有 mesh，筛选 `emissive_factor != 0` 的三角形
- **数据**：每个 emissive 三角形需 world-space 顶点(3×vec3) + emission(vec3) + area(float) + UV(3×vec2) + material_index(uint) = ~96 bytes
- **Alias table**：CPU 构建（Vose's algorithm，O(N)），与三角形数据一同上传
- **重建时机**：场景切换时重建；IBL rotation 不影响（shader 内旋转方向）

### 讨论点

- **资产文件集成**：Sobol direction numbers 和 blue noise 纹理需作为项目资产打包。可嵌入二进制（`xxd` / CMake resource embedding）或作为运行时加载文件
- **上传时机**：Sobol + blue noise 在 Renderer 初始化时一次性上传。Emissive 数据在 `load_scene` 时上传。Cubemap 转换在 IBL 加载时执行
- **依赖关系**：Env alias table 从 equirect 像素构建（不依赖 cubemap），可与 cubemap 转换并行或独立
