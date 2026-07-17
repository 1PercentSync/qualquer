# 当前阶段

> 目标：自适应帧率 + 采样质量收尾（Stochastic Alpha、Ray Cone LOD、Normal Map Specular AA）
>
> 任务清单见 `tasks/phase4.5.md`（小项与下文「技术要点」同序一一对应）。
> 已落地的 PT 内核、BRDF、NEE、DLSS-RR、累积与 FrameSlot 等见源码与 `docs/archive/phase4.md`。

---

## 范围

Phase 4 已交付功能完整的 PT 与实时前置（含 DLSS-RR、Sobol、分辨率解耦、数值正确性与结构重构）。本阶段仅完成下列未竟项：

| 纳入                                  | 不纳入（已否决或已在 Phase 4 完成）                      |
|-------------------------------------|---------------------------------------------|
| 自适应 sample 数（三级降级）                  | Firefly clamping、target sample count        |
| Stochastic Alpha（Blend）             | Alpha Mask / double-sided pass-through（已完成） |
| Ray Cone LOD                        | Blue noise、OptiX Denoiser                   |
| Normal Map Specular AA（依赖 Ray Cone） | —                                           |

---

## 实现步骤

### 依赖关系

```
Step 15（自适应 sample 数）     ← 独立

Step 16（Stochastic Alpha）     ← 独立

Step 17（Ray Cone LOD）         ← 独立（payload 18 寄存器位已预留）
     ↓
Step 18（Normal Map Specular AA）← 依赖 Step 17 的 cone footprint
```

MUSTREAD:8

### 总览

| Step | 主题                     | 验证标准                      |
|------|------------------------|---------------------------|
| 15   | 自适应 sample 数           | Mode 1/2/3 自动切换，帧率符合目标    |
| 16   | Stochastic Alpha       | blend 材质正确半透明             |
| 17   | Ray Cone LOD           | 高频纹理 aliasing 减少，远处纹理正确模糊 |
| 18   | Normal Map Specular AA | 法线贴图接缝处 specular 闪烁减少     |

---

## 设计决策摘要

| 主题                   | 选择                                        | 详见                                            |
|----------------------|-------------------------------------------|-----------------------------------------------|
| 自适应策略                | 三级降级（ping-pong ≥150fps / 串行 / 极端）         | `technical-decisions.md`「自适应帧率」               |
| Stochastic Alpha RNG | hash(pixel, sample, primitive)，不用 Sobol   | `technical-decisions.md`「Stochastic Alpha 采样」 |
| 纹理 LOD               | Ray Cone（payload p16/p17）                 | 下文技术要点                                        |
| Specular AA          | Kaplanyan 2016 roughness 方差；不做 LEAN/LEADR | 下文技术要点                                        |

---

## 技术要点

下列小节标题与顺序与 `tasks/phase4.5.md` 复选框一致。编译验证小项无独立指南（见各 Step 总览验证标准）。

### Step 15：自适应 Sample 数

策略与目标帧率表见 `technical-decisions.md`。`max_bounces` / `samples_per_frame` 已从累积 reset 检测中排除（自适应会每帧改
spp，不改变积分对象）。

#### 刷新率查询

`glfwGetVideoMode` 取当前 monitor 刷新率，作为 Mode 目标计算输入。

#### 帧时间测量

CUDA events 测 raygen（compute_stream）执行时间，作为 spp 调节与 Mode 判定输入。呈现链路 debug 计时（Phase 4 已有）可辅助观察，不替代
raygen 测量。

#### Mode 选择逻辑

按 1spp 能否满足 Mode 1 目标 → 否则 Mode 2 → 否则 Mode 3；Mode 内用帧时间余量调节 `samples_per_frame`（上限 64，TDR）。

#### Ping-pong / 串行切换

Mode 1 保持双 stream ping-pong 并行；Mode 2/3 切到单 stream 串行（消除额外一帧显示延迟）。切换时注意 FrameSlot event /
interop semaphore 语义不破坏。

#### UI：mode 显示与手动/自动切换

当前 mode 只读显示；手动/自动开关（手动时沿用用户 `samples_per_frame` 滑块）。

### Step 16：Stochastic Alpha

#### anyhit：Blend 模式 stochastic alpha

在现有 `__anyhit__alpha` 上扩展：

- `alpha_mode == Mask`：保持 `texel_alpha < cutoff → ignore`（已有）。
- `alpha_mode == Blend`：`hash(pixel_index, sample_index, primitive_id) > texel_alpha → optixIgnoreIntersection()`。

必须用 hash 而非 Sobol（anyhit 调用次数与顺序不确定）。Shadow ray 与 bounce 共享 anyhit 时，半透明阴影与主路径一致。

### Step 17：Ray Cone LOD

mip 基础设施已就绪；当前固定 LOD 0 的采样点改为 cone 驱动 LOD。实现时对照 Himalaya / 标准 ray cone 文献校准细节。

#### payload 接入 cone_width / cone_spread

`numPayloadValues = 18` 已配置：

| Register | 内容                 |
|----------|--------------------|
| p16      | cone_width（float）  |
| p17      | cone_spread（float） |

#### raygen 初始化 cone

`cone_width = 0`，`cone_spread = 2 * tan(0.5 * fov) / render_height`（primary 像素 footprint）。

#### closesthit 传播与更新 cone

`cone_width += cone_spread * hit_distance`；bounce 时按 BRDF 散射特性放大 `cone_spread`。

#### 纹理采样改为 cone 驱动 LOD

`tex2DLod`，`LOD = log2(cone_width * texture_resolution / triangle_footprint)`（triangle_footprint 由交点偏导或等价几何估计）。

### Step 18：Normal Map Specular AA

依赖 Step 17 的 cone footprint。不做 LEAN/LEADR（预处理与存储复杂度与收益不符）。

#### 由 ray cone footprint 估算法线贴图方差

由 footprint 估计法线贴图在像素内的方差（Kaplanyan 2016）。

#### 方差叠加到 roughness 并重算 alpha

`roughness² += variance`，clamp 后重算 alpha，再进入 specular 原语。

---

## 完成标准

- 自适应 Mode 1/2/3 正确切换，帧率稳定在策略目标附近
- Blend 材质 stochastic alpha 正确，shadow 行为合理
- Ray Cone LOD 降低高频纹理 aliasing，远处纹理平滑
- 法线贴图 specular 闪烁减少
- 无 OptiX / CUDA / DLSS 报错
