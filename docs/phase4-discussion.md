# Phase 4 讨论议程

> 跟踪文档。讨论过程中对每个问题做出决策后标记 ✅ 并记录决策结论。
> 因决策而失去讨论意义的问题标记 ~~删除线~~。新衍生问题追加到相应位置。
>
> **关于决策文字与 Phase 边界**：各 D 决策文字是在讨论当时记录的早期决策（“纳入 Phase 4”）。后续 Phase 划分（见末尾 §Phase 划分）将部分特性调整到 Phase 4.5 推迟实现。特性最终归属以 §Phase 划分为准，决策文字的“纳入”不必然意味着 Phase 4 实现——若两者冲突，以 Phase 边界为准。

---

## 第一层：架构根基（影响所有下游决策）

### D1. Kernel 架构：Megakernel vs Multi-launch vs Wavefront vs Hybrid

**核心问题**：raygen 内 bounce loop（Megakernel）还是每 bounce 一次 optixLaunch（Multi-launch）还是 OptiX 仅做 traversal + CUDA 做 shading（Wavefront）？

**级联影响**：
- 决定 closesthit 的职责边界（全部着色 vs 仅写 hit data）
- 决定是否需要全局 ray state buffer
- 决定 payload 设计（closesthit→raygen 传什么）
- 影响 M2 ReSTIR 的架构兼容性
- SER（Shader Execution Reordering）的收益依赖 megakernel

**选项摘要**：
- **A. Megakernel**：最简单，Himalaya 的做法，SER 友好，closesthit 做全部着色
- **B. Multi-launch**：每 bounce 一次 launch，bounce 间可插入逻辑（ReSTIR 友好），需要全局 ray state buffer
- **C. Wavefront**：OptiX 仅 traversal，CUDA kernel 着色。最大 occupancy，实现最复杂
- **D. Hybrid**：Phase 4 用 Megakernel，设计上为未来拆分预留空间

**决策**：✅ **Megakernel + SER**。演进路径：需要 ReSTIR 等全局同步时迁移到 Multi-launch；体积渲染时再评估 Wavefront。

---

### D2. Payload 设计

**前置依赖**：D1（kernel 架构决定 closesthit 输出什么）

**核心问题**：closesthit → raygen 的数据传递走 OptiX payload registers 还是 global buffer？

**选项摘要**：
- **A. 全信息 Payload**（18+ registers）：color, next_origin, next_direction, throughput_update, hit_distance, bounce, env_mis_weight, last_brdf_pdf, sample_index — 全走 registers（Phase 4.5 ray cone 扩展至 19）
- **B. Minimal Payload + Global Buffer**（1 register pixel_index）：closesthit 读写全局 buffer
- **C. Hybrid**：热路径数据走 payload（~10 regs），冷数据走 global

**关键约束**：OptiX 上限 32 registers；SER 对 payload-based 有优化

**决策**：✅ **A. 全信息 Payload（~18 registers）**。SER 重排后 payload 随线程移动、零全局内存访问；B/C 的 global buffer 在 SER 重排后访问变为 non-coalesced，反而更差。

---

### D3. Device 代码组织

**核心问题**：programs.cu 即将从 307 行膨胀到 1500+ 行，如何组织？

**选项摘要**：
- **A. 单 .cu + 多 .cuh 头文件**：保持一个 module，`__forceinline__ __device__` 函数拆入 brdf.cuh / rng.cuh / ray_utils.cuh / nee.cuh 等。编译器全局优化。构建流程零改动
- **B. 多 .cu → 多 Module → OptiX Module Linking**：增量编译，但实现复杂度高，跨 module 调用受限

**决策**：✅ **A. 单 .cu + 多 .cuh**。1500 行量级编译不构成瓶颈；全局内联对 megakernel 性能关键；构建流程零改动。

---

## 第二层：核心管线结构（影响多个特性的实现方式）

### D4. 每帧多 Sample 的执行策略

**核心问题**：N samples/frame 如何执行？

**选项摘要**：
- **A. Raygen 内 sample 循环**：1 次 optixLaunch，raygen 内 for(s=0..N-1)，寄存器内局部累积。带宽最优
- **B. 多次 optixLaunch**（每 sample 一次）：改动最小，但 launch overhead × N，每次都读写全局 accum buffer
- **C. 3D launch**（width × height × N）：需原子操作或 per-sample 临时 buffer

**关键考量**：TDR 安全（Windows 2s 限制），寄存器压力（A 只多 3 floats + 1 counter）

**决策**：✅ **A. Raygen 内 sample 循环**。单次 launch，寄存器内局部累积，每帧 1 读 + 1 写 accum buffer。硬上限 max_samples_per_frame=64（TDR 安全）。

---

### D5. 累积策略

**前置依赖**：D4（多 sample 策略决定累积公式）

**子问题**：
1. **Running Average vs Separate Accumulator**：`mix(old, frame_avg, N/(total+N))` vs `sum[pixel] += total; count[pixel] += N; display = sum/count`
2. **累积重置触发条件**：camera 变化、PT 参数变化、渲染分辨率变化、场景切换
3. **检测方式**：逐帧比较 inv_view + inv_projection（exact float compare）
4. **frame_counter_ / sample_count_ 语义拆分**：当前 frame_counter_ 混用了 slot 索引和 frame index

**注意**：未来 DLSS Ray Reconstruction 可在 1 spp 下直接产出高质量画面，届时实时预览模式可能不走累积管线。但 Phase 4 的累积系统仍然是物理收敛渲染的基础，两者互补而非替代。

**决策**：✅
1. **Separate Sum 累积**：`sum_new = sum_old + frame_total`，tonemap 中 `display = sum / count`。精度略优于 running average，与现有 ping-pong 架构兼容。
2. **Per-slot 计数**：每个 ping-pong buffer 各自记录 sample 数（`accum_counts_[2]`），sum 和 count 作为配对数据一起走。tonemap 的除数永远与所读 buffer 的内容匹配。（原方案使用全局 `sample_count_` + 清零两 buffer，实现后发现 reset 帧 clear 与同帧 tonemap 读同一 buffer 存在跨 stream 数据竞争且导致黑帧，按决策可更新性原则改为 per-slot 计数 + 覆写模式。）
3. **累积重置**：camera/PT 参数/渲染分辨率/场景切换时 `chain_count` 归零 → raygen 覆写 write buffer（不读 read buffer），等效清零但无 memset 和跨 stream 竞争。tonemap 同帧仍读 read buffer（count 有效），显示上一帧画面。
4. **语义拆分**：`frame_counter_`（帧号，永不 reset——%2 选 slot，同时上传为 LaunchParams::frame_index 作 device temporal/RNG 源）、`accum_counts_[2]`（per-slot sample 数，帧末更新 write slot）。不单设 `frame_seed_`：frame_counter_ 本身单调递增、永不 reset，复用作 RNG temporal scramble 源即可。

---

### D6. SBT / Pipeline 配置

**核心问题**：Phase 4 的 OptiX pipeline 需要几个 miss program、几个 hit group？

**当前状态**：1 raygen + 1 miss + 1 hitgroup（DISABLE_ANYHIT），SBT stride=0

