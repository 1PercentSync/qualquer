# Phase 4 讨论议程

> 跟踪文档。讨论过程中对每个问题做出决策后标记 ✅ 并记录决策结论。
> 因决策而失去讨论意义的问题标记 ~~删除线~~。新衍生问题追加到相应位置。

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
- **A. 全信息 Payload**（~18 registers / 72 bytes）：color, next_origin, next_direction, throughput_update, hit_distance, bounce, env_mis_weight, last_brdf_pdf, cone_width, cone_spread — 全走 registers
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
4. **frame_counter_ / sample_count_ / frame_seed_ 语义拆分**：当前 frame_counter_ 混用了 slot 索引和 frame index

**注意**：未来 DLSS Ray Reconstruction 可在 1 spp 下直接产出高质量画面，届时实时预览模式可能不走累积管线。但 Phase 4 的累积系统仍然是物理收敛渲染的基础，两者互补而非替代。

**决策**：✅
1. **Separate Sum 累积**：`sum_new = sum_old + frame_total`，tonemap 中 `display = sum / total_sample_count`。count 是全局值（全帧均匀采样），无需 per-pixel buffer。精度略优于 running average，与现有 ping-pong 架构兼容。
2. **累积重置**：camera/PT 参数/渲染分辨率/场景切换时 `sample_count_ = 0`，实现参考 Himalaya。
3. **语义拆分**：`frame_counter_`（slot 索引，永不 reset）、`sample_count_`（累积总量，reset 时归零）、`frame_seed_`（RNG temporal scramble，永不 reset——避免重置后噪声序列重复）。

---

### D6. SBT / Pipeline 配置

**核心问题**：Phase 4 的 OptiX pipeline 需要几个 miss program、几个 hit group？

**当前状态**：1 raygen + 1 miss + 1 hitgroup（DISABLE_ANYHIT），SBT stride=1

**Phase 4 需要**：
- 至少 2 个 miss：`__miss__env`（环境光）+ `__miss__shadow`（设 visible=1）
- 是否需要 2 个 hit group（opaque vs non-opaque）取决于 alpha mask 是否在 Phase 4 实现
- stride=1（如果不做 non-opaque）vs stride=2（如果要 alpha-tested shadow ray）

**级联影响**：
- 影响 BLAS 构建（per-geometry opaque flag）
- 影响 shadow ray 对 alpha 物体的行为
- 数据链已完整：`BLASGeometry::opaque` 字段已存在，Material 有 `alpha_mode`

**结论**：由 D11 决策直接推导，无独立决策空间。2 miss + 2 hitgroup，stride=1。
- Miss：`__miss__env`（missIndex=0）+ `__miss__shadow`（missIndex=1）
- Hitgroup 0（opaque）：closesthit only，`DISABLE_ANYHIT`
- Hitgroup 1（non-opaque）：closesthit + anyhit（alpha mask / stochastic alpha）
- 几何体通过 `sbtOffset` 选择 hitgroup（`opaque = alpha_mode == 0`）
- Shadow ray 通过 ray flag `DISABLE_CLOSESTHIT` 跳过 closesthit，与 bounce ray 共享 hitgroup
- BLAS 构建改动：`BLASGeometry::opaque` 从硬编码 `true` 改为按 `material.alpha_mode` 判断

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
- V_SmithGGX（Visibility，height-correlated）
- F_Schlick（Fresnel）
- Lambertian diffuse（INV_PI）
- Multi-lobe selection（specular_probability）
- GGX VNDF sampling（Heitz 2018）
- Cosine hemisphere sampling
- Combined multi-lobe PDF
- Orthonormal basis construction

**讨论点**：
- 目标是"完整 PT 但不做性能优化"——上述全集是否都算"完整 PT 的基本组件"而非"优化"？
- VNDF sampling 和 cosine hemisphere 是 importance sampling，但它们属于"让 PT 正确/可用的基本采样"还是"性能优化"？
- 如果不做 importance sampling，uniform hemisphere sampling 是否能在合理时间内收敛？

