# 当前阶段

> 目标：核心 Path Tracer（BRDF、多 bounce、IBL、NEE+MIS、累积收敛）
>
> **范围判据**：以 "4000 spp 大致收敛" 作为 Phase 4 / 4.5 特性归属判据——某特性缺失会导致 4000 spp 也无法收敛 → 归 Phase 4；缺失时 4000 spp 仍能收敛但为实时 PT 必需 → 归 Phase 4.5。4000 spp 是判据不是项目终点，项目长期目标是实时 PT 渲染器（见 `roadmap.md`）。
>
> 任务清单见 `tasks/phase4.md`。
> 讨论记录见 `docs/phase4-discussion.md`（D1-D29 决策）。

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
| 4 | 环境光 + Env map 加载 UI | miss 显示 HDR 全景，命中表面仍为 ambient，运行时可加载 HDR |
| 5 | Bounce loop + 累积 | 多 bounce PT 在 HDR env 下工作，累积收敛可见 |
| 6 | Alpha 处理 + SBT 扩展 | 树叶/栅栏正确镂空，单面材质背面穿透 |
| 7 | NEE + MIS | 噪声大幅降低，小光源可见，4000 spp 大致收敛（Phase 4 范围判据，非项目终点） |
| 8 | UI 参数面板 | 运行时参数可调，Phase 4 完成 |

---

## 设计决策摘要

所有决策详见 `docs/phase4-discussion.md`（D1-D30）。

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
| D28c Diffuse-Specular 耦合 | Sforza 39 系数 E_glossy 多项式 | diffuse 权重 = 1 - E_glossy，精确含 Fresnel + Turquin |
| D28 准确性审计 | 全部已决策 | Shadow terminator、EON、耦合修正纳入 Phase 4；specular AA 纳入 4.5；F82-tint 不纳入 |
| D30 UI 架构 | 领域结构体 + Context 组合引用 | RenderSettings（可调旋钮）+ SceneStats（只读快照），避免 Context 散装膨胀 |

### 光源

仅 IBL（环境贴图）+ emissive 三角形。不实现解析光源（glTF KHR_lights_punctual 的 directional / point / spot 不纳入）。

### Phase 4 / 4.5 边界

**Phase 4 包含**（缺失则 4000 spp 无法收敛）：
- 多 bounce（Megakernel + SER）、BRDF 全集（specular: GGX + Turquin 补偿, diffuse: EON + CLTC 采样）+ 多散射能量补偿、NEE + MIS、Env cubemap
- Alpha mask + double-sided pass-through
- Ray origin offset + shading normal consistency
- Separate Sum 累积 + reset、Subpixel jitter、PCG hash RNG
- Tonemap（Khronos PBR Neutral）+ 手动 exposure

**Phase 4.5 推迟**（缺失时 4000 spp 仍能收敛，但为迈向实时 PT 的必需组件）：
- 前半部分：IBL 旋转、Russian Roulette、Sobol + hash 去相关、Render resolution decoupling、DLSS-RR 集成（含 aux data）、自适应 sample 数
- 后半部分：Stochastic Alpha、Ray Cone LOD、Normal Map Specular AA、DLSS-RR 可选质量提升

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

### Payload Layout

| Register | 内容 |
|----------|------|
| p0-p2 | next_origin (float3) |
| p3-p5 | next_direction (float3) |
| p6-p8 | throughput_update (float3) |
| p9-p11 | color (float3) — emissive + NEE radiance |
| p12 | hit_distance (float) — <0 仅表示几何 miss（sky）；无效 BRDF 采样（VNDF 下半球）写真实 hit t，终止由 throughput_update=0 承担。raygen 首跳捕获依赖此语义区分 hit/sky 决定 aux data 写法，混用会使无效采样像素被误判为 sky、aux 被覆盖成噪点 |
| p13 | env_mis_weight (float) |
| p14 | last_brdf_pdf (float) |
| p15 | sample_index (uint32) — raygen 传递给 closesthit 用于 RNG |
| p16 | reserved（Phase 4.5 → cone_width） |
| p17 | bounce index (uint32) |
| p18 | reserved（Phase 4.5 → cone_spread） |

### SER Coherence Hint 策略

Megakernel 下所有 opaque 几何体共享一个 SBT record，无参数的 `optixReorder()` 只能分离 hit/miss，无法区分不同材质。通过 `optixReorder(hint, numBits)` 传入材质信息提升纹理缓存聚合。

**当前实现（bounce ray）**：

`optixTraverse` 后查询 hit object，取 `material_buffer_offset & 0x3FF` 作为 hint（**10 bits**，掩码）。miss 时 hint=0，自然与所有 hit 材质分离。代价：每线程每 bounce 一次 `geometry_infos[]` global memory 读取。

**实测数据**（部署链修复后 Release 构建，warehouse 场景 62 材质，1920×1080，spp 总吞吐；此前 Step 8 的对比数据因过期二进制部署 bug 作废）：

| 变体 | 1spp sps | 32spp sps |
|------|----------|-----------|
| SER 关（无 reorder） | 740 | ~910（不稳定） |
| 零参 `optixReorder()` | 684 | 1133 |
| 8-bit 掩码 hint | 680 | 1140 |
| **10-bit 掩码 hint（采用）** | 680 | 1100 |
| 12-bit 掩码 hint | 680 | 1090 |
| 16-bit 裸传 hint | 676 | 1062 |

**结论与选择理由**：

- SER 本体在高 spp 下收益显著（+23% vs 无 reorder），必须保留；1spp 下 reorder 是净开销（-8%）但不足以补偿高 spp 损失
- 排序成本随 hint 位数平滑递增（每 4 bits 约 -2~4%）。原 16-bit 裸传声明了 10 个恒零高位，纯浪费排序预算，属用法错误
- 材质 hint 的分组收益在本场景测不出（8-bit vs 零参 +0.6%，噪声内）。判断：测试场景材质空间聚集度高、closesthit 为 compute 主导、Ada 48MB L2 容纳纹理工作集
- 选 10 bits 是容量取舍：1024 材质精确分组覆盖预期场景上限（256 可能突破、1024 不太可能），为材质发散场景保留 hint 通道，接受本场景 ~3% 成本
- 掩码语义：超容量时低位混叠（共格材质仍成组，平滑退化）。掩码利用全局索引的低位紧致性——若全局材质数远超容量且索引稀疏，混叠趋于生日分布，届时的修法是可见材质稠密重映射，而非加宽位数