**Phase 4 需要**：
- 至少 2 个 miss：`__miss__env`（环境光）+ `__miss__shadow`（设 visible=1）
- 是否需要 2 个 hit group（opaque vs non-opaque）取决于 alpha mask 是否在 Phase 4 实现
- stride=0（单 hitgroup，BLAS flag 控制 anyhit）vs stride=1+（多 hitgroup，SBT 选择）

**级联影响**：
- 影响 BLAS 构建（per-geometry opaque flag）
- 影响 shadow ray 对 alpha 物体的行为
- 数据链已完整：`BLASGeometry::opaque` 字段已存在，Material 有 `alpha_mode`

**结论**：由 D11 决策直接推导，无独立决策空间。2 miss + 1 hitgroup，stride=0。
- Miss：`__miss__env`（missIndex=0）+ `__miss__shadow`（missIndex=1）
- Hitgroup：closesthit + anyhit（单 program group），BLAS per-geometry flag 控制 anyhit 调用
- opaque 几何体：`OPTIX_GEOMETRY_FLAG_DISABLE_ANYHIT`（硬件跳过 anyhit）
- non-opaque 几何体：`OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL`（硬件调用 anyhit）
- Shadow ray 通过 ray flag `DISABLE_CLOSESTHIT` 跳过 closesthit，与 bounce ray 共享 hitgroup
- BLAS 构建改动：`BLASGeometry::opaque` 从硬编码 `true` 改为按 `material.alpha_mode` 判断

**方案演化**：早期设计为 2 hitgroup（opaque 无 anyhit / non-opaque 有 anyhit）+ stride=1 + per-geometry SBT records。实现前评估发现：（1）OptiX closesthit 和 anyhit 是独立入口点、分别分配寄存器，opaque hitgroup 不链接 anyhit 无寄存器收益；（2）SER hint 已用 material_buffer_offset（16 bits），多 hitgroup 不提供额外 SER 信息；（3）BLAS per-geometry flag 是 OptiX 控制 anyhit 调用的原生机制，1 hitgroup + geometry flag 在架构和性能上均优于 2 hitgroup + SBT 选择。

---

### D7. RNG 架构

**核心问题**：随机数生成系统的选择和存储方式。

**RNG 方案**：
- **A. Sobol + Blue Noise**（Himalaya 做法）：128 维低差异序列 + 空间去相关，业界标准
- **B. PCG per-pixel**：最简单，但白噪声收敛慢
- **C. Owen-scrambled Sobol**：理论最优去相关，实现稍复杂

**Sobol Table 存储**：
- **A. `__constant__` memory**（16KB，与 LaunchParams 共享 64KB 限制）
- **B. LaunchParams 指针引用 global memory**（指针 8 bytes，数据走 L2 cache）
- **C. 嵌入 LaunchParams 结构体**（每帧 upload 16KB）

**Blue Noise 存储**：
- **A. CUDA Texture Object**（走 texture cache，硬件归一化）
- **B. Global memory buffer**（手动归一化，但 16KB 几乎全在 L2）

**决策**：✅
1. **Sobol + Blue Noise**：128 维低差异序列 + Cranley-Patterson rotation + golden-ratio temporal scramble，dim ≥ 128 降级 PCG hash。
2. **Sobol Table → `__constant__` memory**：warp 内同维度查询命中 constant cache 广播，16KB 远低于 64KB 限制。
3. **Blue Noise → CUDA Texture Object**：`tex2D<float>` 硬件归一化，走 texture cache，与已有纹理管线一致。

---

## 第三层：渲染特性（大多互相独立）

### D8. BRDF 实现范围

**核心问题**：Phase 4 实现哪些 BRDF 组件？

**Himalaya 的完整集**：
- D_GGX（Normal Distribution）
- V_SmithGGXCorrelated（Visibility，height-correlated，Heitz 2014）
- F_Schlick（Fresnel）
- ~~Lambertian diffuse（INV_PI）~~ → EON diffuse（D28a）
- Multi-lobe selection（specular_probability）
- GGX VNDF sampling（Heitz 2018）
- Cosine hemisphere sampling
- Combined multi-lobe PDF
- Orthonormal basis construction

**讨论点**：
- 目标是"完整 PT 但不做性能优化"——上述全集是否都算"完整 PT 的基本组件"而非"优化"？
- VNDF sampling 和 cosine hemisphere 是 importance sampling，但它们属于"让 PT 正确/可用的基本采样"还是"性能优化"？
- 如果不做 importance sampling，uniform hemisphere sampling 是否能在合理时间内收敛？

**决策**：✅ **全做**。上述全集是完整 PT 的最小 BRDF 集，不是优化。Cosine hemisphere / VNDF / multi-lobe selection 是让 PT 在可行时间内产出可辨识结果的基本采样策略——没有它们 specular 材质实际不可用。Diffuse 模型由 Lambertian 升级为 EON（D28a）。

**参数约定（alpha vs roughness）**：specular 微面元原语（D_GGX、V_SmithGGXCorrelated、sample_ggx_vndf、pdf_ggx_vndf）接收 **alpha**（= roughness²），贴合 Heitz 2018 原文与微面元参数惯例；EON diffuse / E_ss / E_FON / CLTC 拟合接收 **linear roughness r ∈ [0,1]**，贴合多项式拟合变量。两套惯例源于不同数学源头无法统一。closesthit 端同时持有 roughness 和 alpha，alpha 在单点 clamp（D28 NaN 防护），specular 原语内部不重复 clamp。与 Himalaya（全收 roughness、函数内平方）不同——Qualquer 集中算 alpha 避免函数内重复平方、让 clamp 可见。

**命名（V_SmithGGXCorrelated）**：Qualquer 用 `V_SmithGGXCorrelated` 而非 Himalaya 的 `V_SmithGGX`，显式标注 height-correlated（Heitz 2014）实现。理由：诚实标注实现形态（"GGX" 只描述 NDF，不描述 G2 的 correlated/separable 选择），并为日后对比 separable 版本预留无歧义命名空间。

**combined_lobe_pdf 独立函数**：combined multi-lobe PDF 的组装 `p_spec·pdf_spec + (1−p_spec)·pdf_diff` 抽成 `__forceinline__ __device__ combined_lobe_pdf(pdf_spec, pdf_diff, p_spec)` 独立函数，而非 Himalaya 的 closesthit 内联组装。理由：Qualquer 拆成 brdf.cuh + closesthit + NEE eval 多处调用点（brdf_sample 两分支 + 两个 NEE eval，共 4 处），独立函数让 combined PDF 与 lobe 选择概率的一致性在代码上显而易见（4 处调用同一函数，不可能漏改某处破坏 MIS 无偏性）；`__forceinline__` 零运行时开销；轻量抽象（3 参数、一行公式）不违反 KISS。

**实现查验义务**：以下实现细节文档未预先记录，实现时必须实际查验参考项目源码，不得凭推导、印象或现有文档内容臆测：
- E_glossy / Turquin 补偿系数对金属 RGB F0 的逐通道处理方式（查 Sforza repo 拟合代码 + Himalaya closesthit 的 F0 vec3 调用点）
- EON 公式中 ρ（diffuse 反照率）的取值来源（查 EON repo GLSL 实现）
- CLTC 采样的 PDF 求值公式（查 EON repo GLSL 实现，与 combined_lobe_pdf 的 pdf_diff 一致）