**决策**：✅ **全做**。上述全集是完整 PT 的最小 BRDF 集，不是优化。Cosine hemisphere / VNDF / multi-lobe selection 是让 PT 在可行时间内产出可辨识结果的基本采样策略——没有它们 specular 材质实际不可用。

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
- 2 floats 已包含在 Himalaya 的 18 register payload 内

**决策**：✅ **A. Ray Cone**。payload 已有 2 floats 位置，mipmap 基础设施就绪，`tex2DLod` 直接可用。减少高频纹理 aliasing → 每 sample 质量更高。

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
2. **Exposure 控制**：手动 `exposure` 参数，`color *= exp2(exposure)`。

---

### D18. Denoiser / DLSS-RR 预留

**核心问题**：Phase 4 为后续降噪/重建系统预留什么？

**两条技术路线**：
- **OptiX Denoiser**（Phase 6）：需要 albedo + normal aux buffer
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

**决策**：✅ **纳入**。画质改善（去除 firefly 伪影），非性能优化。阈值 `max_clamp` 通过 UI 暴露。

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
- max_clamp（float, 默认 10.0）
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

## Phase 划分

> 基于 D1-D26 决策结果，按"4000 spp 大致收敛"标准划分 Phase 4 与 Phase 4.5 的特性边界。

### Phase 4：核心 PT（4000 spp 可收敛）

Phase 4 的目标是实现一个功能完整的 PT 渲染器，在 4000 spp 下对各类场景（含 HDR 环境光、小面积 emissive）能大致收敛。

**包含特性**：
- 多 bounce 循环（Megakernel + SER）— D1
- 全信息 Payload（18 registers）— D2
- 单 .cu + 多 .cuh 代码组织 — D3
- Raygen 内 sample 循环（max 64 spp/frame）— D4
- Separate Sum 累积 + reset + 语义拆分 — D5
- 2 miss + 2 hitgroup（opaque + non-opaque）— D6（由 D11 推导）
- **PCG hash RNG**（D7 决策的 Sobol + Blue Noise 推迟到 4.5，Phase 4 用 PCG 替代）
- BRDF 全集（D_GGX, V_SmithGGX, F_Schlick, Lambertian, VNDF, cosine hemisphere, multi-lobe selection, combined PDF）— D8
- NEE + MIS 全集（env alias table, emissive alias table, power heuristic, shadow ray）— D9
- Shadow ray with any-hit — D10（由 D9+D11 推导）
- Alpha mask + double-sided pass-through — D11（stochastic alpha 推迟到 4.5）
- Env cubemap — D12
- Subpixel jitter
- Ray origin offset + shading normal consistency — D14
- Tonemap（Khronos PBR Neutral）+ 手动 exposure — D17
- Multi-launch 迁移预留（PathState / payload helpers / bounce isolation）— D26
- UI 参数暴露（max_bounces, samples_per_frame, exposure）— D25 子集

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
- Denoiser

### Phase 4.5：收敛质量 + Denoiser

#### 第一部分：收敛质量与性能

| # | 特性 | 来源决策 | 效果 |
|---|------|---------|------|
| 1 | Sobol + Blue Noise RNG | D7 | 低差异序列替代 PCG，等效 sample 效率提升 4-8× |
| 2 | Ray Cone LOD | D13 | 减少高频纹理 aliasing noise |
| 3 | Russian Roulette | D20 | 无偏终止低 throughput 路径，减少无效计算 |
| 4 | Firefly Clamping | D21 | 抑制 firefly 伪影（有 bias 但视觉改善大） |
| 5 | Stochastic Alpha | D11 | blend 材质的正确半透明处理 |
| 6 | IBL 旋转 | D22 | UX：环境光 Y 轴旋转 |
| 7 | Target sample count + auto-stop | D23 | UX：达到目标 sample 数后停止 |
| 8 | Render resolution decoupling | D15 | 渲染分辨率独立于 swapchain，为自适应/DLSS-RR 预留 |

#### 第二部分：Denoiser 集成

| # | 特性 | 来源决策 | 说明 |
|---|------|---------|------|
| 9 | Aux data 写入 + debug view | D18 | closesthit bounce==0 写入 albedo/normal/depth，UI debug view |
| 10 | OptiX Denoiser 集成 | — | 原 Phase 6 内容，提前到 4.5 |
