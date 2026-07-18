# 当前阶段

> 目标：审查收尾 + 自适应帧率 + 采样质量收尾（Stochastic Alpha、Ray Cone LOD、Normal Map Specular AA）
>
> 任务清单见 `tasks/phase4.5.md`（小项与下文同序一一对应）。
> Phase 4 已归档，细节见 `docs/archive/phase4.md`。

---

## 依赖关系

```
Step 14.6（审查优先修复）       ← 独立；先于 14.7–14.9 / 15
     ↓
Step 14.7（小修复与清理）       ← 独立
Step 14.8（采样与光照优化）     ← 独立
Step 14.9（吞吐与收敛）         ← primary 复用与 Step 15 叠加；可先于 15
     ↓
Step 15（自适应 sample 数）     ← 独立

Step 16（Stochastic Alpha）     ← 独立

Step 17（Ray Cone LOD）         ← 独立（payload 需扩至 18 以携带 cone）
     ↓
Step 18（Normal Map Specular AA）← 依赖 Step 17 的 cone footprint
```

MUSTREAD:8

---

## Step 14.6：审查优先修复

**验证**：firefly clamp / sequence index / 中间图精度问题消除；决策变更写入 `technical-decisions.md`。

### Firefly clamp

实现 firefly clamp。

### DLSS ON 下 Sobol path sequence index

DLSS 模式把 `sample_count` 每帧清零，同时错误地将它用作 Sobol 的 path sequence index，导致原生 RNG 与 EON BSDF
产生稳定的跨维度相关噪声，并被 DLSS-RR 时域历史持续加深。sequence index 与 per-slot 累积计数解耦，使用不随 DLSS 清零的序号。

### 显示中间图 RGBA16F

线性颜色在 sRGB 编码前被 `R8G8B8A8_UNORM` 提前量化。改为 `RGBA16F` 中间图（验证：阴影色阶断层消失）。落地时更新「显示
Buffer」决策。

---

## Step 14.7：小修复与清理

**验证**：契约 / 守卫 / 日志 / 解耦项无回归。

### `sobol_rng` Sobol 主路径 `[0,1)` 契约

`rng.cuh` Sobol 主路径仍违反 `[0,1)` 契约（此前仅修了 xxhash fallback 与 host 端）。改为
`return static_cast<float>(sobol_val >> 8) * 0x1p-24f;`。

### `scene_loader` 空 mesh 索引验证

`last_vertex = vertex_count - 1` 在 `vertex_count == 0` 时下溢。索引验证前守卫空 mesh（跳过）。

### `scene_loader` 无效索引日志汇总

循环内逐索引 `spdlog::warn` 在损坏 mesh 上可产生海量日志。统计无效索引数，循环结束后一条汇总。

### `kPosInf` 具名常量

`nee.cuh` 与 `programs.cu` 中 `__int_as_float(0x7f800000)` 各一处。在 `math_utils.cuh` 定义 device 具名常量（如 `kPosInf`
）并替换。

### closesthit pass-through 延迟 normalize

pass-through 路径的 `is_back_face` 只需 face normal 符号，不必归一化。将 `normalize` 延后到 pass-through 判定之后。

### `init_brdf_params` 与管线关注点解耦

`dlss_enabled` / `bounce` 不应进入 BRDF 构造。`init_brdf_params` 对纯金属始终跳过 `E_glossy`；closesthit aux 写入（已有
`dlss_enabled && bounce == 0` 门控）单独算 `E_glossy` 并写入 aux。

---

## Step 14.8：采样与光照优化

**验证**：alias / payload / NEE 变更后无偏或可接受 bias 已记录；相关决策更新。

### Env alias table 降采样

全分辨率 alias 表 L2 共享预算代价过大（4K×2K ≈ 96 MB）。降采样至目标上限 1024×512，自动计算最小 power-of-2 因子（8K→÷8,
4K→÷4, 2K→÷2, ≤1K×512→不降采样）；box-filter 构建；采样时 block 内 jitter 恢复角度精度；PDF 按 block 粒度，构建与求值分辨率一致。
cubemap 保持全分辨率。非 power-of-2 输入维度产生非最优降采样比，已知可接受。

### OptiX payload type semantics

当前 `OptixProgramGroupOptions` 零初始化（无 payloadType），编译器跨 shadow trace 保守保存全部 16 个 payload register。 声明单一
bounce payload type + shadow 改为零 payload `optixTraverse`，降低 continuation stack 压力。

**方案**：

1. 定义寄存器读写表（16 行，每行说明该寄存器在 caller/CH/MS/AH 中谁读谁写），通过 `OptixModuleCompileOptions`
   的 `numPayloadTypes=1` 传给编译器（`numPayloadValues` 置 0，两者互斥）。`Pipeline::init` 增加
   `payload_types` / `num_payload_types` 参数（renderer 定义内容，optix 层透传）。编译器拿到这张表后，能区分 typed bounce
   trace（16 个寄存器）和 untyped shadow trace（零寄存器），跨 shadow trace 时不再保存/恢复 bounce 寄存器
2. Shadow ray 改为零 payload `optixTraverse(...)` + `!optixHitObjectIsHit()`——shadow 只需要知道「有没有撞到东西」，
   `optixHitObjectIsHit()` 直接回答，不需要通过寄存器传数据。AH（alpha 测试）在遍历阶段执行，不属于 invoke 阶段，
   零 payload 不影响 AH 触发
3. 移除 `__miss__shadow`——它唯一的作用是往寄存器写 `visible=1`，shadow 不再用寄存器后没有存在意义。程序组 4→3，SBT
   miss 记录 2→1
