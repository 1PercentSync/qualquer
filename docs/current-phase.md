# 当前阶段

> 目标：核心 Path Tracer（BRDF、多 bounce、IBL、NEE+MIS、累积收敛。4000 spp 大致收敛）
> 任务清单见 `tasks/phase4.md`。
> 讨论记录见 `docs/phase4-discussion.md`（D1-D26 决策）。

---

## 实现步骤

### 依赖关系

```
Step 1（Infrastructure + Pipeline 配置）
  ├──→ Step 2（Tonemap）
  ├──→ Step 3（BRDF）
  ├──→ Step 4（Env map）
  └──→ Step 5（依赖 Step 2/3/4 全部完成）→ Step 6 → Step 7 → Step 8
```

Step 1 包含 Pipeline 变更（numPayloadValues=18），后续 Step 均依赖此配置。
Step 2（Tonemap）、Step 3（BRDF）、Step 4（Env map）之间无依赖，可任意顺序。Step 5（Bounce loop）依赖三者全部完成。

### 总览

| Step | 主题 | 验证标准 |
|------|------|----------|
| 1 | Infrastructure & 代码重组（含 Pipeline 配置） | 编译通过，现有渲染行为不变 |
| 2 | Tonemap + Exposure | ambient 着色经 PBR Neutral tonemap 正确显示 |
| 3 | BRDF 函数 | 编译通过（函数在 Step 5 集成到 closesthit） |
| 4 | 环境光 | miss 显示 HDR 全景，命中表面仍为 ambient |
| 5 | Bounce loop + 累积 | 多 bounce PT 在 HDR env 下工作，累积收敛可见 |
| 6 | Alpha 处理 + SBT 扩展 | 树叶/栅栏正确镂空，单面材质背面穿透 |
| 7 | NEE + MIS | 噪声大幅降低，小光源可见，4000 spp 大致收敛 |
| 8 | UI 参数面板 | 运行时参数可调，Phase 4 完成 |

---

## 设计决策摘要

所有决策详见 `docs/phase4-discussion.md`（D1-D28）。

### 架构

| 决策 | 选择 | 理由 |
|------|------|------|
| D1 Kernel 架构 | Megakernel + SER | SER 消除 divergence，零中间 buffer；未来按需 → Multi-launch |
| D2 Payload | 全信息 18 registers | SER 重排后 payload 随线程移动，零全局内存 |
| D3 代码组织 | 单 .cu + 多 .cuh | 编译器全局内联优化，构建零改动 |
| D4 多 Sample | Raygen 内循环 | 单次 launch，寄存器内累积，max 64 spp/frame |
| D5 累积 | Separate Sum | 精度优于 running average，count 是全局值 |
| D7 RNG | PCG hash（Phase 4） | Sobol + Blue Noise 推迟到 Phase 4.5 |
| D26 迁移预留 | PathState / payload helpers / bounce isolation | 零开销代码组织 |
| D27 能量补偿 | Turquin + Sforza 多项式 | 零 LUT，不改采样，specular 乘增益 |
| D28a Diffuse 模型 | EON（Energy-Preserving Oren–Nayar） | 能量守恒+保持，互易，粗糙回射，OpenPBR 标准 |
| D28 准确性审计 | 全部已决策 | Shadow terminator、EON、耦合修正纳入 Phase 4；specular AA 纳入 4.5；F82-tint 不纳入 |

### 光源

仅 IBL（环境贴图）+ emissive 三角形。不实现解析光源（glTF KHR_lights_punctual 的 directional / point / spot 不纳入）。

### Phase 4 / 4.5 边界

**Phase 4 包含**（4000 spp 收敛必需）：
- 多 bounce（Megakernel + SER）、BRDF 全集（specular: GGX + Turquin 补偿, diffuse: EON + CLTC 采样）+ 多散射能量补偿、NEE + MIS、Env cubemap
- Alpha mask + double-sided pass-through
- Ray origin offset + shading normal consistency
- Separate Sum 累积 + reset、Subpixel jitter、PCG hash RNG
- Tonemap（Khronos PBR Neutral）+ 手动 exposure

**Phase 4.5 推迟**（4000 spp 收敛不依赖）：
- 第一部分（收敛质量/性能）：Sobol + Blue Noise、Ray Cone LOD、Russian Roulette、Firefly Clamping、Stochastic Alpha、IBL 旋转、Target sample count、Render resolution decoupling
- 第二部分（Denoiser）：Aux data 写入 + debug view、OptiX Denoiser

---

## 技术要点

### PCG Hash RNG