**潜在扩展（Step 7 shadow ray）**：

Shadow ray 方向高度聚合（大量线程指向同一光源），可将 ray direction 量化为低精度整数（如 octahedral mapping → 8-16 bits）作为 hint，让方向相似的 shadow ray 聚在一起，提升 BVH 遍历缓存命中率。待 NEE 实现后按需评估。

### SBT / Pipeline

| 类型 | 数量 | 说明 |
|------|------|------|
| Raygen | 1 | `__raygen__rg` |
| Miss | 2 | `__miss__env`（missIndex=0）、`__miss__shadow`（missIndex=1） |
| Hitgroup | 1 | closesthit + anyhit，BLAS per-geometry flag 控制 anyhit 调用 |

stride=0。opaque 几何体通过 BLAS geometry flag（`OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT`）跳过 anyhit，non-opaque 几何体通过 `OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL` 触发 anyhit。Shadow ray 用 `DISABLE_CLOSESTHIT` flag。

### LaunchParams 扩展

Phase 3 基础上新增：

```cpp
// PT 参数
uint32_t max_bounces;
uint32_t samples_per_frame;
uint32_t sample_count;  // chain count（read buffer 的已累积数；0 = raygen 覆写模式）
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
| accum_counts_[2] | 每个 ping-pong buffer 的真实 sample 数 | reset 不直接改——chain_count 归零使 raygen 覆写，write slot 的 count 在帧末设为 1 |

> 不单独设立 frame_seed：frame_counter_ 本身单调递增、永不 reset，直接复用作 RNG temporal scramble 源（device 经 LaunchParams::frame_index 读取）。frame_index 的命名已表达「用帧号作种子」的意图，无需另立别名字段。

### Separate Sum 写读配对（per-slot 计数）

每个 ping-pong buffer 自带 sample 数（`accum_counts_[i]`），sum 和 count 作为配对数据一起走，取消全局 `sample_count_` 和 buffer 清零。

- **写（raygen）**：host 算 `chain_count = camera_changed ? 0 : accum_counts_[read_slot]`，上传为 `LaunchParams::sample_count`。raygen 据此决定覆写（= 0）还是累加（> 0）。帧末 `accum_counts_[write_slot] = chain_count + 1`。
- **读（tonemap）**：直接用 `accum_counts_[read_slot]` 作除数（buffer 内容与 count 永远配对）。count == 0（init/resize 后首帧）时 tonemap 输出黑——此时 buffer 未初始化，raygen 正在写另一个 buffer 的第一个 sample。
- **reset**：camera/config 变化时 `chain_count` 归零 → raygen 覆写 write buffer，不碰 read buffer。tonemap 同帧仍读 read buffer（count 有效，数据有效），显示上一次有效画面。下帧 tonemap 读刚写入的 1-sample 数据。
- **不清零 buffer**：覆写模式（`sample_count == 0`）使 raygen 忽略 read buffer 直接写 contribution，等效于清零+累加但无 memset 开销和跨 stream 竞争。

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

**参考实现**：`D:\Github\EON-diffuse`（github.com/portsmouth/EON-diffuse，GLSL → CUDA 移植）

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

**集成**：`f_total = f_specular × turquin_comp + (1 - metallic) × (1 - E_glossy) × f_EON`，其中 `E_glossy(NdotV, roughness, F0)` 为 Sforza 39 系数有理多项式输出的 Turquin 补偿后 specular 含 Fresnel 方向反照率（D28c 耦合修正）。F0 取 specular lobe 的实际 F0 = `lerp(0.04, baseColor, metallic)`；F0 为 RGB 时逐通道求值 E_glossy（3 次标量多项式）。Specular 和 diffuse 的多散射能量补偿相互独立。

### BRDF 参数约定（alpha vs roughness）

Specular 微面元原语（`D_GGX`、`V_SmithGGXCorrelated`、`sample_ggx_vndf`、`pdf_ggx_vndf`）接收 **alpha**（= roughness²），贴合 Heitz 2018 原文与微面元参数惯例；EON diffuse / `E_ss` / `E_FON` / CLTC 拟合接收 **linear roughness r ∈ [0,1]**，贴合多项式拟合变量。

两套惯例并存源于数学源头不同，无法统一到单一参数。closesthit 端同时持有两个变量，各按所属惯例传入：

```cuda
float roughness = max(mr_texel.g * mat.roughness_factor, 0.04f);  // glTF 下限
float alpha     = max(roughness * roughness, 1e-4f);               // D28 NaN 防护
// D_GGX(..., alpha);  sample_ggx_vndf(..., alpha, ...);
// E_ss(roughness, mu);  E_FON(mu, roughness);
```

与 Himalaya（全收 roughness、函数内平方）不同：Qualquer 在 closesthit 单点算 alpha 并 clamp，避免函数内重复平方、让 NaN 防护集中可见。EON 多项式按 linear roughness 拟合，不参与 alpha clamp（r=0 有定义不发散，仅需保证 r∈[0,1]）。

### Roughness 下限 Clamp

GGX NDF 在 α = 0 时退化为 Dirac delta，D(h) → ∞/0 → NaN。clamp 对象是 **alpha**，不是 roughness——在 closesthit 端单点完成（见上节参数约定），specular 原语内部不再重复 clamp。

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
- 对 specular lobe 统一适用（metallic 高时 F0 高补偿强，dielectric 时 F0 ≈ 0.04 补偿自然弱）
- E_ss 结果需 clamp 到 [ε, 1] 避免除零
- **Diffuse-Specular 耦合**（D28c）：diffuse 权重从 `(1-F)` 改为 `(1 - E_glossy)`，其中 `E_glossy(NdotV, roughness, F0)` 为 Sforza 39 系数有理多项式（见下节），输出 Turquin 补偿后 specular 含 Fresnel 的方向反照率，确保 specular + diffuse 总能量 ≤ 1
- specular_probability 不需要调整：E_glossy 对 dielectric（F0=0.04）在正视角约 0.032，与 F_Schlick(NdotV) ≈ 0.04 差距极小，对采样方差影响可忽略

### Diffuse-Specular 耦合（E_glossy，Sforza 39 系数）

**物理含义**：E_glossy(μ, r, F0) 是 Turquin 补偿后 GGX specular 层（含 dielectric Fresnel）的方向反照率——即 specular 层总共拿走了多少能量。`1 - E_glossy` 就是穿透到 diffuse 层的能量比例。

**为什么不能用 (1-F)**：`(1-F)` 忽略了微面元间多次弹跳后穿透到 diffuse 层的光。roughness 高 + 掠射角时，大量光在微面元间弹跳，其中多数最终穿透进 diffuse（因 dielectric 每次弹跳只有 ~4% 被反射）。`(1-F)` 在这些角度过度压制 diffuse。

**为什么不能直接从 19 系数 E_ss 推导**：E_ss 是 F=1 的几何反照率，不含 Fresnel。从 E_ss 推导 E_glossy 需要近似 `E_glossy ≈ F(NdotV) * E_ss * comp`，但这在掠射角不准确（Fresnel 依赖 VdotH 而非 NdotV）。39 系数多项式直接对含 Fresnel 的积分做拟合，无需近似。

**有理多项式**（Sforza & Pellacini，3 阶，39 系数，3D 输入）：

输入：`F0 ∈ [0,1]`（由 IOR 映射），`r = linear roughness ∈ [0,1]`（sqrt(α)），`μ = NdotV ∈ [0,1]`

```
              P(F0, r, μ)
