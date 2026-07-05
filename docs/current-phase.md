# 当前阶段

> 目标：核心 Path Tracer（BRDF、多 bounce、IBL、NEE+MIS、累积收敛。4000 spp 大致收敛）
> 任务清单见 `tasks/phase4.md`。
> 讨论记录见 `phase4-discussion.md`（D1-D26 决策）。

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

所有决策详见 `phase4-discussion.md`（D1-D26）。

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

### 光源

仅 IBL（环境贴图）+ emissive 三角形。不实现解析光源（glTF KHR_lights_punctual 的 directional / point / spot 不纳入）。

### Phase 4 / 4.5 边界

**Phase 4 包含**（4000 spp 收敛必需）：
- 多 bounce（Megakernel + SER）、BRDF 全集、NEE + MIS、Env cubemap
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
uint32_t frame_seed;
float exposure;

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
| frame_counter_ | 双 stream slot 索引（%2） | 永不 reset |
| sample_count_ | 总累积 sample 数 | camera/config 变化时 = 0 |
| frame_seed_ | RNG temporal scramble | 永不 reset |

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
    float luminance;  // 降采样后的 per-pixel luminance（不含 sin_theta）
};
```

构建：Vose's O(N)。采样：O(1)。

**Env alias table 细节**：
- 降采样到半分辨率（equirect_width/2 × equirect_height/2），减少 entry 数量
- 权重 = luminance × sin(theta)（equirect 投影面积校正）
- `env_pdf()` 使用 entry 中的 `luminance`（不含 sin_theta）计算 solid-angle PDF，确保与 alias table 采样分布一致
- alias table 宽高和 total_luminance 通过 LaunchParams 传递（不使用 SSBO header）

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

### Env Cubemap

加载流程：stb_image 加载 equirect HDR → CUDA kernel 转换为 6-face cubemap → `cudaMipmappedArray`（cubemap flag）→ `cudaTextureObject_t`。Miss shader 用 `texCubemap<float4>` 采样。

---

## Himalaya 参考对照

| Qualquer 模块 | Himalaya 源 | 适配说明 |
|---------------|------------|---------|
| BRDF（brdf.cuh） | `shaders/rt/pt_common.glsl` | GLSL → CUDA intrinsics |
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
- BRDF 正确（Cook-Torrance specular + Lambertian diffuse）
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