**brdf_eval / brdf_sample 接口 + BrdfParams**：multi-lobe BRDF 求值与采样抽成独立接口函数 `brdf_eval` / `brdf_sample`，着色点不变量打包为 `BrdfParams` 结构体、由 `init_brdf_params` 构造入口一次性填充，替代 Himalaya 的 closesthit 内联。理由：与 combined_lobe_pdf 同理，eval/sample 在 brdf_sample 两分支 + 两处 NEE eval 共 4 处复用，接口化保证 BRDF 求值一致；`init_brdf_params` 集中 D27/D28c 逐通道补偿计算（turquin_comp 逐通道调 turquin_compensation 共用标量 E_ss、E_glossy 逐通道用混合 F0、diffuse_weight 双重衰减），避免散到 closesthit；`__forceinline__` 零运行时开销。`build_orthonormal_basis`、`specular_probability` 同样移植为独立函数。VNDF 采样产生下半球方向（L_ts.z ≤ 0）时 brdf_sample 返回 pdf=0/throughput_update=0 表示无效，path 终止逻辑由 closesthit 处理。

---

### D9. 光源系统：环境光 + Emissive 三角形

**子问题**：
1. **Env Map 表示**（D12 单独讨论）
2. **Env Map 重要性采样**：alias table（O(1) 采样）vs uniform sampling
3. **Emissive 三角形 NEE**：收集 emissive 三角形 + alias table + NEE 采样
4. **MIS**（BRDF strategy + light strategy 的 power heuristic）

**讨论点**：
- NEE 和 MIS 是"性能优化"还是"正确 PT 的必要组件"？
  - 纯 BRDF 采样（无 NEE）在小光源场景下极难收敛（小灯泡、点光源效果）
  - 但目标说"不做性能优化"
- 可能的分界线：NEE = 让 PT 在合理时间内产出可接受结果的必要机制；importance sampling = 加速收敛的优化

**决策**：✅ **全做**。Env importance sampling（alias table）、emissive 三角形 NEE、MIS（power heuristic）、shadow ray。与 D8 同理——不是优化，而是让 PT 在小光源/HDR 环境下产出可用结果的必要机制。

**光源策略**：仅 IBL（环境贴图）+ emissive 三角形。不实现任何解析光源（glTF KHR_lights_punctual 的 directional / point / spot 不纳入）。所有照明均通过 env miss shader 和 emissive 表面 NEE 提供。

**Env alias table 分辨率**：✅ **全分辨率**（equirect_width × equirect_height）。调研了 nvpro_core2/vk_gltf_renderer（NVIDIA 官方样例）、teofum/platinum、harskish/fluctus 等使用 alias table 的 GPU 路径追踪器，除 Himalaya 外均采用全分辨率。Himalaya 的 ÷2 是出于 VRAM 预算的工程取舍，非业界标准。全分辨率保持角度精度与 PDF 评估一致性；若实测发现 hot pixel 导致采样过度集中，再考虑降采样（÷2 box filter）作为缓解手段。

---

### D10. Shadow Ray 策略

**前置依赖**：D9（有 NEE 才需要 shadow ray），D6（SBT 配置影响 shadow ray 走法）

**选项**：
- **A. Standard**：`TERMINATE_ON_FIRST_HIT | DISABLE_CLOSESTHIT`，miss 设 visible=1
- **B. With Any-hit**：不用 TERMINATE，anyhit 做 alpha test（透明阴影）

**关键细节**：
- Ray origin offset（Wächter & Binder）是 shadow ray 和 bounce ray 的前置需求
- Emissive shadow ray 的 tMax = `dist * (1 - 1e-4)`

**结论**：由 D9 + D11 直接推导。B（with any-hit）——shadow ray 经过 non-opaque 几何体时走 anyhit 做 alpha test，opaque 几何体仍然 TERMINATE_ON_FIRST_HIT。

---

### D11. Alpha 处理（Mask + Stochastic + Pass-through）

**前置依赖**：D6（SBT 配置），D10（shadow ray 是否走 anyhit）

**三个相关特性**：
1. **Alpha Mask（any-hit）**：alpha_mode==1，texel alpha < cutoff → ignoreIntersection。数据已就绪
2. **Stochastic Alpha（any-hit）**：alpha_mode==2，随机 vs texel alpha。需要 RNG（D7）
3. **Double-sided pass-through**：单面材质背面命中时穿越表面继续行进

**讨论点**：
- 这三个是 Phase 4 的范围内还是可以推迟？
- Alpha mask 是许多 glTF 场景（树叶、栅栏等）的基础正确性需求
- Stochastic alpha 更偏优化/高级特性

**决策**：✅ **三个全做**。
1. **Alpha Mask**：any-hit 中 `texel alpha < cutoff → optixIgnoreIntersection()`。
2. **Stochastic Alpha**：any-hit 中 `hash(pixel, sample, primitive_id) > alpha → optixIgnoreIntersection()`。用 hash 而非 Sobol——any-hit 调用次数和顺序不确定，Sobol 的维度分配模式不适用。
3. **Double-sided pass-through**：closesthit 中判断 back-face + !double_sided → 穿透（throughput 不变，消耗一次 bounce）。

---

### D12. Env Map 表示

**核心问题**：miss shader 采样环境光用 cubemap 还是 equirect 2D 纹理？

**选项**：
- **A. Cubemap**（Himalaya）：equirect → cubemap CUDA kernel 转换，`texCubemap` 采样，无极地畸变
- **B. Equirect 直接采样**：跳过转换步骤，但极地 sampling 畸变，filtering 质量不如 cubemap

**关键考量**：alias table 从 equirect 像素构建（与采样方式无关）；环境旋转两种方式都是对 ray direction 做 Y 轴旋转

**决策**：✅ **A. Cubemap**。`texCubemap` 硬件选面零数学开销、无极地畸变、边界无缝 filtering。代价仅为一次性 equirect→cubemap 转换 kernel + ~1.5× 显存。

---

### D13. 纹理 LOD（Ray Cone）

**核心问题**：Phase 4 是否实现 ray cone LOD？

**选项**：
- **A. Ray Cone**：跟踪 cone_width + cone_spread（2 floats in payload），物理准确
- **B. 固定 LOD 0**：最简单，但高频纹理 aliasing
- **C. Hit distance heuristic**：不跟踪 per-ray state，bounce 后不准确

**关键考量**：
- mipmap 基础设施完全就绪（`tex2DLod` 可直接使用）
- Ray cone 减少 texture aliasing noise → 每个 sample 质量更高 → 自适应 spp 更高效
- Phase 4.5 实现时使用 p16（reserved）+ p18（numPayloadValues = 19）

**决策**：✅ **A. Ray Cone**。mipmap 基础设施就绪，`tex2DLod` 直接可用。减少高频纹理 aliasing → 每 sample 质量更高。