E_glossy = ─────────────────
              Q(F0, r, μ)

P = c0 + c1·F0 + c2·r + c3·μ + c4·F0² + c5·F0·r + c6·F0·μ + c7·r² + c8·r·μ + c9·μ²
  + c10·F0³ + c11·F0²·r + c12·F0²·μ + c13·F0·r² + c14·F0·r·μ + c15·F0·μ²
  + c16·r³ + c17·r²·μ + c18·r·μ² + c19·μ³

Q = 1 + c20·F0 + c21·r + c22·μ + c23·F0² + c24·F0·r + c25·F0·μ + c26·r² + c27·r·μ + c28·μ²
  + c29·F0³ + c30·F0²·r + c31·F0²·μ + c32·F0·r² + c33·F0·r·μ + c34·F0·μ²
  + c35·r³ + c36·r²·μ + c37·r·μ² + c38·μ³
```

系数（MIT 许可，github.com/dsforza96/energy-preservation）：

```
c0  =  0.04301317   c1  =  132.98329    c2  = -0.9273584
c3  = -0.61434704   c4  = -262.23462    c5  = -137.75214
c6  = -234.72151    c7  =  5.125822     c8  = -0.37465897
c9  =  9.284745     c10 =  129.71187    c11 =  171.82188
c12 =  400.04813    c13 =  206.99231    c14 =  1.0847985
c15 =  428.02484    c16 = -2.2108653    c17 = -6.056363
c18 =  0.95864034   c19 = -11.775469    c20 =  139.43494
c21 = -24.177433    c22 = -3.7300687    c23 = -253.77824
c24 =  6.717145     c25 =  98.03935     c26 =  153.19194
c27 = -184.53282    c28 =  230.02286    c29 =  113.9376
c30 =  66.64211     c31 =  108.315094   c32 =  23.577564
c33 =  120.04127    c34 =  102.90899    c35 =  17.030241
c36 =  25.947954    c37 =  75.77901     c38 =  49.348934
```

精度（对 32×32×32 ground truth 表）：待实测确认（论文报告拟合误差与 conductor 同量级）。

**F0 映射**：glTF metallic-roughness 的 specular F0 = `lerp(0.04, baseColor, metallic)`（纯电介质 IOR=1.5 → F0=0.04，纯金属 F0=baseColor）。E_glossy 和 turquin_compensation 的 F0 输入均取此混合值。F0 为 RGB 时逐通道求值（Sforza 多项式按标量 F0 ∈ [0,1] 拟合，全域有效；Filament 同样对 vec3 F0 逐通道处理）。纯金属（metallic=1）无 diffuse，不调用 E_glossy；过渡金属（metallic ∈ (0,1)）的 diffuse 权重 `(1 - metallic) × (1 - E_glossy)` 双重衰减，物理正确。

**参考实现**：`D:\Github\energy-preservation`（github.com/dsforza96/energy-preservation，`scripts/glossy.py` 生成表，`scripts/fit.py` 拟合多项式）

### BRDF eval/sample 接口

multi-lobe BRDF 求值与采样抽成独立接口，closesthit（Step 5 bounce 采样）与 NEE（Step 7 光照方向求值）共用，替代 Himalaya 的 closesthit 内联。

- `build_orthonormal_basis(N, T, B)`：世界空间 TBN 构造（N 近 +Z 时改与 +X 叉乘避免退化），移植自 Himalaya
- `specular_probability(NdotV, F0)`：F_Schlick(NdotV, F0) 的 luminance，clamp [0.01, 0.99] 防除零；D28c 确认不调整
- `BrdfParams`：着色点不变量打包（V/N/T/B、F0=lerp(0.04,base_color,metallic)、diffuse_rho=base_color、alpha、r、NdotV、turquin_comp(RGB)、diffuse_weight=(1-metallic)×(1-E_glossy)(RGB)、p_spec）
- `init_brdf_params(...)`：构造入口，集中 D27/D28c 逐通道补偿（turquin_comp 逐通道调 turquin_compensation 共用标量 E_ss；E_glossy 逐通道用混合 F0；diffuse_weight 双重衰减）；纯金属（metallic=1）跳过 E_glossy、diffuse_weight=0
- `brdf_eval(params, L, NdotL)`：返回 specular（D·V·F·turquin_comp）+ diffuse（diffuse_weight·f_EON）BRDF 值
- `brdf_sample(params, u_lobe, u0, u1)`：返回 {next_dir, throughput_update, pdf_combined}；specular 分支 VNDF 采样，diffuse 分支 CLTC 采样；combined PDF 经 combined_lobe_pdf 组装；VNDF 采样 L_ts.z ≤ 0 时返回 pdf=0/throughput_update=0 表示无效，终止逻辑由 closesthit 处理

`diffuse_rho = base_color`（不含 1-metallic），`(1-metallic)` 并入 `diffuse_weight` 与 `(1-E_glossy)` 双重衰减，符合 f_EON 要求 caller 外部应用 (1-metallic) 的约定。

### 顶点色与 AO

**顶点色**：glTF `COLOR_0` 作为 base_color 的逐顶点乘数：`base_color = baseColorTexture × baseColorFactor × vertexColor`。Vertex 结构体含 `color`（float4，默认白 `{1,1,1,1}`），着色器无条件插值并乘入 base_color（与纹理/法线的"缺失填默认、无条件采样"模式一致）。顶点色 alpha 参与 alpha 处理（Step 6）。

**AO 不参与 PT 着色**：AO 贴图是光栅化管线的全局光照遮蔽近似，PT 通过光线弹射物理计算遮蔽——叠加 AO 等于双重遮蔽。Material 不含 `occlusion_tex` / `occlusion_strength`，SceneLoader 不加载 glTF occlusionTexture。Himalaya 的 PT closesthit 同样不使用 AO（材质结构体保留字段但着色器未读取）。

### 纹理管线层归属重构

Phase 3 建纹理管线时，资产预处理和 CUDA 上传写在同一个 `texture.cpp`（renderer 层）。renderer 是 app 和 optix 之间的公共依赖点，放这里"两边都能用"，但与 architecture.md 的层次定义不符——资产预处理是 app 的"资产管理"职责，CUDA 资源创建是 optix 的"CUDA 内存管理"职责。Phase 4 开始新增资产处理逻辑（HDR 加载、equirect→cubemap、alias table），在此时修正层归属。

**迁移方案**：

| 模块 | 原位置 | 新位置 | 理由 |
|------|--------|--------|------|
| 图片解码（load_image 系列）+ stb_impl | renderer/texture.h,cpp + stb_impl.cpp | app | 资产加载，仅 SceneLoader 调用 |
| BC 压缩（compress_texture 系列）+ mip 生成 | renderer/texture.h,cpp | app | 资产预处理，仅 SceneLoader 调用 |
| tangent 生成（generate_tangents） | renderer/mesh.h,cpp | app | 网格资产预处理，仅 SceneLoader 调用 |
| cache + ktx2 | renderer | app | 磁盘缓存，服务资产管线 |
| finalize_texture + create_default_textures | renderer/texture.h,cpp | optix | CUDA 资源创建，和 CudaBuffer/CudaTexture 同质 |
| TextureFormat / PreparedTexture / SamplerDesc | renderer/texture.h | optix | 随 finalize_texture 迁移，被 app（压缩端）和 optix（上传端）共用 |
| DefaultTextures / TextureRole | renderer/texture.h | app | 仅 app 层使用（SceneLoader + Application） |
| ImGuiBackend | renderer/imgui_backend.h,cpp | vulkan | 呈现基础设施（将 draw data 通过 Vulkan 管线送上屏幕），不含渲染逻辑 |

**CMake 变更**：
- app target 启用 CUDA language（后续 equirect_to_cubemap.cu 直接在 app 创建）
- app target 增加 bc7enc、ispc_texcomp、mikktspace、stb、xxHash 链接/include
- vulkan target 增加 imgui 链接（PUBLIC，renderer 通过依赖链继承）
- renderer target 移除 bc7enc、ispc_texcomp、mikktspace、stb、xxHash、imgui 依赖
- optix target 增加 `finalize_texture.cpp`（或将其纳入已有文件）

### Env Cubemap

加载流程：stb_image 加载 equirect HDR → CUDA kernel 转换为 6-face cubemap（face size = `min(bit_ceil(w/4), 4096)`）→ BC6H 压缩（ISPC CPU）→ `finalize_texture()` 上传 `cudaMipmappedArray`（cubemap flag）→ `cudaTextureObject_t`。Miss shader 用 `texCubemap<float4>` 采样。BC6H 压缩结果通过 KTX2 缓存，后续加载直接跳过解码和压缩阶段。

### UI 架构（D30）

**领域结构体**：

```cpp
// renderer 层，与 Camera 同级
struct RenderSettings {
    uint32_t max_bounces = 16;
    uint32_t samples_per_frame = 1;
    float exposure_ev = 0.0f;       // UI 展示 EV，Application 做 pow(2,ev) 转线性传 Renderer
    bool accumulation_enabled = true;
};