```cuda
__forceinline__ __device__ uint32_t pcg_hash(uint32_t input) {
    uint32_t state = input * 747796405u + 2891336453u;
    uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

__forceinline__ __device__ float rng(uint32_t pixel_index, uint32_t sample_index, uint32_t dimension) {
    uint32_t seed = pcg_hash(pixel_index ^ pcg_hash(sample_index ^ pcg_hash(dimension)));
    return float(seed) / float(0xFFFFFFFFu);
}
```

维度分配（与 Himalaya Sobol 一致，Phase 4.5 升级时无缝切换）：
- dim 0-1：subpixel jitter
- per-bounce base = `2 + bounce × 12`：+0 lobe_select, +1/+2 brdf_xi, +3 reserved（RR，Phase 4.5）, +4..+7 env_nee, +8..+11 emissive_nee

### PathState

MUSTREAD:8
```cuda
struct PathState {
    float3 origin;
    float3 direction;
    float3 throughput;
    float3 radiance;
    uint32_t pixel_index;
    uint32_t sample_index;
    uint32_t bounce;
    bool alive;
};
```

Phase 4 作为 raygen 局部变量。Multi-launch 迁移时变为全局 buffer 元素类型。

### Payload Layout (18 registers)

| Register | 内容 |
|----------|------|
| p0-p2 | next_origin (float3) |
| p3-p5 | next_direction (float3) |
| p6-p8 | throughput_update (float3) |
| p9-p11 | color (float3) — emissive + NEE radiance |
| p12 | hit_distance (float) — <0 表示 miss |
| p13 | env_mis_weight (float) |
| p14 | last_brdf_pdf (float) |
| p15-p16 | reserved（Phase 4.5 ray cone） |
| p17 | bounce index (uint32) |

### SBT / Pipeline

| 类型 | 数量 | 说明 |
|------|------|------|
| Raygen | 1 | `__raygen__rg` |
| Miss | 2 | `__miss__env`（missIndex=0）、`__miss__shadow`（missIndex=1） |
| Hitgroup | 2 | 0: opaque（DISABLE_ANYHIT）、1: non-opaque（closesthit + anyhit） |

stride=1。几何体通过 `sbtOffset` 选择 hitgroup。Shadow ray 用 `DISABLE_CLOSESTHIT` flag。

### LaunchParams 扩展

Phase 3 基础上新增：

```cpp
// PT 参数
uint32_t max_bounces;
uint32_t samples_per_frame;
uint32_t sample_count;
float exposure;  // 线性倍率（app 层由 EV 转换 pow(2, ev)）；渲染层直接 color *= exposure

// 环境光
cudaTextureObject_t env_cubemap;
const EnvAliasEntry* env_alias_table;
uint32_t env_alias_count;
uint32_t env_alias_width;
uint32_t env_alias_height;
float env_total_luminance;

// Emissive
const EmissiveTriangle* emissive_triangles;
const AliasEntry* emissive_alias_table;
uint32_t emissive_count;
float emissive_total_power;
```

### 语义拆分

| 字段 | 用途 | Reset 行为 |
|------|------|-----------|
| frame_counter_ | 帧号（%2 选 slot；同时上传为 LaunchParams::frame_index 作 device temporal/RNG 源） | 永不 reset |
| sample_count_ | 总累积 sample 数 | camera/config 变化时 = 0 |

> 不单独设立 frame_seed：frame_counter_ 本身单调递增、永不 reset，直接复用作 RNG temporal scramble 源（device 经 LaunchParams::frame_index 读取）。frame_index 的命名已表达「用帧号作种子」的意图，无需另立别名字段。

### Separate Sum 写读配对

Separate Sum 的写（raygen）与读（tonemap）必须配对，保证 sample_count_ 与 buffer 内容一致：

- **写（raygen）**：读旧总和 `accumulation_buffer_read`（= `accum_buffers_[accum_index_]`），加本帧 contribution，写新 buffer `accumulation_buffer`（= `accum_buffers_[1 - accum_index_]`）。覆盖写改为累加。
- **读（tonemap）**：读 `accum_buffers_[accum_index_]`（上帧 raygen 写的新总和），除以 sample_count_（执行时 = 截至上帧完成的累积数）恢复均值。
- **时序一致性**：tonemap 延迟一帧读 buffer，读的是上帧 raygen 写的新总和，对应「截至上帧」的 sample_count_（上帧末尾已递增）。sample_count_ == 0 时除以 1（首帧/reset 后，raygen 覆盖式累加到清零的 buffer）。
- **reset**：camera/config 变化时 sample_count_ = 0 + 清零两 buffer，下帧从单 sample 重新累加。

### Alias Table (Vose's Algorithm)

Env 和 emissive 三角形各一个 alias table。两者 entry 结构不同：env 需要额外存储 per-pixel luminance 供 `env_pdf()` 查找。