**Payload register 计划**：p15 = sample_index（raygen sample loop 传递给 closesthit RNG），p16 reserved。Phase 4.5 实现 ray cone 时：p16 → cone_width，p18 → cone_spread（numPayloadValues = 19）。p15 保留不动。

---

### D14. Ray 健壮性

**非可选项（标记为参考）**：
- **Ray origin offset（Wächter & Binder）**：bounce ray 和 shadow ray 的自交避免。PT 的硬性需求
- **Shading normal consistency**（reflect shading normal to geometric normal hemisphere）：防止"黑色像素"问题

**讨论点**：
- 这两个应该没有争议——都是正确 PT 的必要组件。仅确认纳入 Phase 4

**决策**：✅ **全部纳入**。两者均为正确 PT 的硬性前置需求，无选项空间。

---

## 第四层：外围系统

### D15. 渲染分辨率与呈现分辨率解耦

**核心问题**：accumulation buffer 大小是否独立于 swapchain？

**选项**：
- **A. Display buffer 保持 swapchain 大小**：tonemap 写入子区域，VK blit 做上采样。InteropImage 不因渲染分辨率变化重建
- **B. Display buffer 跟随渲染分辨率**：精确内存，但重建 InteropImage 代价高

**关键考量**：
- 如果 Phase 4 暴露 `render_scale` 参数但不做自适应，用户手动调整 → 低频变化 → A 和 B 都可行
- A 为未来自适应渲染分辨率（每帧变化）预留了路
- 8 个紧耦合点需要改造（Part 2 §4.3 已列出）

**决策**：✅ **A. Display buffer 保持 swapchain 大小**。tonemap 写子区域，blit 上采样。InteropImage 不因渲染分辨率变化重建，为未来自适应分辨率 / DLSS-RR 预留路径。显存浪费可忽略。

---

### D16. 自适应 Sample 数

**子问题**：
1. **帧时间测量**：CUDA timing events vs CPU wall clock vs 混合
2. **控制策略**：比例控制 vs EWMA 平滑 vs 迟滞 vs 增量控制
3. **安全上限**：TDR 保护（max_samples_per_frame）

**讨论点**：
- Phase 4 是否实现自适应，还是仅暴露手动 `samples_per_frame` 参数？
- 自适应逻辑本身不复杂，但帧时间测量需要新增 timing events

**注意**：未来 DLSS-RR 在 1 spp 实时预览模式下，自适应 sample 数不再关键（固定 1 spp）。但对物理收敛渲染模式仍然有价值。

**决策**：✅ **A. 仅手动参数**。UI 暴露 `samples_per_frame`（1~64）。自适应是 UX 便利非渲染正确性，PT 核心完成后可随时加入。

---

### D17. Tonemap

**当前状态**：`clamp01(hdr)` — 不是真正的 tonemap

**需要讨论**：
1. **Tonemap 函数**：ACES / Reinhard / AgX / 可运行时切换？
2. **Exposure 控制**：固定值 / auto-exposure / UI 可调？
3. **与渲染分辨率解耦的交互**：tonemap kernel 处理 render_width × render_height 子区域

**决策**：✅
1. **Tonemap 函数**：Khronos PBR Neutral。专为 PBR 设计，baseColor 低于阈值时 1:1 精确还原，仅压缩高亮，无风格化色偏。
2. **Exposure 控制**：手动 exposure，线性倍率语义。app 层存 EV stops 并转换为线性倍率 `pow(2, ev)` 后传入渲染层；渲染层直接线性乘 `color * exposure`，不关心 EV 摄影/摄影概念。转换点在 app 层，与 Himalaya 一致（其 RenderInput.exposure 注释“linear scale, from pow(2, EV)”）。（原方案 `color *= exp2(exposure)` 让渲染层做 EV 转换，按“决策可更新性”调整为渲染层只接收线性倍率，职责更单一。）

---

### D18. Denoiser / DLSS-RR 预留

**核心问题**：Phase 4 为后续降噪/重建系统预留什么？

**两条技术路线**：
- **OptiX Denoiser**（Phase 4.5）：需要 albedo + normal aux buffer
- **DLSS Ray Reconstruction**（未来）：需要 albedo + normal + depth + motion vectors。Motion vectors 需要时域数据（前帧 hit position），Phase 4 不实现

**Phase 4 可预留项**：
- LaunchParams 携带 aux buffer 指针（albedo + normal + depth）
- 分配 aux buffers（跟随渲染分辨率）
- closesthit bounce==0 时可选写入 aux data（albedo、normal、depth 对两条路线都需要）
- motion vectors 不在 Phase 4 范围，但 LaunchParams 中可预留指针位置

**决策**：✅ **Phase 4 实际写入 aux data + debug view**。
1. LaunchParams 增加 aux buffer 指针（albedo + normal + depth），motion vectors 指针预留位置不分配
2. 分配 aux buffers（跟随 render_width × render_height）
3. closesthit bounce==0 时写入 albedo、world normal、linear depth（`optixGetRayTmax()`）
4. UI 提供 debug view 切换（显示 aux buffer 内容），提前验证数据正确性

---

### D19. 数据资产加载管线

**非可选项（实现细节讨论）**：
- **Sobol direction numbers**：Joe & Kuo 标准文件解析，一次性上传 16KB
- **Blue noise 纹理**：128×128 R16 PNG，一次性上传
- **Emissive 三角形数据**：场景加载时扫描收集 + alias table 构建
- **Env map**：取决于 D12 的决策

**讨论点**：
- 资产文件的集成方式：嵌入二进制（CMake resource embedding）vs 运行时文件加载？
- Sobol + Blue noise 是静态数据，嵌入更可靠；env map 和 emissive 是场景相关

**决策**：✅
1. **静态数据（Sobol + Blue Noise）**：CMake 嵌入二进制。数据固定不变，零运行时 I/O，不依赖文件路径。
2. **场景数据（Emissive 三角形 + Env Map）**：`load_scene` 扩展逻辑。Emissive 扫描 + alias table 构建；Env HDR 加载 + equirect→cubemap 转换。

---

## 第五层：特性级别细节

### D20. Russian Roulette

- Himalaya：bounce ≥ 2 启用，存活概率 = clamp(max_component(throughput), 0.05, 0.95)
- Phase 4 是否纳入？（减少不必要的长路径计算，但本质上是"优化"）

**决策**：✅ **纳入**。无偏终止（存活时 throughput /= survival_prob），不改变期望值。是避免浪费而非加速收敛。

---

### D21. Firefly Clamping

- indirect bounces (bounce > 0) 对 contribution 做 `min(value, max_clamp)`
- 这是画质改善而非优化。Phase 4 是否纳入？阈值如何暴露？

**决策**：❌ **不纳入**。Firefly clamping 有 bias（截掉能量）；Himalaya 实践中默认禁用（`max_clamp = 0`，注释 "OIDN denoise suffices"）——降噪器本身能处理 firefly，极亮点是合法辐射度信息。等出现可见 firefly 问题再考虑加入。

---

### D22. IBL 旋转

- Y 轴旋转（sin/cos），env 采样和 miss shader 均应用
- 简单且有用的用户交互功能

**决策**：✅ **纳入**。miss shader 和 NEE 采样方向均应用旋转，UI 暴露旋转角度。