// renderer 层
struct SceneStats {
    uint32_t meshes = 0;
    uint32_t blas_groups = 0;
    uint32_t instances = 0;
    uint32_t tlas_instances = 0;
    uint32_t materials = 0;
    uint32_t textures = 0;
    uint32_t triangles = 0;
    uint32_t vertices = 0;
    uint32_t emissive_triangles = 0;
    uint32_t env_map_width = 0;
    uint32_t env_map_height = 0;
};
```

**所有权与数据流**：

- RenderSettings：Application 拥有，运行时 live 参数，不持久化。通过 SceneRenderInput 传入 Renderer（exposure 转线性）
- SceneStats：Application 在 load_scene 后统计并缓存
- DebugUIContext 新增 `RenderSettings&`（mutable）、`Camera&`（mutable，FOV）、`accumulated_samples`（只读）、`const SceneStats&`
- 参数变化检测：Renderer 自行比对前帧值（与 camera 检测同机制），chain_count=0。手动 Reset 走 DebugUIActions
- Deferred slider：slider_float_deferred + slider_angle_deferred + slider_uint_on_release，匿名命名空间工具函数。ImGui on-release 模式要求被编辑的中间值跨帧持久化：widget active 期间将拖拽值存入 `ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), display)`，释放帧从 StateStorage 取回最后拖拽值用于提交。局部变量每帧从 `*v` 重新初始化会导致释放帧丢失拖拽值（`SliderBehavior` 在 widget 非 active 时不应用拖拽位置），表现为滑块弹回原值

---

## Himalaya 参考对照

| Qualquer 模块 | Himalaya 源 | 适配说明 |
|---------------|------------|---------|
| BRDF specular（brdf.cuh） | `shaders/rt/pt_common.glsl` | GLSL → CUDA intrinsics |
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
- UI 参数可调（max_bounces, samples_per_frame, exposure, FOV）
- 4000 spp 下各类场景大致收敛
- 无 OptiX / CUDA 报错

---

## Phase 4.5：收敛质量 + DLSS-RR + 自适应

> 目标：DLSS-RR 集成（时域累积+去噪+放大）、采样质量提升、自适应帧率
>
> 任务清单见 `tasks/phase4.md` Step 9+。
> 决策记录见 `docs/phase4-discussion.md`（D31-D37）。

### 实现步骤

#### 依赖关系

```
Step 9（IBL 旋转 + RR）
Step 10（Sobol RNG）          ← 9 和 10 互相独立，按顺序实现但无依赖
Step 11（Render res decoupling）
     ↓
