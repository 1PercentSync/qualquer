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

**决策**：

---

## 第二层：核心管线结构（影响多个特性的实现方式）

### D4. 每帧多 Sample 的执行策略

**核心问题**：N samples/frame 如何执行？

**选项摘要**：
- **A. Raygen 内 sample 循环**：1 次 optixLaunch，raygen 内 for(s=0..N-1)，寄存器内局部累积。带宽最优
- **B. 多次 optixLaunch**（每 sample 一次）：改动最小，但 launch overhead × N，每次都读写全局 accum buffer
- **C. 3D launch**（width × height × N）：需原子操作或 per-sample 临时 buffer

**关键考量**：TDR 安全（Windows 2s 限制），寄存器压力（A 只多 3 floats + 1 counter）

**决策**：

---

### D5. 累积策略

**前置依赖**：D4（多 sample 策略决定累积公式）

**子问题**：
1. **Running Average vs Separate Accumulator**：`mix(old, frame_avg, N/(total+N))` vs `sum[pixel] += total; count[pixel] += N; display = sum/count`
2. **累积重置触发条件**：camera 变化、PT 参数变化、渲染分辨率变化、场景切换
3. **检测方式**：逐帧比较 inv_view + inv_projection（exact float compare）
4. **frame_counter_ / sample_count_ / frame_seed_ 语义拆分**：当前 frame_counter_ 混用了 slot 索引和 frame index

**注意**：未来 DLSS Ray Reconstruction 可在 1 spp 下直接产出高质量画面，届时实时预览模式可能不走累积管线。但 Phase 4 的累积系统仍然是物理收敛渲染的基础，两者互补而非替代。

**决策**：

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

**决策**：

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

**决策**：

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

**决策**：

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

**决策**：

---

### D10. Shadow Ray 策略

**前置依赖**：D9（有 NEE 才需要 shadow ray），D6（SBT 配置影响 shadow ray 走法）

**选项**：
- **A. Standard**：`TERMINATE_ON_FIRST_HIT | DISABLE_CLOSESTHIT`，miss 设 visible=1
- **B. With Any-hit**：不用 TERMINATE，anyhit 做 alpha test（透明阴影）

**关键细节**：
- Ray origin offset（Wächter & Binder）是 shadow ray 和 bounce ray 的前置需求
- Emissive shadow ray 的 tMax = `dist * (1 - 1e-4)`

**决策**：

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

**决策**：

---

### D12. Env Map 表示

**核心问题**：miss shader 采样环境光用 cubemap 还是 equirect 2D 纹理？

**选项**：
- **A. Cubemap**（Himalaya）：equirect → cubemap CUDA kernel 转换，`texCubemap` 采样，无极地畸变
- **B. Equirect 直接采样**：跳过转换步骤，但极地 sampling 畸变，filtering 质量不如 cubemap

**关键考量**：alias table 从 equirect 像素构建（与采样方式无关）；环境旋转两种方式都是对 ray direction 做 Y 轴旋转

**决策**：

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

**决策**：

---

### D14. Ray 健壮性

**非可选项（标记为参考）**：
- **Ray origin offset（Wächter & Binder）**：bounce ray 和 shadow ray 的自交避免。PT 的硬性需求
- **Shading normal consistency**（reflect shading normal to geometric normal hemisphere）：防止"黑色像素"问题

**讨论点**：
- 这两个应该没有争议——都是正确 PT 的必要组件。仅确认纳入 Phase 4

**决策**：

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

**决策**：

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

**决策**：

---

### D17. Tonemap

**当前状态**：`clamp01(hdr)` — 不是真正的 tonemap

**需要讨论**：
1. **Tonemap 函数**：ACES / Reinhard / AgX / 可运行时切换？
2. **Exposure 控制**：固定值 / auto-exposure / UI 可调？
3. **与渲染分辨率解耦的交互**：tonemap kernel 处理 render_width × render_height 子区域

**决策**：

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

**讨论点**：
- Phase 4 是否实际写入 aux data（作为 debug view 验证）？
- 还是仅预留指针/buffer，后续再填充？
- depth 输出的格式（linear Z vs 其他）

**决策**：

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

**决策**：

---

## 第五层：特性级别细节

### D20. Russian Roulette

- Himalaya：bounce ≥ 2 启用，存活概率 = clamp(max_component(throughput), 0.05, 0.95)
- Phase 4 是否纳入？（减少不必要的长路径计算，但本质上是"优化"）

**决策**：

---

### D21. Firefly Clamping

- indirect bounces (bounce > 0) 对 contribution 做 `min(value, max_clamp)`
- 这是画质改善而非优化。Phase 4 是否纳入？阈值如何暴露？

**决策**：

---

### D22. IBL 旋转

- Y 轴旋转（sin/cos），env 采样和 miss shader 均应用
- 简单且有用的用户交互功能

**决策**：

---

### D23. Target Sample Count + Auto-stop

- 达到 target_samples 后停止累积
- 简单的收敛停止条件

**决策**：

---

### D24. indirect_intensity 乘数

- 全局间接光强度乘数
- 简单的艺术控制参数

**决策**：

---

### D25. UI 参数暴露

**需要在 Phase 4 暴露的运行时参数（依赖上述决策）**：
- max_bounces
- samples_per_frame（或自适应时的 target_fps）
- render_scale（如果做分辨率解耦）
- max_clamp（firefly clamping 阈值）
- tonemap 函数选择
- exposure
- IBL rotation
- target_sample_count
- 其他取决于各决策结果

**决策**：

---

## 衍生问题

### D26. Multi-launch 迁移预留

**来源**：D1 决策（Megakernel，未来演进到 Multi-launch）

**核心问题**：Phase 4 的 Megakernel 实现中，需要做哪些设计上的预留以降低未来迁移到 Multi-launch 的成本？

**潜在预留方向**：
- 着色逻辑的封装方式（是否可从 closesthit 和 CUDA kernel 共用）
- Ray state 的结构化定义
- Payload 编解码的隔离

**决策**：