---

### D23. Target Sample Count + Auto-stop

- 达到 target_samples 后停止累积
- 简单的收敛停止条件

**决策**：✅ **纳入**。`sample_count_ >= target_sample_count` 时跳过 launch。UI 暴露 `target_sample_count`（0 = 无限累积）。

---

### D24. indirect_intensity 乘数

- 全局间接光强度乘数
- 简单的艺术控制参数

**决策**：✅ **不纳入**。非物理正确，Phase 4 不需要艺术控制参数。

---

### D25. UI 参数暴露

**需要在 Phase 4 暴露的运行时参数（依赖上述决策）**：
- max_bounces（int, 默认 16）
- samples_per_frame（int, 1~64）
- render_scale（float, 0.25~1.0）
- exposure（float）
- ibl_rotation（float, 0~2π）
- target_sample_count（int, 0=无限）
- debug view 切换（enum: final / albedo / normal / depth）

**决策**：✅ **上述清单即最终参数集**。无 indirect_intensity（D24）、无 tonemap 切换（D17 仅 PBR Neutral）、无自适应参数（D16）。

---

## 衍生问题

### D26. Multi-launch 迁移预留

**来源**：D1 决策（Megakernel，未来演进到 Multi-launch）

**核心问题**：Phase 4 的 Megakernel 实现中，需要做哪些设计上的预留以降低未来迁移到 Multi-launch 的成本？

**决策**：✅ **三项代码组织原则**（零额外运行时/架构开销）：
1. **PathState 结构体**：定义 path 完整状态（origin, direction, throughput, radiance, cone, pixel/sample/bounce index, alive）。Phase 4 作为 raygen 局部变量，迁移时变为全局 buffer 元素类型。
2. **Payload pack/unpack helpers**：closesthit→raygen 的 18 register 编解码封装在 `payload_helpers.cuh`，改布局只需改一处。
3. **Bounce loop body 隔离**：raygen bounce loop 体抽为 `__device__` 函数。迁移时从 for-loop 调用变为每次 launch 调用一次。

---

### D27. 多散射能量补偿（Metal Energy Compensation）

**核心问题**：标准单散射微面元 BRDF（GGX + Smith G2）在高粗糙度时系统性丢失能量，粗糙金属表面肉眼可见地偏暗。是否在 Phase 4 中补偿？

**背景**：Himalaya 未实现能量补偿（`shaders/common/brdf.glsl` 仅含标准 D_GGX + V_SmithGGXCorrelated + F_Schlick）。GGX 在 roughness=1、F=1 时能量损失约 60%。

**补偿策略选项**：

| 方法 | 核心思路 | 互易性 | 采样影响 |
|------|---------|--------|---------|
| Heitz 2016 | 微面元随机游走模拟 | ✓ | 需额外随机数 |
| Kulla-Conty 2017 | 加漫反射形补偿 lobe | ✓ | 需采样新 lobe |
| Turquin 2019 | 对 ρ_ss 乘缩放系数 | ✗ | 无需改动 |

Heitz 太慢（7-15×）排除。Kulla-Conty 需要修改采样策略。Turquin 只需乘增益、不改采样，且 lobe 形状更接近 ground truth（Turquin 论文 Table 1 评级 B+ vs Kulla-Conty 的 B）。互易性对单向 PT 无影响。

**E_ss 获取选项**：

| 方式 | 描述 |
|------|------|
| 预计算 LUT（32×32） | 需要预计算 kernel + 纹理上传 + LaunchParams 扩展 |
| 有理多项式拟合（Sforza & Pellacini 2022） | 19 个系数，纯数学，零额外资源 |

多项式拟合精度验证（对 32×32 ground truth 表）：均值误差 0.004，最大误差 0.027（极端角落），furnace test 验证"几乎精确匹配"。

**决策**：✅ **Turquin + 多项式拟合**。

- 补偿策略：Turquin（对 ρ_ss 乘增益，不改采样）
- E_ss 来源：Sforza & Pellacini 有理多项式（19 系数，零 LUT）
- 适用范围：specular lobe（导体/电介质共用，电介质因 F0 低补偿量自然小）
- 实现位置：`brdf.cuh`，纯代码增量

**精度验证（White Furnace Test，F=1，target E=1.0）**：

| 指标 | 无补偿 | 本方案 |
|------|--------|--------|
| 均值误差 | 0.194 | 0.006 |
| 最大误差 | 0.693 | 0.082 |
| 中位误差 | 0.139 | 0.003 |

5.7%（58/1024）的点本方案误差大于无补偿，全部位于低粗糙度区域（r < 0.13）——该区域能量损失本身极小（无补偿误差 < 0.013），多项式近似误差导致微小过补偿，最大额外误差 0.012，视觉不可见。在真正需要补偿的高粗糙度区域始终大幅优于无补偿。

**参考文献**：
- Turquin 2019, "Practical multiple scattering compensation for microfacet models"
- Sforza & Pellacini 2022, "Enforcing Energy Preservation in Microfacet Models"（代码：github.com/dsforza96/energy-preservation，MIT 许可）

---

### D28. 渲染准确性审计（对照物理正确性）

**来源**：Phase 4+4.5 全计划审视，对照物理地面真值（非对标 Himalaya）。

**已处理项**：
- Shadow Terminator：已加入 Step 7 为单独小项
- roughness=0 NaN：已补充到 Step 3 brdf.cuh 项
- Emissive NEE 光源朝向检查：已补充到 Step 7 Emissive NEE 项

**待决议题**（需用户决策是否纳入 Phase 4/4.5）：

#### 28a. ~~Lambertian vs Disney/Burley Diffuse~~ → EON Diffuse

**决策**：✅ 采用 **EON（Energy-Preserving Oren–Nayar）** 替代 Lambertian。EON 比 Burley 更优：能量守恒且保持（通过 furnace test）、互易、捕捉粗糙回射效应、自带多散射补偿、被 OpenPBR 标准采纳。性能差异在 GPU PT 中可忽略（< 1%）。

Phase 4 完整采纳 EON + CLTC 采样（掠射角方差降 100×）。

参考实现：github.com/portsmouth/EON-diffuse（代码 MIT 许可，允许移植修改；JCGT 2025 论文文本为 CC BY-ND 3.0，与代码许可无关）。

#### 28b. Schlick Fresnel vs 精确 Fresnel / F82-tint

**决策**：❌ **不纳入**。F82-tint 需要 KHR_materials_specular 扩展提供 F82 参数，无此扩展数据时 Schlick 与 F82-tint 结果完全一致。M1 不解析该扩展，无数据源。

**F_Schlick 函数形态**：仅提供标准形式 `F_Schlick(VdotH, F0)`（F90=1 隐含），**不暴露 F90 参数重载**。理由：F82-tint 不做、涂层/coated 材质不规划、metal energy compensation（D27）走独立 Turquin 增益而非 F90 修正——无任何 F90≠1 的使用场景。任务清单原措辞“F0 + F90”指 Schlick 公式的两个端点形式（描述公式），不构成 API 要求。

#### 28c. Turquin 补偿的 Diffuse-Specular 耦合