Step 12（Aux data）──→ Step 13（DLSS SDK 接入）──→ Step 14（DLSS 管线接入）──→ Step 15（自适应）

Step 16（Stochastic Alpha）
Step 17（Ray Cone LOD）       ← 16/17 独立于 DLSS-RR
Step 18（Normal Map Spec AA）← 依赖 Step 17
```

Step 9/10 互相独立，也独立于 DLSS-RR，按排定顺序先行。Step 11 是 Step 12 的前置（aux data 按渲染分辨率分配）。Step 12 → 13 → 14 → 15 是链式依赖。后半部分中 Step 16/17 独立于 DLSS-RR，Step 18 依赖 Step 17（需要 ray cone footprint）。

#### 总览

| Step | 主题 | 验证标准 |
|------|------|----------|
| 9 | IBL 旋转 + Russian Roulette | IBL 旋转可调，RR 减少 bounce 数 |
| 10 | Sobol + hash 去相关 RNG | 低 spp 下噪声更均匀，收敛更快 |
| 11 | Render resolution decoupling | 渲染分辨率可独立配置，画面正确缩放 |
| 12 | Aux data 写入 | 临时 tonemap 可视化验证各 aux buffer 内容正确 |
| 13 | DLSS-RR SDK 接入 | 初始化成功、feature 创建成功，渲染输出不变 |
| 14 | DLSS-RR 管线接入 | DLSS-RR 输出干净放大的画面 |
| 15 | 自适应 sample 数 | Mode 1/2/3 自动切换，帧率符合目标 |
| 16 | Stochastic Alpha | blend 材质正确半透明 |
| 17 | Ray Cone LOD | 高频纹理 aliasing 减少 |
| 18 | Normal Map Specular AA | 法线贴图 specular 闪烁减少 |

### 设计决策摘要

| 决策 | 选择 | 理由 |
|------|------|------|
| D31 降噪方案 | DLSS-RR（CUDA API） | 时域累积 + 去噪 + 放大三合一，跳过 OptiX Denoiser 中间站 |
| D32 Aux data | E_glossy 作 specular albedo | guide buffer 和 color 自洽优于匹配训练分布 |
| D33 采样策略 | Sobol + hash 去相关 | DLSS-RR 不推荐 blue noise，hash 去相关满足独立性要求 |
| D34 自适应帧率 | 三级降级（ping-pong/串行/极端） | 在吞吐和输入延迟之间选择最优工作点 |
| D35 Target sample count | 移除 | DLSS-RR 内部管理时域历史 |

### 技术要点

#### DLSS-RR 集成（CUDA API）

**初始化**：`NVSDK_NGX_CUDA_Init_with_ProjectID` → `NVSDK_NGX_CUDA_GetCapabilityParameters` → 驱动版本检查。

**Feature 创建**：
```cpp
NVSDK_NGX_DLSSD_Create_Params dlssParams{};
dlssParams.InDenoiseMode = NVSDK_NGX_DLSS_Denoise_Mode_DLUnified;
dlssParams.InWidth = render_width;
dlssParams.InHeight = render_height;
dlssParams.InTargetWidth = display_width;
dlssParams.InTargetHeight = display_height;
dlssParams.InPerfQualityValue = auto_quality;  // 根据实际放大比率自动选取最接近的档位
dlssParams.InFeatureCreateFlags = MVLowRes | IsHDR;
dlssParams.InUseHWDepth = NVSDK_NGX_DLSS_Depth_Type_Linear;
dlssParams.InRoughnessMode = NVSDK_NGX_DLSS_Roughness_Mode_Unpacked;
// 创建前设置，确保 release 时立即释放 VRAM（resize/分辨率变化时 recreate 不残留）
ngxParams->Set(NVSDK_NGX_Parameter_FreeMemOnReleaseFeature, 1);
```

渲染分辨率由 UI 滑块直接控制。Quality mode 自动选取：启动时用 `NGX_DLSSD_GET_OPTIMAL_SETTINGS` 查询每个 mode（DLAA / Quality / Balanced / Performance / UltraPerformance）的 optimal / min / max 渲染分辨率，缓存结果。用户调整滑块时，选取 optimal 最接近用户值的 mode；若用户值不在该 mode 的 [min, max] 内，比较相邻两个 mode 的 clamp 距离，选改变量更小的。用户值超出所有 mode 范围时，clamp 到最近端的 min/max；UI 提供 bypass 开关跳过此 clamp（允许超范围渲染分辨率）。render >= target 时选 DLAA。Render Preset 默认 E（Latest transformer model，质量最优）；SDK 推荐 Default（随 OTA 更新），UI 提供 Default/D/E 切换。

**每帧执行**：填充 `NVSDK_NGX_CUDA_DLSSD_Eval_Params`，调用 `NGX_CUDA_EVALUATE_DLSSD_EXT`。Jitter offset = `-(jx - 0.5)`, `-(jy - 0.5)`（取反：投影矩阵 jitter 方向与像素偏移方向相反，两个 NVIDIA 官方参考均取反）。InMVScale = 1.0。MVJittered flag 不设（Flag=0，MV 两端均用 unjittered VP 投影，只含几何运动，与 vk_gltf_renderer 一致）。InIndicatorInvertYAxis = 1（OptiX/CUDA Y-up 坐标系，与 optix-subd / Blender Cycles 一致）。

**矩阵传递**：pInWorldToViewMatrix / pInViewToClipMatrix 传 `glm::value_ptr(glm::transpose(mat4))`——GLM column-major 转置后按 row-major 读取即为原始矩阵（vk_gltf_renderer `dlss_wrapper.cpp:488-491` 做法，optix-subd 的 row-major `otk::Matrix4x4` 直接传等价）。`to_float4x4()` 仅用于 LaunchParams 供 device `mul()` 使用，不用于 DLSS 矩阵传递。

**前帧 VP 矩阵**：LaunchParams 传当前帧和前帧各一个 unjittered `VP = projection × view`（`float4x4`，row-major 供 device `mul()` 使用），MV 计算将 world pos 分别投影到两帧 VP 取差（vk_gltf_renderer `dlss_util.h:87-96` 做法）。host 端每帧末缓存当前帧 VP 作为下帧的 prevVP。

**Stream 分配与管线流程**（DLSS ON / OFF 双路径）：

| Stream | DLSS ON | DLSS OFF（fallback） |
|--------|---------|---------------------|
| compute_stream | raygen（单帧 noisy HDR → surf2Dwrite 到 write slot） | raygen（Separate Sum 累加 → surf2Dwrite 到 write slot） |
| display_stream | DLSS-RR eval（读 read slot tex → 写 dlss_output surf）→ tonemap（读 dlss_output tex, 1:1, 无除法 → 写 display surf）→ signal | tonemap（读 read slot tex, resampling + 除 count → 写 display surf）→ signal |

DLSS-RR 在 display_stream 上执行，与 compute_stream 的 raygen 并行。读的是上一帧的 noisy buffer（ping-pong），不 stall raygen。DLSS-RR feature 创建时传入 display_stream（`InCUStream`）。DLSS OFF 时 display_stream 不执行 DLSS-RR eval，tonemap 直接读 ping-pong 累积 buffer（与 Phase 4 行为一致，仅读写方式从线性指针改为 tex/surf）。

**中间 HDR buffer**：新增资源，输出分辨率，float4。DLSS-RR 输出目标，tonemap 输入源。同一 stream 上顺序执行无竞争。仅 DLSS ON 时使用。

**重建条件**：渲染分辨率或输出分辨率（窗口 resize）变化时 release + recreate feature。

**参考实现**：`D:\Github\optix-subd\denoiserDlss.cu`

#### Ping-pong Buffer 类型迁移

DLSS-RR CUDA API 的 `pInColor` 要求 `CUtexObject*`（指向 `cudaTextureObject_t` 的指针），`pInOutput` 要求 `CUsurfObject*`（指向 `cudaSurfaceObject_t` 的指针）。所有 aux input 同理。这是 DLSS CUDA API 的硬性要求（参考 optix-subd `denoiserDlss.cu`、Blender Cycles `denoiser_dlss.cpp`，均使用 `CUarray` + tex/surf objects）。

当前 ping-pong 累积 buffer 是 `CudaBuffer<float4>`（线性 CUDA 内存），没有 texture object，无法直接传给 DLSS。

**迁移**：`std::array<CudaBuffer<float4>, 2>` → `std::array<CudaArrayBuffer<float4>, 2>`。

- raygen 写：`accumulation_buffer[pixel]` → `surf2Dwrite(color, color_output, x * sizeof(float4), y)`
- raygen 读（Separate Sum，DLSS OFF）：`accumulation_buffer_read[pixel]` → `tex2D<float4>(color_input, x + 0.5f, y + 0.5f)`
- tonemap 读（DLSS OFF fallback）：线性指针 → `tex2D<float4>(color_input, ...)`
- DLSS 读：`CudaArrayBuffer::tex_object()` 传入 eval params 的 `pInColor`

**LaunchParams 变更**：

- `float4* accumulation_buffer` → `cudaSurfaceObject_t color_output`（raygen surf2Dwrite 写入）
- `const float4* accumulation_buffer_read` → `cudaTextureObject_t color_input`（DLSS OFF 时 Separate Sum 读旧值；DLSS ON 时不使用）
- 新增 `uint32_t dlss_enabled`：raygen 判断单帧输出（1）还是 Separate Sum 累加（0）

`sample_count` 保留：DLSS OFF 时 raygen 和 tonemap 仍依赖此字段。DLSS ON 时 raygen 忽略它。

#### Sobol + Hash 去相关

**Sobol 表存储**：`uint32_t sobol_directions[4096]` 嵌入 LaunchParams 结构体，数据直接在 `__constant__` memory 中。OptiX `optixLaunch` 将 device buffer 复制到 `__constant__` memory（GPU 内部操作），Sobol 查表获得专用 L1 constant cache 广播（全 warp 读同一地址 ~8 cycles，vs global memory 走 L2 ~200+ cycles）。结构体从 ~200 bytes 增长到 ~16.5 KB，占 64 KB 限制的 25%。host→device 上传开销（PCIe 4.0 下 ~640 ns）相比 raygen 时间可忽略。

```cuda
// xxhash32: 96-bit 输入 → 32-bit 输出，多维混合质量优于链式 PCG
// 移植自 nvpro_core2 nvshaders/random.h.slang（Cyan4973/xxHash）
__forceinline__ __device__ uint32_t xxhash32(uint32_t p0, uint32_t p1, uint32_t p2) {
    const uint32_t PRIME32_2 = 2246822519u, PRIME32_3 = 3266489917u;
    const uint32_t PRIME32_4 = 668265263u,  PRIME32_5 = 374761393u;
    uint32_t h32 = p2 + PRIME32_5 + p0 * PRIME32_3;
    h32 = PRIME32_4 * ((h32 << 17) | (h32 >> 15));
    h32 += p1 * PRIME32_3;
    h32 = PRIME32_4 * ((h32 << 17) | (h32 >> 15));
    h32 = PRIME32_2 * (h32 ^ (h32 >> 15));
    h32 = PRIME32_3 * (h32 ^ (h32 >> 13));
    return h32 ^ (h32 >> 16);
}