```cuda
// Emissive alias table entry (8 bytes)
struct AliasEntry {
    float prob;
    uint32_t alias;
};

// Env alias table entry (12 bytes) — 额外存储 luminance 供 env_pdf() 使用
struct EnvAliasEntry {
    float prob;
    uint32_t alias;
    float luminance;  // per-pixel luminance（不含 sin_theta）
};
```

构建：Vose's O(N)。采样：O(1)。

**Env alias table 细节**：
- 全分辨率（equirect_width × equirect_height），每个像素一个 entry
- 权重 = luminance × sin(theta)（equirect 投影面积校正）
- `env_pdf()` 使用 entry 中的 `luminance`（不含 sin_theta）计算 solid-angle PDF，确保与 alias table 采样分布一致
- alias table 宽高和 total_luminance 通过 LaunchParams 传递（不使用 SSBO header）
- 暂不做磁盘缓存（构建耗时在可接受范围内，后续按需添加）
- 若实测发现 hot pixel 导致采样过度集中，再考虑降采样（÷2 box filter）作为缓解手段

### EmissiveTriangle

```cuda
struct EmissiveTriangle {
    float3 v0;    float _pad0;
    float3 v1;    float _pad1;
    float3 v2;    float _pad2;
    float3 emission;              // emissive_factor（材质常量，非纹理采样值）
    float  area;                  // 世界空间三角形面积
    uint32_t material_index;      // 材质索引（NEE 需要采样 emissive 纹理）
    uint32_t _pad3;
    float2 uv0, uv1, uv2;        // 逐顶点 UV（NEE 采样 emissive 纹理用）
};
```

NEE 采样发光三角形时需要插值 UV 并采样 emissive 纹理，因此 EmissiveTriangle 存储逐顶点 UV。

**Emissive alias table 局限性**：

当前方案的三角形功率仅由 `luminance(emissive_factor) × area` 决定（材质常量，不采样纹理），三角形内部均匀采样。当 emissive 纹理空间变化剧烈时（LED 文字、屏幕内容等），采样分布与实际辐射分布偏差大，方差显著升高。

潜在改进：对 emissive 纹理做低分辨率预积分，为每个三角形建小型条件分布表（先选块，再块内均匀），在内存开销和方差之间取平衡。多数场景中发光体纹理较均匀，纯 factor 粒度通常足够，此优化留作后续按需评估。

### EON Diffuse（Energy-Preserving Oren–Nayar）

**参考实现**：github.com/portsmouth/EON-diffuse（GLSL → CUDA 移植）

替代 Lambertian diffuse。EON = FON 单散射 + Kulla-Conty 式多散射补偿 lobe，能量守恒且保持（通过 furnace test）、互易、捕捉粗糙回射效应。

**公式**（Listing 1）：

```
f_EON(ωi, ωo) = f_ss + f_ms

// 单散射（FON）
AF = 1 / (1 + constant1_FON * r)     // constant1_FON = 0.5 - 2/(3π)
BF = r * AF
s = dot(ωi, ωo) - (N·ωi)(N·ωo)
s/tF = s > 0 ? s / max(N·ωi, N·ωo) : s
f_ss = (ρ/π) * AF * (1 + r * s/tF)

// 多散射补偿
EFo = E_FON(N·ωo, r)               // FON 方向反照率
EFi = E_FON(N·ωi, r)
avgEF = AF * (1 + constant2_FON * r)  // constant2_FON = 2/3 - 28/(15π)
ρ_ms = ρ² * avgEF / (1 - ρ * (1 - avgEF))
f_ms = (ρ_ms/π) * (1 - EFo) * (1 - EFi) / (1 - avgEF)
```

**E_FON 近似**（4 系数多项式，误差 < 0.1%）：

```
E_FON(μ, r) = (1 + r * G/π) / (1 + constant1_FON * r)

G/π = μc * (g1 + μc * (g2 + μc * (g3 + μc * g4)))   // μc = 1 - μ
g1 = 0.0571085289, g2 = 0.491881867, g3 = -0.332181442, g4 = 0.0714429953
```

**采样**：CLTC（Clipped Linearly Transformed Cosine）采样，掠射角方差比 cosine hemisphere 降 100×。LTC 矩阵系数 a/b/c/d 为 (μ, r) 的 rational polynomial fit（Listing 2），采样通过 Nusselt analog + 半圆-半椭圆区域均匀采样实现（Listing 3-4）。

**集成**：`f_total = f_specular × turquin_comp + (1 - metallic) × (1 - F) × f_EON`。Specular 和 diffuse 的能量补偿相互独立。

### Roughness 下限 Clamp

GGX NDF 在 α = 0 时退化为 Dirac delta，D(h) → ∞/0 → NaN。所有使用 roughness 的位置需先 clamp：

```cuda
float alpha = max(roughness * roughness, 1e-4f);
```

### 多散射能量补偿（Turquin + Sforza 多项式）