**决策**：✅ **纳入**，与 Turquin 补偿在 Step 3 同一小项中实现。

**问题**：Turquin 补偿增加了 specular 能量，但 diffuse 权重 `(1-F)` 未相应减少。更根本地，`(1-F)` 忽略了微面元间多次弹跳后穿透到 diffuse 层的光——roughness 高 + 掠射角时偏差显著。

**修复**：将 diffuse 权重从 `(1-F)` 改为 `(1 - E_glossy)`，其中 `E_glossy(NdotV, roughness, F0)` 为 Sforza 39 系数有理多项式，直接输出 Turquin 补偿后含 Fresnel 的 specular 方向反照率。代价 ~39 FMA/着色点，GPU PT 中可忽略。

**勘误**：此前文档曾写 `E_spec_compensated = E_ss + F0*(1-E_ss)` 给出 0.808，这是错误的——E_ss 是 F=1 几何反照率（~0.8），不含 Fresnel；正确的 specular 方向反照率对 dielectric（F0=0.04）在正视角约 0.032。39 系数多项式直接对含 Fresnel 的积分拟合，避免了这个错误。

**specular_probability**：不需要调整。E_glossy 对 dielectric 在正视角约 0.032，与 F_Schlick(NdotV) ≈ 0.04 差距极小，对采样方差影响可忽略（非上一轮分析中基于错误公式得出的"8% vs 89%"）。

**参考**：github.com/dsforza96/energy-preservation（MIT 许可，`glossy.csv` 系数 + `glossy.py` 生成表）。

#### 28d. Normal Map Specular Anti-Aliasing

**决策**：✅ **纳入 Phase 4.5**，在 Ray Cone LOD 实现后做。依赖 ray cone footprint 估算法线贴图方差，将方差叠加到 roughness²。不做 LEAN/LEADR（预处理管线复杂度过高），采用 Kaplanyan 2016 式 roughness variance 方案。

---

### D29. Step 8 UI 面板范围审视

**来源**：Step 7 完成后、Step 8 实现前的全项目审视。先审计 Qualquer 全源码，再对照 Himalaya 面板补充，最终逐类讨论决策。

**审计方法**：完整阅读所有非第三方源码（约 60 个文件），提取六类潜在面板项（可调参数、只读统计、功能开关、按钮/动作、可视化/Debug 模式、交互体验改进），共计 97 项。

**决策**：✅ 以下为审视后纳入 Phase 4 Step 8 的项（D25 原始清单的增量）：

| 类别 | 纳入项 | 不纳入（理由） |
|------|--------|---------------|
| 可调参数 | FOV 滑块 | near/far（PT 中对渲染结果零影响）、roughness clamp（0.04 已是业界通用值）、其他底层阈值/epsilon |
| 只读统计 | Camera pos + yaw/pitch、场景资产统计（Meshes/BLAS、Instances、TLAS Instances、Materials、Textures、Triangles、Vertices、Emissive triangles、Env map resolution） | 帧号/ping-pong slot 诊断、含默认纹理总数、opaque 计数、emissive 功率、alias table 细节、AABB、对角线、常量；ms/sample（帧率已有） |
| 功能开关 | 开/关累积（关闭时 spp=0 冻结画面，保留已收敛结果；原方案 chain_count 恒 0 每帧覆写导致 1 spp 噪声闪烁，实现后改为冻结） | 暂停渲染（需处理 semaphore 链，复杂度高）、NEE/MIS/BRDF/tonemap 等开关（不纳入 Phase 4） |
| 按钮 | 手动 Reset 累积 | 缓存清理按钮 |
| 可视化 | 无 | 全部不纳入 Phase 4（Phase 4.5 有 aux data + debug view） |
| 交互改进 | Deferred slider（Himalaya slider_float_deferred） | — |

**附加小项**：SER 性能测试（对比 SER 开/关 + hint/无 hint 帧率差异）。

**near/far 不暴露的技术依据**：raygen unproject 求方向时，near/far 影响的投影矩阵第三行在 perspective divide 后被消掉——三个分量共享含 far 的缩放因子，normalize 后消失。PT 无深度缓冲消费 depth mapping，near/far 对渲染结果零影响。

---

### D30. Step 8 UI 架构设计

**来源**：Step 8 实现前的架构讨论。DebugUIContext 在 Phase 3 规模（6 字段）下合理，Step 8 新增 15+ 字段后膨胀为"万物袋"。

**问题**：Step 8 新增可调参数（max_bounces, samples_per_frame, exposure_ev, fov）、只读统计（sample count, camera pos/yaw/pitch, 场景资产统计 10 项）、功能开关（accumulation_enabled）等字段。平铺进 DebugUIContext 会产生散装标量堆积。

**决策**：✅ 提取领域结构体，DebugUIContext 组合引用。

1. **`RenderSettings`**（renderer 层，与 Camera 同级）——用户可调的 PT 旋钮：
   - `max_bounces`（uint32_t, 默认 16）、`samples_per_frame`（uint32_t, 默认 1）、`exposure_ev`（float, 默认 0.0）、`accumulation_enabled`（bool, 默认 true）
   - Application 拥有，运行时 live 参数，不持久化到 config.json
   - 通过 SceneRenderInput 传入 Renderer::submit_cuda（exposure 在 Application 做 `pow(2, ev)` 转线性）

2. **`SceneStats`**（renderer 层）——只读场景资产快照：
   - meshes, blas_groups, instances, tlas_instances, materials, textures, triangles, vertices, emissive_triangles, env_map_width, env_map_height
   - Application 在 load_scene 后统计并缓存

3. **DebugUIContext 新增引用**：`RenderSettings&`（mutable）、`Camera&`（mutable，FOV 滑块）、`accumulated_samples`（只读快照）、`const SceneStats&`

4. **现有 6 字段保持不动**：delta_time、context、swapchain、error_message、scene_path、env_map_path 各自是单一引用指向明确所有者，不存在散装堆积问题，不为统一而硬塞进某结构体

5. **参数变化检测**：Renderer 自行比对前帧参数值（与 camera 变化检测同机制），变化时 chain_count=0。不依赖 UI 层 flag。手动 Reset 按钮走 DebugUIActions::accum_reset_requested

6. **Deferred slider**：`slider_float_deferred` + `slider_angle_deferred` 作为 debug_ui.cpp 匿名命名空间工具函数

---

## Phase 划分

> 基于 D1-D36 决策结果划分 Phase 4 与 Phase 4.5 的特性边界。

### 范围判据

4000 spp 是 **Phase 4 / 4.5 特性归属的判据**，不是项目终极目标。项目长期目标是实时 PT 渲染器（见 `roadmap.md`）。

- **归 Phase 4**：某特性若缺失，会导致 4000 spp 也无法大致收敛（必要特性）。
- **归 Phase 4.5**：某特性缺失时 4000 spp 仍能收敛，但它是迈向实时 PT 的必需步骤（必要，推迟实现）。
- "推迟" 仅指实现顺序，不意味着可省。Phase 4.5 的低方差采样（Sobol/CLTC 已在 Phase 4）、Russian Roulette、降噪等都是实时 PT 的硬需求。

### Phase 4：核心 PT