// dim 0-1 (jitter): sample_index 替换为 frame_index（D37 per-frame jitter）
// dim 2+  (BRDF/NEE): 仍用 sample_index
__forceinline__ __device__ float sobol_rng(uint32_t pixel_index, uint32_t sequence_index,
                                           uint32_t frame_index, uint32_t dimension) {
    if (dimension >= 128) {
        // fallback: xxhash32 多维混合（替代链式 PCG）
        uint32_t h = xxhash32(pixel_index, sequence_index, dimension);
        return float(h) / float(0xFFFFFFFFu);
    }
    uint32_t sobol_val = sobol_sample(dimension, sequence_index);
    // Cranley-Patterson rotation: additive shift preserves Sobol low-discrepancy
    uint32_t pixel_offset = pcg_hash(pixel_index * 0x1f1f1f1fu ^ dimension);
    // Golden-ratio temporal offset (2654435769 ≈ φ × 2^32)
    uint32_t temporal_offset = frame_index * 2654435769u;
    sobol_val += pixel_offset + temporal_offset;  // unsigned wraparound = mod 2^32
    return float(sobol_val) / float(0xFFFFFFFFu);
}
// 调用方：
//   jitter:    sobol_rng(pixel, frame_index, frame_index, 0/1)  — per-frame
//   BRDF/NEE:  sobol_rng(pixel, sample_index, frame_index, dim) — per-sample
```

**Cranley-Patterson rotation 使用加法而非 XOR**：加法位移（mod 2³²）保持 Sobol 序列的低差异性（discrepancy bound 不变），XOR 会破坏分层结构。Golden-ratio temporal offset 使用整数乘法 `frame_index * 2654435769u`（2654435769 = round(φ × 2³²)），这是 golden ratio 准随机序列在 [0, 2³²) 上的正确整数等价形式，比 `__float_as_uint(frame_index * φ)` 的 IEEE 754 位模式重解释在数学上更正确、在 GPU 上更高效（一条 `imul` vs float 乘法 + bit cast）。

维度分配基本不变：per-bounce base = 2 + bounce × 12。**dim 0-1（subpixel jitter）改为 per-frame**（D37：由 frame_index 驱动，帧内所有 sample 共享同一 jitter，跨帧变化保留 DLSS-RR 时域超分辨率）。dim 2+ 仍 per-sample。

#### Render Resolution Decoupling

渲染分辨率与 swapchain 解耦（D15 机制修正后的落地方案）：

**参数**：`RenderSettings::render_height`（UI 滑块 [240, 2160]，释放时生效——拖拽期间不触发 buffer 重分配；Ctrl+Click 手动输入可超范围）。`render_width = round(render_height × display_aspect)`，render_height 为原生高度（默认值）时恰等于 display_width。camera aspect 保持显示纵横比（render_width 按显示纵横比推导，拉伸映射不改变屏幕几何投影）。

**分辨率流向**：累积 buffer 分配、optixLaunch 维度、`LaunchParams::width/height`（含 raygen jitter 的 u/v 像素尺度）均为渲染分辨率；显示分辨率只有 tonemap 需要，走 kernel 参数，LaunchParams 无新增字段。

**按需重分配**：Renderer 记录已分配的渲染分辨率，submit_cuda 比对期望值，变化时同步双 stream → resize 累积 buffer → accum_counts = {0,0}（覆写模式 + tonemap 黑帧守卫，同原 resize 语义）。`Renderer::resize` 移除；窗口 resize 只重建显示 buffer——纵横比不变时渲染分辨率不变、累积不重置，纵横比变化时 render_width 变化自然触发重分配（inv_projection 变化同时触发累积 reset）。

**Tonemap 重采样**：kernel 以显示分辨率 launch，在线性 HDR（mean，除以 count 后）空间对渲染分辨率累积 buffer 重采样，再 exposure + tonemap 写全幅 display buffer。放大与缩小各用其标准算法，不强行统一：

- 放大（render < display）：Catmull-Rom 双三次（固定 4×4 taps），结果 clamp ≥ 0（HDR 高对比处防负 lobe 振铃产生负辐射度）
- 缩小（render > display）：footprint box 平均（每显示像素平均其覆盖的源像素，边缘按覆盖比例加权）——SSAA resolve 语义，无振铃，任意比例无欠采样
- render == display：1:1 直读

blit / record_vulkan 全幅不变，Vulkan 侧不感知渲染分辨率。DLSS-RR 关闭或不可用时此路径为长期 fallback（开启时 tonemap 读显示分辨率中间 HDR buffer，1:1）。

#### 自适应帧率策略

通过调整每帧 samples_per_frame 控制帧时间，使实际帧率稳定在目标帧率水平。呈现模式（MAILBOX/FIFO）独立于自适应策略。

**Mode 选择**：

| Mode | 架构 | 条件 | 目标帧率 |
|------|------|------|----------|
| 1 | ping-pong 并行 | 1spp 帧时间足够快 | min { n × refresh ≥ 150 } |
| 2 | 串行 | 1spp 无法满足 Mode 1 | refresh（< 150Hz）或 refresh/2（≥ 150Hz） |
| 3 | 串行 | Mode 2 也无法满足 | 反复减半，< 60fps 放开跑 |

**控制机制**：测量实际帧时间，根据目标帧率和帧时间余量调整 samples_per_frame。Mode 1 下 ping-pong 多一帧延迟需 ≥150fps 才不可感知；无法达到时退到 Mode 2 串行（消除额外帧延迟），目标降为刷新率。

#### DLSS-RR Aux Data 写入时机

| 数据 | 写入位置 | hit 像素 | miss (sky) 像素 | back-face pass-through 像素 |
|------|---------|---------|---------------|---------------------------|
| depth | closesthit / raygen | view-space Z：`dot(camera_forward, hitPos - camera_origin)` | infinity | infinity |
| diffuse albedo | closesthit / raygen | raw `base_color`（⚠ 见下方说明） | `pbr_neutral_tonemap(sky_color)`（HDR 压到 [0,1] 作去调制引导） | (0,0,0,0) |
| specular albedo | closesthit / raygen | E_glossy 逐通道 | (0,0,0,0)（天空无 specular 层） | (0,0,0,0) |
| normals | closesthit / raygen | shading normal (world space) | (0,0,0,0) | (0,0,0,0) |
| roughness | closesthit / raygen | linear roughness | 0 | 0 |
| specular hit distance | — | 不分配，DLSS-RR 传 nullptr（optional 输入，multi-spp 下无单一有意义值） | — | — |
| motion vectors | raygen | MV 两端均用 unjittered VP 投影（`vk_gltf_renderer` 做法），MV 只含几何运动，jitter 信息由 InJitterOffset 单独提供；hit 像素：world hit pos 以 w=1 齐次坐标投影到当前/前帧 unjittered VP；miss 像素：ray direction 以 w=0 齐次坐标投影 | 同左 | 同 hit（pass-through 表面 hit_distance > 0） |
| color | raygen | 多 spp 同 jitter 平均后的 noisy HDR | 天空 HDR 辐射度 | 正常路径累积（pass-through 后续 bounce 的贡献） |

**多 spp 与 aux data 一致性**（D37）：帧内所有 sample 共享同一 subpixel jitter（per-frame），primary ray 相同 → aux data 写一次即可。BRDF/NEE 维度仍 per-sample。跨帧 jitter 变化保留 DLSS-RR 时域超分辨率。

**⚠ Diffuse albedo 存疑**：SDK 仅说"the diffuse component of Reflectance material"，未明确是否预乘 (1-metallic)。vk_denoise_dlssrr 传 raw baseColor（参考 `primary.rgen:451`）。我们跟随此做法传 raw base_color。若出现与 albedo 相关的 DLSS-RR 伪影，优先排查此处，备选方案为 `base_color × (1 - metallic)`。

#### Aux Data 资源与分配

**CudaArrayBuffer\<T\>**（optix 层）：RAII 模板，管理一个 2D `cudaArray_t` + `cudaTextureObject_t`（DLSS 读取，point sample / clamp / unnormalized coords）+ `cudaSurfaceObject_t`（着色器 `surf2Dwrite` 写入）。支持 float / float2 / float4 元素类型。

**Aux input buffers**（渲染分辨率，6 个）：

| Buffer | 类型 | 元素 |
|--------|------|------|
| depth | `CudaArrayBuffer<float>` | R32F |
| motion_vectors | `CudaArrayBuffer<float2>` | RG32F |
| diffuse_albedo | `CudaArrayBuffer<float4>` | RGBA32F |
| specular_albedo | `CudaArrayBuffer<float4>` | RGBA32F |
| normals | `CudaArrayBuffer<float4>` | RGBA32F |
| roughness | `CudaArrayBuffer<float>` | R32F |

specular hit distance 不分配（optional 输入，DLSS-RR 传 nullptr，见 D32）。

**DLSS output buffer**（显示分辨率）：`CudaArrayBuffer<float4>`，DLSS-RR 写入 → tonemap 读取。跟随窗口 resize（不跟随渲染分辨率变化）。

**生命周期**：init 时分配，destroy 时释放。渲染分辨率变化时 aux input buffers 与 accum_buffers 一起 drain + resize；显示分辨率变化时 DLSS output buffer 单独 drain + resize。

**LaunchParams 传递**：aux buffers 通过 `cudaSurfaceObject_t` 传入 LaunchParams（closesthit / raygen 用 `surf2Dwrite` 写入）。MV 计算需要 `view_projection` + `prev_view_projection`（unjittered，row-major `float4x4`），host 端每帧末缓存当前 VP 为下帧 prevVP。`prev_view_projection_` 初始值为 identity；首帧 MV 因此无效，由首次 DLSS evaluate 的 `InReset=1` 丢弃。

**首次 evaluate gating**：DLSS evaluate 延迟一帧 gating（`dlss_active && prev_dlss_active`），首次 evaluate 传 `InReset=1`。作用：(1) 避免消费 enable 前的 Separate Sum 陈旧累积总和；(2) 丢弃首帧因 `prev_view_projection_` 为 identity 产生的错误 MV。普通相机移动不触发 reset。

**写入策略**：closesthit 在首 sample 的 bounce 0 写入 depth / diffuse albedo / specular albedo / normals / roughness（`sample_index == sample_count` 门控，D37 共享 jitter → 首 sample 写一次即可）。raygen 在 sample loop 后写入 MV（hit/miss 统一）+ miss 像素的其余 aux 默认值。

**单面 back-face pass-through**：primary ray 命中单面材质背面时，closesthit 在 aux 写入块之前 return（pass-through），bounce 0 的 aux 数据未写入。pass-through 表面不可见，其材质属性与像素最终颜色无对应关系，应在 pass-through return 前写入 sky 默认值（depth=inf, normal=0, roughness=0, diffuse albedo=0, specular albedo=0），语义为「此像素无有意义的表面信息」。

**BrdfParams::E_glossy_rgb**：`init_brdf_params` 已经逐通道计算 E_glossy 用于 diffuse weight，现存入 `BrdfParams` 供 closesthit 直接读取写入 specular albedo，避免重算。

### 完成标准

- DLSS-RR 输出干净、无明显伪影的放大画面
- 自适应 Mode 1/2/3 正确切换，帧率稳定
- Sobol 采样质量可见提升（低 spp 收敛更快）
- IBL 旋转、Russian Roulette、Stochastic Alpha 正确
- Ray Cone LOD 减少纹理 aliasing
- Normal Map Specular AA 减少法线贴图 specular 闪烁
- 无 OptiX / CUDA / DLSS 报错