**公式**：对 specular BRDF 乘补偿系数（Turquin Eq. 16，F_ms 简化为 F0）：

```
compensation = 1 + F0 * (1 - E_ss) / E_ss
throughput_update *= compensation  // 采样时
brdf_value *= compensation         // NEE eval 时
```

其中 `E_ss(r, μ)` 为 GGX 单散射方向反照率，`r = linear roughness ∈ [0,1]`，`μ = cosθ = NdotV ∈ [0,1]`。

**E_ss 有理多项式**（Sforza & Pellacini，3 阶，19 系数）：

```
         P(r, μ)
E_ss = ─────────
         Q(r, μ)

P = c0 + c1·r + c2·μ + c3·r² + c4·r·μ + c5·μ² + c6·r³ + c7·r²·μ + c8·r·μ² + c9·μ³

Q = 1 + c10·r + c11·μ + c12·r² + c13·r·μ + c14·μ² + c15·r³ + c16·r²·μ + c17·r·μ² + c18·μ³
```

系数（MIT 许可，github.com/dsforza96/energy-preservation）：

```
c0  =  1.0247217    c1  = -10.984229    c2  =  10.918318
c3  =  46.93353     c4  = -54.779343    c5  =  21.742077
c6  = -30.368898    c7  =  31.919222    c8  =  -8.013965
c9  =  -6.2407165   c10 = -10.218104    c11 =  10.955399
c12 =  44.08196     c13 = -55.33452     c14 =  21.437538
c15 = -23.744568    c16 =  33.265057    c17 =  -7.9268975
c18 =  -5.930959
```

精度（对 32×32 ground truth 表）：均值误差 0.004，最大误差 0.027。

**集成要点**：

- 补偿系数只依赖 ωo（NdotV + roughness + F0），每个着色点算一次
- 不改变采样方向和 PDF（Turquin 的补偿与主瓣同形）
- specular_probability 不需要调整（二阶效应）
- 对 specular lobe 统一适用（metallic 高时 F0 高补偿强，dielectric 时 F0 ≈ 0.04 补偿自然弱）
- E_ss 结果需 clamp 到 [ε, 1] 避免除零
- **Diffuse-Specular 耦合**（D28c）：diffuse 权重从 `(1-F)` 改为 `(1 - E_spec_compensated)`，其中 `E_spec_compensated = E_ss + F0 * (1 - E_ss)` 为补偿后的 specular 方向反照率，确保 specular + diffuse 总能量 ≤ 1

### Env Cubemap

加载流程：stb_image 加载 equirect HDR → CUDA kernel 转换为 6-face cubemap → `cudaMipmappedArray`（cubemap flag）→ `cudaTextureObject_t`。Miss shader 用 `texCubemap<float4>` 采样。

---

## Himalaya 参考对照

| Qualquer 模块 | Himalaya 源 | 适配说明 |
|---------------|------------|---------|
| BRDF specular（brdf.cuh） | `shaders/rt/pt_common.glsl` | GLSL → CUDA intrinsics |
| BRDF diffuse（brdf.cuh） | github.com/portsmouth/EON-diffuse | GLSL → CUDA，替代 Himalaya 的 Lambertian |
| Closesthit | `shaders/rt/closesthit.rchit` | payload registers 替代 rayPayload struct |
| Raygen bounce loop | `shaders/rt/reference_view.rgen trace_path()` | 加 sample loop + SER |
| Miss shader | `miss.rmiss` + `shadow_miss.rmiss` | GLSL → CUDA |
| Any-hit alpha | `shaders/rt/anyhit_alpha.rahit` | GLSL → CUDA |
| Alias table | `framework/alias_table.h` | 照搬 |
| Emissive collection | `framework/scene_as_builder.cpp` | 适配 CUDA 数据结构 |
| Env cubemap | `framework/texture.cpp` | Vulkan compute → CUDA kernel |
| Ray offset | `shaders/rt/pt_common.glsl offset_ray` | GLSL → CUDA |
| Normal consistency | `shaders/rt/pt_common.glsl` | GLSL → CUDA |

---

## 完成标准

- 多 bounce PT 正确渲染（金属反射、漫反射 GI、环境光照）
- BRDF 正确（Cook-Torrance specular + Turquin 补偿 + EON diffuse）
- HDR 环境光正确（cubemap miss shader）
- NEE + MIS 工作（env + emissive，小光源可见）
- Alpha mask 正确（树叶/栅栏镂空）
- Double-sided pass-through 正确
- 累积收敛可见（静止相机下噪声减少）
- 累积 reset 正确（相机/参数变化时重新开始）
- Tonemap 正确（PBR Neutral）
- UI 参数可调（max_bounces, samples_per_frame, exposure）
- 4000 spp 下各类场景大致收敛
- 无 OptiX / CUDA 报错