Phase 4 实现功能完整的 PT 渲染器。以 "4000 spp 大致收敛" 作为特性归属判据——缺失则 4000 spp 无法收敛的特性归此 Phase。

**包含特性**：
- 多 bounce 循环（Megakernel + SER）— D1
- 全信息 Payload（18 registers）— D2
- 单 .cu + 多 .cuh 代码组织 — D3
- Raygen 内 sample 循环（max 64 spp/frame）— D4
- Separate Sum 累积 + reset + 语义拆分 — D5
- 2 miss + 1 hitgroup（CH + AH，BLAS flag 控制 anyhit）— D6（由 D11 推导）
- **PCG hash RNG**（D7 决策的 Sobol + Blue Noise 推迟到 4.5，Phase 4 用 PCG 替代）
- BRDF 全集（D_GGX, V_SmithGGXCorrelated, F_Schlick, EON diffuse + CLTC 采样, VNDF, multi-lobe selection, combined PDF）+ 多散射能量补偿（specular: Turquin + Sforza 多项式, diffuse: EON 内建）— D8, D27, D28a
- NEE + MIS 全集（env alias table, emissive alias table, power heuristic, shadow ray）— D9
- Shadow ray with any-hit — D10（由 D9+D11 推导）
- Alpha mask + double-sided pass-through — D11（stochastic alpha 推迟到 4.5）
- Env cubemap — D12
- Subpixel jitter
- Ray origin offset + shading normal consistency — D14
- Tonemap（Khronos PBR Neutral）+ 手动 exposure — D17
- Multi-launch 迁移预留（PathState / payload helpers / bounce isolation）— D26
- UI 面板（滑块 + 统计 + 功能开关 + Reset + deferred slider + SER 性能测试）— D25 子集 + D29 审视增量

**不包含（推迟到 Phase 4.5）**：
- Sobol + Blue Noise RNG（用 PCG hash 替代）
- Ray Cone LOD（使用 LOD 0）
- Russian Roulette（所有 path 跑满 max_bounces）
- Firefly Clamping
- Stochastic Alpha
- IBL 旋转
- Target sample count + auto-stop
- Render resolution decoupling
- Aux data 写入 + debug view
MUSTREAD:7
- Denoiser

### Phase 4.5：收敛质量 + DLSS-RR + 自适应

> D31-D36 基于 Phase 4 完成后的重新评估，更新了部分早期决策。详见末尾 §Phase 4.5 决策。

#### 前半部分（按实现顺序）

| # | 特性 | 来源决策 | 效果 |
|---|------|---------|------|
| 1 | IBL 旋转 | D22 | UX：环境光 Y 轴旋转 |
| 2 | Russian Roulette | D20 | 无偏终止低 throughput 路径 |
| 3 | Sobol + hash 去相关 | D7→D33 | 低差异序列替代 PCG，hash 去相关（不用 blue noise） |
| 4 | Render resolution decoupling | D15 | 渲染分辨率独立于 swapchain，DLSS-RR 前置 |
| 5 | DLSS-RR 集成（含 aux data） | D31/D32 | CUDA API：时域累积 + 去噪 + 放大 |
| 6 | 自适应 sample 数 | D34 | ping-pong/串行/极端三级降级 |

#### 后半部分（按实现顺序）

| # | 特性 | 来源决策 | 效果 |
|---|------|---------|------|
| 7 | Stochastic Alpha | D11 | blend 材质 hash-based alpha test |
| 8 | Ray Cone LOD | D13 | 纹理 aliasing 降噪，payload 扩展至 19 registers |
| 9 | Normal Map Specular AA | D28d | ray cone footprint → roughness 方差叠加 |

#### 不纳入

| 特性 | 原决策 | 理由 |
|------|--------|------|
| ~~Firefly Clamping~~ | D21 | 有 bias，DLSS-RR 足以处理 |
| ~~Target sample count~~ | D23→D35 | DLSS-RR 内部管理时域历史 |
| ~~OptiX Denoiser~~ | D18→D31 | 被 DLSS-RR 替代 |
| ~~Blue Noise 纹理~~ | D7→D33 | DLSS-RR 不推荐 |

---

## Phase 4.5 决策

> Phase 4 完成后的重新评估。部分决策更新了早期 Phase 4 决策（标注为 DX→DY）。

### D31. DLSS-RR 替代 OptiX Denoiser

**来源**：Phase 4.5 规划重审。

**核心问题**：降噪 + 时域累积 + 放大方案选择。

**决策**：✅ **DLSS Ray Reconstruction 替代 OptiX Denoiser**。

DLSS-RR 同时做时域累积、去噪、放大，OptiX Denoiser 只做去噪。DLSS-RR 直接到达实时 PT 管线终态，跳过 OptiX Denoiser 中间站。

- 集成方式：CUDA API（DLSS SDK 310.5.3+，`NVSDK_NGX_CUDA_*`），不依赖 Vulkan
- 管线位置：raygen → DLSS-RR → tonemap（官方文档 §3.1："before tone mapping"）
- Separate Sum 累积被 DLSS-RR 内部时域累积替代
- Ping-pong 双缓冲保留（避免 raygen 和 DLSS-RR 读写 stall）
- 渲染分辨率由 UI 滑块直接控制（不使用 DLSS quality mode 档位），InPerfQualityValue 根据实际放大比率自动选取最接近的档位
- Dynamic resolution 不支持（§3.3），分辨率变化需重建 feature

**D18 更新**：原"Phase 4 实际写入 aux data + debug view"的 aux data 规格改为 DLSS-RR 所需输入（见 D32），OptiX Denoiser 相关预留移除。

**参考项目**：
- optix-subd：`D:\Github\optix-subd`（`denoiserDlss.cu`，CUDA API 集成参考）
- vk_denoise_dlssrr：`D:\Github\vk_denoise_dlssrr`（NVIDIA 官方 DLSS-RR Vulkan PT 示例，`dlss_rr/src/dlssrr_wrapper.cpp` + `dlss_rr/shaders/primary.rgen`，jitter/MV/aux data 全流程参考）
- DLSS SDK：`D:\Github\DLSS`（`doc/DLSS-RR Integration Guide.pdf`，`include/nvsdk_ngx_helpers_dlssd_cuda.h`）

---

### D32. DLSS-RR Aux Data

**来源**：D31。

**必需输入**（closesthit bounce==0 写入，渲染分辨率）：

| 参数 | 格式 | 来源 |
|------|------|------|
| pInColor | float4 HDR | raygen 单帧 noisy 输出 |
| pInDepth | R32F | `optixGetRayTmax()`（linear depth，`InUseHWDepth = Linear`） |
| pInMotionVectors | RG32F | 前帧 VP 投影差（静态场景仅 camera motion） |
| pInDiffuseAlbedo | float4 | `base_color × (1 - metallic)` |
| pInSpecularAlbedo | float4 | E_glossy 逐通道（见下） |
| pInNormals | float4 | shading normal（world space） |
| pInRoughness | R32F | linear roughness |
| pInSpecularHitDistance | R32F | 写 infinity（optix-subd 实测不改善输出） |

**矩阵**（host 侧每帧传入）：pInWorldToViewMatrix + pInViewToClipMatrix（float[16]，row major）。

