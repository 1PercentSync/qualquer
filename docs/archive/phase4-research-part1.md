# Phase 4 研究 — Part 1：特性差异清单

> 临时讨论文档。列出 Himalaya PT 部分的完整特性集与 Qualquer 当前实现的差异，以及 Qualquer 自身的新增需求。
> 不做取舍判断，仅罗列供讨论。

---

## 1. 特性清单对照表

### 1.1 来自 Himalaya 的 PT 特性

| # | 特性 | Himalaya 实现 | Qualquer 当前状态 | 差异说明 |
|---|------|--------------|------------------|----------|
| 1 | **多 Bounce 路径追踪** | raygen 内 `trace_path()` 循环，configurable `max_bounces`（默认 16） | 仅 primary ray，单次 `optixTrace`，无 bounce 循环 | **缺失** — 核心缺口 |
| 2 | **Russian Roulette** | bounce ≥ 2 时启用；存活概率 = clamp(max_component(throughput), 0.05, 0.95)；存活后 throughput /= p | 无 | **缺失** |
| 3 | **PBR BRDF 评估** | Cook-Torrance specular (D_GGX + V_SmithGGX + F_Schlick) + Lambertian diffuse | 仅采样纹理值，无 BRDF 计算（ambient shading: baseColor × occlusion） | **缺失** |
| 4 | **Multi-lobe BRDF 采样** | Fresnel-weighted lobe selection：specular_probability(NdotV, F0) → 选 specular 或 diffuse lobe | 无 | **缺失** |
| 5 | **GGX VNDF 重要性采样** | Heitz 2018 — 在 tangent space 采 half vector，转换为 incident direction | 无 | **缺失** |
| 6 | **余弦加权半球采样** | 标准 `sample_cosine_hemisphere` 用于 diffuse lobe | 无 | **缺失** |
| 7 | **BRDF PDF 计算** | `pdf_ggx_vndf` + `pdf_cosine_hemisphere`，combined multi-lobe PDF 用于 MIS | 无 | **缺失** |
| 8 | **Subpixel Jitter** | Sobol dims 0-1 做像素内抖动 | 无抖动（固定 pixel center + 0.5） | **缺失** |
| 9 | **Sobol 准随机序列** | 128 维 Sobol direction numbers (Joe & Kuo)，SSBO 传入 GPU | 无 RNG 系统 | **缺失** |
| 10 | **Blue Noise 去相关** | 128×128 blue noise 纹理做 Cranley-Patterson rotation + golden-ratio temporal scramble | 无 | **缺失** |
| 11 | **PCG Hash 后备** | dim ≥ 128 时使用 PCG hash 做伪随机 | 无 | **缺失** |
| 12 | **Env Cubemap（环境光源）** | equirect HDR → cubemap 转换，miss shader 采样 cubemap 的原始辐射度 | 无 | **缺失** |
| 13 | **Env Map 重要性采样** | Alias table (Vose's O(N) 构建, O(1) 采样)，基于 luminance × sin(θ) 权重 | 无 | **缺失** |
| 14 | **Env Map MIS** | BRDF strategy + light(env) strategy 的 power heuristic (β=2) | 无 | **缺失** |
| 15 | **Emissive 三角形 NEE** | 扫描 emissive 材质的三角形 → world-space 数据 + alias table (power-weighted: luminance × area) | 无 | **缺失** |
| 16 | **Emissive MIS** | BRDF-sampled ray 命中 emissive 时，计算 power heuristic weight vs light sampling PDF | 无 | **缺失** |
| 17 | **Shadow Ray** | `traceRayEXT` with `TERMINATE_ON_FIRST_HIT | SKIP_CLOSEST_HIT`，shadow_miss 设置 visible=1 | 无 shadow ray | **缺失** |
| 18 | **Ray Origin Offset** | Wächter & Binder (RT Gems Ch.6) — integer bit manipulation on float representation | 无（使用 tmin=0.0f） | **缺失** |
| 19 | **Shading Normal Consistency** | `ensure_normal_consistency` — 反射 shading normal 到 geometric normal 半球 | 无 | **缺失** |
| 20 | **Ray Cone LOD** | Akenine-Möller et al. 2021 — cone_width + cone_spread propagation, texel density → LOD | 无（所有纹理采样 LOD 0） | **缺失** |
| 21 | **Ray Cone Curvature** | 从 face normal vs interp normal 估算表面曲率，更新 cone_spread | 无 | **缺失** |
| 22 | **Alpha Mask (any-hit)** | alpha_mode==1: texel alpha < alpha_cutoff → `ignoreIntersectionEXT` | 材质数据有 alpha_mode/alpha_cutoff 但 any-hit 为空，BLAS 标记全 DISABLE_ANYHIT | **数据已有，逻辑缺失** |
| 23 | **Stochastic Alpha (any-hit)** | alpha_mode==2: PCG hash 随机 vs texel alpha → 概率忽略 | 同上 | **数据已有，逻辑缺失** |
| 24 | **Double-sided pass-through** | 单面材质背面命中时，ray 穿越表面继续（offset origin forward，不终止） | closesthit 仅做 flip，无 pass-through | **缺失** |
| 25 | **Firefly Clamping** | indirect bounces (bounce > 0) 对 contribution 做 min(v, max_clamp) | 无 | **缺失** |
| 26 | **Running Average 累积** | imageStore + imageLoad 的增量 running average：mix(old, new, 1/(n+1)) | 有 ping-pong accum buffer 但当前每帧覆写（无累积逻辑） | **骨架已有，逻辑缺失** |
| 27 | **累积重置** | 相机/IBL/config 变化时 reset sample_count → 下帧直接覆写 | 无 | **缺失** |
| 28 | **降噪器** | OIDN (async, background thread, timeline semaphore sync) | 无（Phase 6 规划的是 OptiX Denoiser） | **缺失（技术路线不同）** |
| 29 | **Denoiser 辅助 Buffer** | bounce 0 时写 aux_albedo (diffuse_color) + aux_normal (N_shading) 到 storage image | 无 | **缺失** |
| 30 | **IBL 旋转** | Y 轴旋转（sin/cos in GlobalUBO），env 采样和 miss shader 均应用 | 无 | **缺失** |
| 31 | **Tonemapping** | HDR ACES (fullscreen fragment shader) | 有 CUDA tonemap kernel（当前为简单 clamp01） | **已有但实现不同** |
| 32 | **Target Sample Count** | 达到 target_samples 后停止累积 | 无 | **缺失** |
| 33 | **Orthonormal Basis Construction** | `build_orthonormal_basis(N, T, B)` 基于 N 与 Z 轴/X 轴交叉 | 无 | **缺失** |
| 34 | **indirect_intensity 乘数** | 全局间接光强度乘数（env color *= indirect_intensity） | 无 | **缺失** |
| 35 | **Normal Mapping** | TBN 矩阵构建 + BC5 RG 解码 + tangent 退化 guard | **已实现**（programs.cu get_shading_normal） | ✅ 已有 |
| 36 | **Vertex Interpolation** | 重心坐标插值 position/normal/uv/tangent | **已实现** | ✅ 已有 |
| 37 | **Back-face Normal Flip** | dot(N_face, ray_dir) > 0 → flip both normals | **已实现** | ✅ 已有 |
| 38 | **Material 数据结构** | GPUMaterialData 80 bytes (base_color, emissive, metallic, roughness, normal_scale, etc.) | **已实现** — 完全一致的布局 | ✅ 已有 |
| 39 | **GeometryInfo 数据结构** | 24 bytes (vertex_addr, index_addr, material_offset) | **已实现** — 完全一致 | ✅ 已有 |
| 40 | **Bindless 纹理** | sampler2D textures[] / cudaTextureObject_t array | **已实现** | ✅ 已有 |
| 41 | **BLAS/TLAS** | 加速结构构建 (with compaction in Qualquer) | **已实现** | ✅ 已有 |
| 42 | **Camera (inverse matrices)** | inv_view + inv_projection → primary ray | **已实现** | ✅ 已有 |
| 43 | **Empty Scene Guard** | traversable == 0 时跳过 trace | **已实现** | ✅ 已有 |
| 44 | **Anyhit LOD for Alpha** | any-hit 内用近似 ray cone（hit_distance × pixel_spread）计算 LOD | 无（any-hit 为空） | **缺失** |
| 45 | **NEE shadow ray LOD for emissive tex** | 对 NEE 的 emissive 光源纹理采样使用 ray cone LOD | 无 | **缺失** |

### 1.2 Qualquer 新增需求（Himalaya 没有的）

| # | 特性 | 说明 | Qualquer 当前状态 |
|---|------|------|------------------|
| Q1 | **每帧多 Sample** | raygen 内 sample 循环（N samples/frame），N 可变 | 无 — Himalaya 固定 1 spp/frame |
| Q2 | **自适应 Sample 数** | 基于帧率反馈动态调整 samples_per_frame（目标帧率 → 增减 N） | 无 |
| Q3 | **渲染分辨率解耦** | 渲染分辨率独立于呈现分辨率（accumulation buffer 大小 ≠ swapchain 大小） | 当前所有 buffer 跟随 swapchain extent |
| Q4 | **可调参数暴露** | max_bounces、render_resolution 等作为运行时参数暴露到 UI | 无参数系统 |

---

## 2. 按功能模块分组

### 2.1 核心路径追踪循环

| 特性 | 状态 |
|------|------|
| Multi-bounce loop | ❌ |
| **Multi-sample inner loop (N spp/frame)** | ❌ (新需求) |
| Russian Roulette | ❌ |
| Path throughput tracking | ❌ |
| Firefly clamping | ❌ |
| Early termination (negligible throughput) | ❌ |

### 2.2 BRDF / 材质着色

| 特性 | 状态 |
|------|------|
| D_GGX (Normal Distribution) | ❌ |
| V_SmithGGX (Visibility, height-correlated) | ❌ |
| F_Schlick (Fresnel) | ❌ |
| Lambertian diffuse (INV_PI) | ❌ |
| Multi-lobe selection (specular probability) | ❌ |
| GGX VNDF sampling | ❌ |
| Cosine hemisphere sampling | ❌ |
| Combined multi-lobe PDF | ❌ |
| Orthonormal basis from N | ❌ |

### 2.3 采样系统

| 特性 | 状态 |
|------|------|
| Sobol quasi-random (128-dim) | ❌ |
| Blue noise Cranley-Patterson rotation | ❌ |
| Golden-ratio temporal scramble | ❌ |
| PCG hash (fallback dim ≥ 128) | ❌ |
| Subpixel jitter | ❌ |
| Sobol dimension allocation scheme | ❌ |
| **Variable N per frame 的 sample_index 管理** | ❌ (新需求) |

### 2.4 光源与 NEE

| 特性 | 状态 |
|------|------|
| Env cubemap（equirect → cubemap, miss shader 采样原始辐射度） | ❌ |
| Env map alias table | ❌ |
| Env map importance sampling | ❌ |
| Env MIS (power heuristic) | ❌ |
| Emissive triangle 收集与 alias table | ❌ |
| Emissive NEE + MIS | ❌ |
| Shadow ray (visibility test) | ❌ |
| IBL rotation | ❌ |
| indirect_intensity multiplier | ❌ |

### 2.5 Ray 健壮性

| 特性 | 状态 |
|------|------|
| Ray origin offset (Wächter & Binder) | ❌ |
| Shading normal consistency | ❌ |
| Ray cone LOD (texture level selection) | ❌ |
| Ray cone propagation + curvature | ❌ |

### 2.6 Non-opaque 处理

| 特性 | 状态 |
|------|------|
| Alpha mask (any-hit discard) | ❌ (数据已有) |
| Stochastic alpha (blend mode) | ❌ (数据已有) |
| Double-sided pass-through (single-sided back-face) | ❌ |

### 2.7 累积与显示管线

| 特性 | 状态 |
|------|------|
| Running average accumulation（支持变量 N） | ❌ (骨架已有) |
| Accumulation reset on config/camera change | ❌ |
| Target sample count + auto-stop | ❌ |
| **自适应 sample 数（帧率反馈）** | ❌ (新需求) |
| **渲染分辨率与呈现分辨率解耦** | ❌ (新需求) |
| Denoiser (OptiX Denoiser vs OIDN) | ❌ |
| Aux albedo + normal output (bounce 0) | ❌ |

### 2.8 已实现（无需 Phase 4 工作）

| 特性 | 说明 |
|------|------|
| Normal mapping (TBN + BC5) | programs.cu `get_shading_normal` |
| Vertex interpolation (barycentric) | programs.cu closesthit |
| Back-face flip | programs.cu |
| Material struct (80B, PBR fields) | material.h |
| GPUGeometryInfo (24B) | material.h |
| Bindless texture array | cudaTextureObject_t[] |
| BLAS/TLAS (with compaction) | accel_structure.h/cpp |
| Primary ray from inv_view/inv_projection | programs.cu raygen |
| Interactive camera | camera_controller |
| Tonemap (CUDA kernel) | tonemap.cu |
| Ping-pong accumulation buffers | renderer.h |
| Dual-stream CUDA submission | renderer.cpp |

---

## 3. Himalaya 中非 PT 的特性（已剔除）

以下为 Himalaya 项目中存在但与 PT 渲染无关的功能，不纳入对比：

- Gaussian Splatting 渲染路径（全部 gs_* passes）
- Render Graph 系统（Qualquer 不需要）
- PLY 网格加载
- OIDN 特有的 CPU/GPU 设备选择逻辑（我们用 OptiX Denoiser）
- Vulkan RHI 封装层（我们有自己的架构）
- Cached shader compilation（SPIR-V 特有）
- BC6H 压缩 compute shader（纹理离线处理）
- **IBL Split-Sum 预计算**（irradiance cubemap、prefiltered cubemap、BRDF LUT）— 这些是光栅化管线的产物（Himalaya 主分支曾含光栅化/烘培管线，后削减）。PT 通过 env map sampling + 多 bounce 收敛来计算全局照明，不需要 split-sum 近似。PT 需要的仅是：(1) equirect → cubemap 转换（miss shader 采样原始环境辐射度）；(2) alias table（env importance sampling）

---

## 4. 备注

1. **IBL 在 PT 中的正确角色**：PT 的 miss shader 采样的是环境的**原始辐射度**（cubemap mip 0，未经 prefilter），靠累积 sample 收敛来获得正确结果。Irradiance cubemap / prefiltered cubemap / BRDF LUT 是 split-sum 近似，只服务于实时光栅化，PT 不应使用这些预计算产物。Himalaya 中之所以存在完整 IBL 预计算流水线，是因为其主分支历史上包含光栅化和烘培管线。

2. **Himalaya 的降噪辅助 buffer** 在 bounce 0 输出 diffuse_color（非 base_color）和 shading normal。这是 OIDN/OptiX Denoiser 标准做法。

3. **Himalaya 的 any-hit LOD 近似**：由于 any-hit 无法访问 payload（Vulkan RT 限制），它使用 `hit_distance * pixel_spread` 近似 cone width。OptiX 中 any-hit 同样无法读写 payload，但可以通过 `optixGetAttribute` 等机制传递有限数据，具体实现可能不同。

4. **Himalaya 的 pass-through 行为**：单面材质从背面命中时不吸收也不反射，而是让 ray "穿过" 表面继续行进。这消耗一次 bounce 但 throughput 不变。

5. **每帧多 Sample 的 RNG 影响**：Himalaya 固定 1 spp/frame，sample_index = frame 累计。我们需要 N spp/frame，sample_index = 跨帧全局累计值 + 帧内 local offset (0..N-1)。Sobol 低差异性依赖 sample_index 连续递增，这在变量 N 下仍然成立（每 sample 拿到唯一顺序 index）。

6. **Running average 公式与变量 N 的适配**：Himalaya 的 `mix(old, new, 1/(count+1))` 假设每次加 1 个 sample。变量 N 时：`mix(old_avg, frame_avg, N/(total+N))`，数学等价，只需传 N 和 total 到 shader。

7. **Ping-pong 与 running average 的兼容性**：已验证当前 ping-pong 架构自然兼容 running average。Raygen 读 buf[accum_index_]（旧值），写 buf[1-accum_index_]（新均值）。Tonemap 读 buf[accum_index_]。两个 stream 并行读同一 buffer 无冲突（均为读），写不同 buffer（仅 compute 写）。翻转后下帧正确。

8. **BRDF 采样权重的 cos/INV_PI 简化**（已验证：closesthit.rchit:407-412）：Himalaya 的 diffuse lobe 中 `throughput_update = (1.0 - F_d) * diffuse_color / (1.0 - p_spec)`。推导：BRDF = `(1-F) * diffuse_color * INV_PI`，PDF = `cos(θ) * INV_PI`，标准 weight = BRDF × cos / PDF = `(1-F) * diffuse_color`（cos 和 INV_PI 互消），再除以 lobe probability `(1-p_spec)`。这个简化减少计算且避免了 cos≈0 处的数值问题。Specular lobe 的权重则是 `(D * Vis * F * NdotL) / (pdf * p_spec)`——VNDF 的 PDF 设计使 G1 项在某些推导下可对消，但 Himalaya 保留了完整形式。

9. **Specular 采样失败路径终止**（已验证：closesthit.rchit:371-378）：当 VNDF 采样出的反射方向在表面以下（`L_ts.z <= 0`），closesthit 终止路径：设 `throughput_update = vec3(0)`、`hit_distance = -1`、`env_mis_weight = 1.0` 后 return。设 `hit_distance = -1` 使 raygen bounce loop 将其视为 miss 并 break。`env_mis_weight = 1.0` 确保该 bounce 的 `emissive + nee_radiance` 贡献在 raygen 的 `contribution *= env_mis_weight` 中不被缩减。路径虽终止但已计算的 emissive + NEE 结果保留。

10. **NEE 中的 double-sided emissive 处理**（已验证：closesthit.rchit:271-277）：emissive NEE 采样时查光源三角形材质的 `double_sided`。如果光源面背对着色点（`cos_theta_light < 0`）但 `light_mat.double_sided == 1u`，翻转 cos_theta_light 后仍计算 NEE 贡献。注意这里读取的是**光源**的材质双面属性，不是当前着色点的材质。

11. **Env MIS weight 的预计算时机**（已验证：closesthit.rchit:418-421 + pt_common.glsl:843-845）：closesthit 在 BRDF 采样确定 `next_direction` 之后即计算 `env_mis_weight = mis_power_heuristic(brdf_pdf_combined, env_pdf(next_dir))`，写入 payload。关键流程：如果下一次 trace miss → miss shader 只设 `payload.color = env_color` + `hit_distance = -1` → **raygen** 的 trace_path 中检测到 `hit_distance < 0` 时执行 `contribution *= payload.env_mis_weight`。所以 weight 在 closesthit 计算，在 raygen 应用——miss shader 本身不使用此值。

12. **tex2D vs tex2DLod — mipmap 已就绪**（已验证：texture.cpp）：当前 closesthit 用 `tex2D<float4>()` 采样（隐式 LOD 0）。切换到 `tex2DLod<float4>(tex, u, v, lod)` 的前置条件已全部满足：(1) `generate_cpu_mip_chain()` via stbir 生成完整 mip chain ✅；(2) `cudaMallocMipmappedArray` 创建 mipmapped array ✅；(3) `mipmapFilterMode = cudaFilterModeLinear` ✅；(4) `maxMipmapLevelClamp` 正确设置 ✅。Phase 4 若含 ray cone LOD 只需计算 LOD 值，纹理侧零修改。若不做 ray cone LOD 则保持 `tex2D` 不变。

13. **Himalaya Sobol 维度精确分配方案**（已验证：pt_common.glsl:348-350 + closesthit.rchit dim 使用处）：`DIMS_PER_BOUNCE = 12`。dim 0-1 = subpixel jitter (raygen)；per-bounce base = `2 + bounce × 12`：+0 lobe_select, +1/+2 brdf_xi (2D), +3 russian_roulette, +4..+7 env_nee (4D), +8..+11 emissive_nee (4D)。128 dims → 前 10 个 bounce 全覆盖（`2 + 10×12 = 122 < 128`），bounce 11+ 的部分维度降级为 PCG hash。

14. **Qualquer 已有的基础设施优势**（对比 Himalaya）：(a) **双 CUDA stream**（compute + display 并行）— Himalaya 是单线程 Vulkan submit；(b) **Ping-pong accumulation buffer** 避免 read-after-write 对同一 buffer 的依赖 — Himalaya 用单个 storage image 的 imageLoad/imageStore 读写同一 image（reference_view.rgen:51-59）；(c) **BLAS compaction** 两者都有；(d) **Binary interop semaphore** 的 CUDA↔VK 同步机制比 Himalaya 的纯 Vulkan timeline semaphore 更底层但对异构计算更灵活。