4. Bounce trace 的 `optixTraverse` / `optixInvoke` 加 type 参数 `kPayloadTypeBounce`（= `OPTIX_PAYLOAD_TYPE_ID_0`），
   告诉 OptiX 这次 trace 用 bounce 的寄存器布局；shadow trace 不传 type——这是编译器区分两者的依据
5. `OptixProgramGroupOptions` 保持零初始化（单一 type 可唯一推导）
6. 单一 type 下 `optixSetPayloadTypes`（程序内部声明支持哪些 type 的 API）不需要——编译器从模块选项中已知唯一 type
   的语义，SDK 示例在 CH/MS 中调用它是多 type 场景的惯例，单 type 下无效果

bounce payload 16 register 语义：

| 寄存器 | 内容                            | Caller     | CH         | MS    | AH |
|--------|---------------------------------|------------|------------|-------|----|
| p0-p8  | origin / direction / throughput | READ       | WRITE      | —     | —  |
| p9-p11 | color                           | READ       | WRITE      | WRITE | —  |
| p12    | hit_distance                    | READ       | WRITE      | WRITE | —  |
| p13    | last_brdf_pdf                   | READ_WRITE | READ_WRITE | READ  | —  |
| p14    | sequence_index                  | WRITE      | READ       | —     | —  |
| p15    | bounce                          | WRITE      | READ       | —     | —  |

### 单策略 NEE 混合

待重写

---

## Step 14.9：吞吐与收敛

**验证**：Catmull-Rom 优化无画质回退；temporal offset A/B 结论写入决策。

### Catmull-Rom tonemap 16→4 fetch

DLSS OFF 且 render ≠ display 时 `sample_catmull_rom` 为 4×4 point fetch。对同一 `cudaArray` 建 linear-filter texture
object，按 GPU Gems 2 Ch.20 将相邻权重合并为 bilinear fetch（2×2 = 4 次）。

### Sobol temporal offset 收敛验证

`frame_index * φ` 每帧变化，不在固定 CP rotation 定理覆盖内。A/B：有 / 无 temporal offset，比较 DLSS OFF 累积 RMSE–spp
曲线，再决定去留。

---

## Step 15：自适应 Sample 数

**验证**：Mode 1/2/3 自动切换，帧率符合目标。

策略与目标帧率表见 `technical-decisions.md`「自适应帧率」。

### 刷新率查询

`glfwGetVideoMode` 取当前 monitor 刷新率，作为 Mode 目标计算输入。

### 帧时间测量

CUDA events 测 raygen（compute_stream）执行时间，作为 spp 调节与 Mode 判定输入。呈现链路 debug 计时（Phase 4 已有）可辅助观察，不替代
raygen 测量。

### Mode 选择逻辑

按 1spp 能否满足 Mode 1 目标 → 否则 Mode 2 → 否则 Mode 3；Mode 内用帧时间余量调节 `samples_per_frame`（上限 64，TDR）。

### Ping-pong / 串行切换

Mode 1 保持双 stream ping-pong 并行；Mode 2/3 切到单 stream 串行（消除额外一帧显示延迟）。切换时注意 FrameSlot event /
interop semaphore 语义不破坏。

### UI：mode 显示与手动/自动切换

当前 mode 只读显示；手动/自动开关（手动时沿用用户 `samples_per_frame` 滑块）。

---

## Step 16：Stochastic Alpha

**验证**：blend 材质正确半透明。

### anyhit：Blend 模式 stochastic alpha

在现有 `__anyhit__alpha` 上扩展：

- `alpha_mode == Mask`：保持 `texel_alpha < cutoff → ignore`（已有）。
- `alpha_mode == Blend`：`hash(pixel_index, sample_index, primitive_id) > texel_alpha → optixIgnoreIntersection()`。

必须用 hash 而非 Sobol（anyhit 调用次数与顺序不确定；见 `technical-decisions.md`「Stochastic Alpha 采样」）。Shadow ray 与
bounce 共享 anyhit 时，半透明阴影与主路径一致。

---

## Step 17：Ray Cone LOD

**验证**：高频纹理 aliasing 减少，远处纹理正确模糊。

mip 基础设施已就绪；当前固定 LOD 0 的采样点改为 cone 驱动 LOD。实现时对照 Himalaya / 标准 ray cone 文献校准细节。

### payload 接入 cone_width / cone_spread

当前 pipeline 为 16 个 payload 寄存器。本小项将 `numPayloadValues` 扩至 18，并在 `payload_helpers` 中增加 cone 字段读写：

| Register | 内容                 |
|----------|----------------------|
| p16      | cone_width（float）  |
| p17      | cone_spread（float） |

### raygen 初始化 cone

`cone_width = 0`，`cone_spread = 2 * tan(0.5 * fov) / render_height`（primary 像素 footprint）。

### closesthit 传播与更新 cone

`cone_width += cone_spread * hit_distance`；bounce 时按 BRDF 散射特性放大 `cone_spread`。

### 纹理采样改为 cone 驱动 LOD

`tex2DLod`，`LOD = log2(cone_width * texture_resolution / triangle_footprint)`（triangle_footprint 由交点偏导或等价几何估计）。

---

## Step 18：Normal Map Specular AA

**验证**：法线贴图接缝处 specular 闪烁减少。

### 由 ray cone footprint 估算法线贴图方差

依赖 Step 17 的 cone footprint，估计法线贴图在像素内的方差（Kaplanyan 2016）。

### 方差叠加到 roughness 并重算 alpha

`roughness² += variance`，clamp 后重算 alpha，再进入 specular 原语。