**Specular Hit Distance**：NVIDIA optix-subd 参考实现实测发现写入实际 specular hit distance 对 DLSS-RR 输出质量无改善，直接写 `std::numeric_limits<float>::infinity()`（代码注释："doing so did not improve the denoiser output"）。Qualquer 同样写 infinity。

**Specular Albedo 使用 E_glossy**：DLSS-RR 文档附录给出 EnvBRDFApprox2（Ray Tracing Gems Ch.32 split-sum 近似），但 Qualquer 的 specular 层使用 Turquin 多散射补偿，实际 specular 能量高于单散射近似。E_glossy 与渲染的 specular 能量自洽，guide buffer 和 color 的一致性优于匹配训练分布。

**Specular Motion**：提供 pInSpecularHitDistance（infinity）+ 两个矩阵，SDK 内部推导 specular motion vectors。

**多 spp 与 aux data 一致性**：帧内多 sample 共享同一 subpixel jitter（per-frame jitter，不 per-sample jitter），所有 sample 的 primary ray 相同 → aux data（depth、normals、albedo、roughness）在所有 sample 间完全一致，写一次即可。跨帧 jitter 变化保留 DLSS-RR 时域超分辨率能力。Color 为同一亚像素位置 N 个 sample 的辐射度平均（更干净的点采样，非 box filter）。Jitter offset 每帧一个值，无歧义。详见 D37。

**可选输入**：InFrameTimeDeltaInMsec 随初始集成提供（零额外代价）。

**保留项**（能做但先不做，出现可见问题时再评估）：
- pInDepthHighRes：若边缘有可见 bleeding/ghosting，可考虑 OptiX primary ray only launch 生成输出分辨率 depth。optix-subd 参考实现未给 DLSS-RR 喂此值。vk_denoise_dlssrr 亦未使用。
- postProcess 背景修正：若 DLSS-RR 输出出现 sky 伪影，在输出分辨率上用 depth 分类 sky 像素并重着色环境光（参考 optix-subd `postProcessKernel` WAR）。与 pInDepthHighRes 共享输出分辨率 depth 资源。vk_denoise_dlssrr 未做此处理。

**不适用的可选输入**：
- pInDiffuseHitDistance / pInReflectedAlbedo：PT stochastic lobe 选择下 per-sample 不一致，无单一正确值
- pInColorBeforeTransparency / pInTransparencyLayer：PT 无分离透明通道
- pInAnimatedTextureMask：M1 静态场景无动画纹理
- pInMotionVectors3D：静态场景下 3D MV 全为零，2D MV 信息更完整

---

### D33. 采样策略更新（D7 更新）

**来源**：D31 DLSS-RR 集成。

**决策**：✅ **Sobol + hash 去相关，不用 blue noise**。

DLSS-RR Integration Guide §3.5 将 blue noise 归入"to be avoided"列表。推荐白噪声 / 高质量 hash。Blue noise 的唯一优势是低 spp 下裸图的感知质量（噪声推到高频），对降噪后的画面无实际收益。

- Sobol 低差异序列保留（收敛收益）
- 去相关方式：PCG hash per-pixel Cranley-Patterson rotation（替代 blue noise 纹理查询）
- golden-ratio temporal scramble 保留
- dim ≥ 128 fallback PCG hash（不变）
- Blue noise 纹理 + CUDA texture 加载移除
- `__constant__` memory Sobol table 保留

**Jitter**：DLSS-RR 要求至少 32 个 jitter 位置（§3.6），不限上限。Sobol dim 0-1 + hash 去相关产生无限不重复 jitter。每帧通过 `InJitterOffsetX/Y` 传递当前 jitter offset（像素空间，[-0.5, 0.5]）。

---

### D34. 自适应帧率

**来源**：Phase 4.5 新增。

**核心问题**：ping-pong 架构比串行多一帧显示延迟。串行下 75fps 以上延迟不可感知；ping-pong 需 150fps 以上。

**决策**：✅ **三级降级策略**。通过调整每帧 samples_per_frame 控制帧时间，使实际帧率稳定在目标水平。呈现模式（MAILBOX/FIFO）独立于自适应策略。

**Mode 1（ping-pong 并行）**：
- 条件：调整 spp 后帧时间能满足目标
- 目标：min { n × refresh_rate | n ∈ ℤ⁺, n × refresh_rate ≥ 150 }
- 示例：60Hz → 180fps, 120Hz → 240fps, 165Hz → 165fps

**Mode 2（串行）**：
- 条件：即使 1spp 也无法满足 Mode 1
- 目标：refresh_rate（若 < 150Hz）或 refresh_rate / 2（若 ≥ 150Hz）
- 同样通过调整 spp 控制帧时间

**Mode 3（极端降级）**：
- Mode 2 目标在 1spp 下也无法满足时反复减半
- 低于 60fps 时放弃帧节奏控制，1spp 串行放开跑

**整数倍刷新率**：避免非整数倍导致的微卡顿。与 DLSS-RR 无关，是基本帧节奏要求。

---

### D35. Target Sample Count 移除（D23 更新）

**来源**：D31。

**决策**：✅ **移除**。DLSS-RR 内部管理时域历史，不暴露"已累积多少 sample"，无有意义的收敛停止点。D23 原决策被取代。

---

### D36. Phase 4.5 特性排序

**前半部分**（按实现顺序）：IBL 旋转 → Russian Roulette → Sobol + hash 去相关 → Render resolution decoupling → Aux data 写入 → DLSS-RR SDK 接入 → DLSS-RR 管线接入 → 自适应 sample 数

**后半部分**（按实现顺序）：Stochastic Alpha → Ray Cone LOD → Normal Map Specular AA

---

### D37. 多 spp 帧内 jitter 策略（D4 更新）

**来源**：D32 aux data 一致性问题。

**核心问题**：多 spp 时每个 sample 有不同 subpixel jitter，导致 primary ray 不同 → aux data（depth、normals、albedo 等）在 sample 间不一致。DLSS-RR 每帧只接收一组 aux data 和一个 jitter offset。

**决策**：✅ **帧内 sample 共享 jitter，仅跨帧变化 jitter**。

raygen 内 sample loop 的 subpixel jitter 从 per-sample 改为 per-frame：所有 sample 使用同一 jitter（由 frame_index 决定），BRDF/NEE 等维度（dim 2+）仍然 per-sample 不同。

- 同一帧所有 sample 的 primary ray 相同 → aux data 完全一致，写一次即可
- color 是同一亚像素位置 N 个 sample 的辐射度平均（更干净的点采样，非 box filter）
- jitter offset 每帧一个值，直接传给 DLSS-RR，无歧义
- 跨帧 jitter 变化完整保留 → DLSS-RR 时域超分辨率不受影响
- 帧内几何/纹理 anti-aliasing 不靠 per-sample jitter → 由 DLSS-RR 跨帧时域重建处理

**D4 更新**：原"raygen 内 sample 循环"机制不变（仍在 raygen 内 for loop），仅 jitter 维度分配调整。RNG dim 0-1 改为由 frame_index 驱动（per-frame），不再由 sample_index 驱动（per-sample）。
