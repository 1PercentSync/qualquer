# 当前阶段

> 目标：自适应帧率 + 采样质量收尾（Stochastic Alpha、Ray Cone LOD、Normal Map Specular AA）
>
> 任务清单见 `tasks/phase4.5.md`（小项与下文同序一一对应）。
> Phase 4 已归档，细节见 `docs/archive/phase4.md`。

---

## 依赖关系

```
Step 15（自适应 sample 数）     ← 独立

Step 16（Stochastic Alpha）     ← 独立

Step 17（Ray Cone LOD）         ← 独立（payload 需扩至 18 以携带 cone）
     ↓
Step 18（Normal Map Specular AA）← 依赖 Step 17 的 cone footprint
```

MUSTREAD:8

---

## Step 15：自适应 Sample 数

**验证**：Mode 1/2/3 自动切换，帧率符合目标。

策略与目标帧率表见 `technical-decisions.md`「自适应帧率」。

### 刷新率查询

`glfwGetVideoMode` 取当前 monitor 刷新率，作为 Mode 目标计算输入。

### 帧时间测量

CUDA events 测 raygen（compute_stream）执行时间，作为 spp 调节与 Mode 判定输入。呈现链路 debug 计时（Phase 4 已有）可辅助观察，不替代 raygen 测量。

### Mode 选择逻辑

按 1spp 能否满足 Mode 1 目标 → 否则 Mode 2 → 否则 Mode 3；Mode 内用帧时间余量调节 `samples_per_frame`（上限 64，TDR）。

### Ping-pong / 串行切换

Mode 1 保持双 stream ping-pong 并行；Mode 2/3 切到单 stream 串行（消除额外一帧显示延迟）。切换时注意 FrameSlot event / interop semaphore 语义不破坏。

### UI：mode 显示与手动/自动切换

当前 mode 只读显示；手动/自动开关（手动时沿用用户 `samples_per_frame` 滑块）。

---

## Step 16：Stochastic Alpha

**验证**：blend 材质正确半透明。

### anyhit：Blend 模式 stochastic alpha

在现有 `__anyhit__alpha` 上扩展：

- `alpha_mode == Mask`：保持 `texel_alpha < cutoff → ignore`（已有）。
- `alpha_mode == Blend`：`hash(pixel_index, sample_index, primitive_id) > texel_alpha → optixIgnoreIntersection()`。

必须用 hash 而非 Sobol（anyhit 调用次数与顺序不确定；见 `technical-decisions.md`「Stochastic Alpha 采样」）。Shadow ray 与 bounce 共享 anyhit 时，半透明阴影与主路径一致。

---

## Step 17：Ray Cone LOD

**验证**：高频纹理 aliasing 减少，远处纹理正确模糊。

mip 基础设施已就绪；当前固定 LOD 0 的采样点改为 cone 驱动 LOD。实现时对照 Himalaya / 标准 ray cone 文献校准细节。

### payload 接入 cone_width / cone_spread

当前 pipeline 为 16 个 payload 寄存器。本小项将 `numPayloadValues` 扩至 18，并在 `payload_helpers` 中增加 cone 字段读写：

| Register | 内容 |
|----------|------|
| p16 | cone_width（float） |
| p17 | cone_spread（float） |

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
