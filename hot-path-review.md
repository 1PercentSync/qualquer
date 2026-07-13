# 热路径审计评审

> 对 CUDA/OptiX 热路径审计结果的评审。
> 每项分别列出原始审计的意见和评审分析的意见，所有项均待决策。

---

## 一、正确性

### 1. Pass-through 的 MIS 状态

**代码位置**：`programs.cu:464-496`，单面材质背面穿透分支。

**功能**：单面材质被从背面命中时，光线穿过去继续走，消耗一次 bounce。穿透时需要决定 payload 里两个 MIS 相关寄存器怎么处理——p13（`env_mis_weight`，控制 BRDF 方向 miss 到环境时拿多少权重）和 p14（`last_brdf_pdf`，控制 BRDF 方向命中 emissive 时拿多少权重）。

**现状**：p13 被重置为 1.0（490 行）；p14 被刻意保留不动（491-494 行注释解释了意图）。代码注释自身矛盾——491 行说保留 p14 "对 miss 到 environment 也正确"，但 p13 已被重写。

**原始审计**：认为 p13 不该重置，应和 p14 一样保留。理由是穿透不产生新散射策略，不应改变前一个 BRDF sample 的 MIS 状态。

**评审分析**：认为方向相反——p13 重置为 1.0 是对的，p14 保留才是 bug。事实链：

- shadow ray 用 `TERMINATE_ON_FIRST_HIT | DISABLE_CLOSESTHIT`（`nee.cuh:146-147`），opaque 几何在 BLAS 层 `DISABLE_ANYHIT`（`accel_structure.cpp:29-30`）。shadow ray 碰到单面背面会判定遮挡，不会穿透——穿透只在 closesthit 里发生。
- 所以 S 处的 NEE 采样同方向 d 时，shadow ray 被 B 挡住，贡献恒为零。该方向只有 BRDF 策略能取到。
- 两个策略的可见性函数在此不一致（NEE 看到遮挡，路径看到穿透），MIS 无偏性要求竞争策略对某样本有效贡献为零时主策略满权重。
- 穿透后光线从 `pass_origin` 出发（468-470 行），emissive hit 的 `optixGetRayTmax()` 量的是 pass_origin 到 E 的距离，而保留的 p14 里的 pdf 对应的是 S 出发时的距离，距离对不上。

**两者分歧**：完全相反。

| 方案 | 改动 | 理由 |
|------|------|------|
| A（原始审计） | p13 也保留（不写 1.0） | 穿透不产生新散射策略，不该改变 MIS 状态 |
| B（评审分析） | p14 也清零，p13 维持 1.0 | shadow ray 不可穿透 → NEE 对穿透后方向贡献恒零 → 应满权重 |

---

### 2. Normal consistency 的参照法线

**代码位置**：`programs.cu:522`，调用 `ensure_normal_consistency(N_mapped, N_interp)`。函数本体在 `pt_common.cuh:103-109`。

**功能**：防止法线贴图把 shading normal 推到几何表面的反面。做法是检查 shading normal 和参照法线的点积，为负就反射。

**现状**：参照法线用的是 `N_interp`（插值顶点法线）。

**原始审计**：应参照 `N_face`（几何面法线）。理由是 N_face 代表真实几何表面，ray offset、back-face 判断、shadow terminator 都以它为准；N_interp 本身可能因平滑法线跨越几何面半球。

**评审分析**：倾向同意换 N_face，补充了以下论据：
- N_interp 合法越过 N_face 半球需要顶点法线与所属面法线夹角超 90°，常规资产几乎不会出现。
- 换了之后 Chiang 公式的 `dot(N_face, N_shading) > 0` 由构造保证，`pt_common.cuh:173-176` 的逃生分支变死代码。
- 与函数自身文档宣称的目的（"防穿过几何表面"）一致。

**两者分歧**：方向一致（都倾向 N_face），评审补充了额外论据。

完整选项供参考：

| 参照 | 保护什么 | 优势 | 劣势 |
|------|---------|------|------|
| N_face | 防穿过真实几何面 | 管线一致；Chiang 前提由构造保证；与函数文档一致 | N_interp 越过 N_face 时（极罕见）会触发不必要反射 |
| N_interp（现状） | 防翻越插值法线 | 剪影处法线变化更平滑 | 管线中不一致 |

---

### 3. dot(N_shading, V) ≤ 0 时没有 fallback

**代码位置**：`brdf.cuh:713`（`init_brdf_params` 里的 NdotV clamp），以及 `brdf.cuh:759,791,823`（`brdf_eval/pdf/sample` 各自重算 `V_ts`）。

**功能**：`init_brdf_params` 确保标量 `NdotV > 0`，供 D/V/F 等函数使用。但 `brdf_eval/pdf/sample` 各自用 `world_to_tangent` 重新计算 `V_ts`，其 z 分量是真实的 `dot(N, V)`。

**现状**：标量 NdotV 被 clamp 到正值，但 V_ts.z 可以为负。

**原始审计**：指出此缺口——N_shading 背向 V 时 EON/CLTC/VNDF 收到定义域外方向。建议校正后仍 `dot(N_shading, V) ≤ 0` 时回退到面向视线的有效法线。

**评审分析**：同意问题存在。补充：
- 平滑低模剪影处每帧可达，不是理论角落。
- 与 Step 14.5 曾结论的"geometry/shading frame 不可能退化"矛盾——那次审计漏了。
- **无论参照法线选 N_interp 还是 N_face，半球反射都不保证结果面向 V**，所以这个 fallback 是独立于第 2 条的必要修复。

**两者分歧**：问题判断一致。评审补充了严重性和独立性分析。

fallback 形态选项：

| 方案 | 做法 | 优势 | 劣势 |
|------|------|------|------|
| 回退到 N_face | `N_shading = N_face` | 物理自洽；与 N_interp 归零时的现有 fallback（`programs.cu:444-451`）同构；NdotV 在正常范围内 | 着色不连续 |
| 最小旋转到 V 半球边缘 | 把 N_shading 投影到恰好使 NdotV > 0 | 着色连续 | 制造 NdotV≈ε，V_Smith/EON 数值最差的点 |

---

### 4. shadow_terminator_factor 的 NgdotNs 逃生分支

**代码位置**：`pt_common.cuh:173-176`。

**功能**：当 `dot(N_geo, N_shading) ≤ 0` 时跳过校正返回 1.0。

**现状**：注释自己承认这个分支存在是因为 consistency 参照的是 N_interp 而非 N_face。

**原始审计**：未单独提及。

**评审分析**：如果第 2 条选择 N_face 参照，`dot(N_face, N_shading) > 0` 由构造保证，此分支变死代码，可删。如果维持 N_interp 参照，此分支仍有意义。

---

### 5. PDF 边界语义

**代码位置**：`nee.cuh:116-117`（`env_pdf` 的 `fmaxf(·,1e-7f)`）；`nee.cuh:210-214`（`emissive_light_pdf` 的 `total_power ≤ 0 || cos ≤ 0 → 1e-7f`）；`nee.cuh:291,399`（两个 NEE evaluator 的除法 `fmaxf(pdf, 1e-7f)`）。

**功能**：防止 PDF 为零时除零。

**现状**：所有地方统一用 1e-7 作为 floor。

**原始审计**：指出四个问题——
1. emissive 切线 PDF 方向反了（cos=0 时正确极限是 pdf→∞，当前返回 1e-7 给出 MIS weight≈1）
2. NEE 除法外层 fmaxf 是死代码（内层函数已保证 ≥1e-7）
3. PDF 浮点下溢时 epsilon 分母可能生成极大 contribution
4. MIS ratio 两个 PDF 同时 Inf 时产生 NaN

**评审分析**：同意前两点。对后两点做了更深入查证——
- 第 3 点：查完 Vose 构建全流程，pdf 掉到 floor 以下的现实通路是极暗非零像素被选中（概率本身极小），floor 效果是除以偏大分母造成轻微低估，不会放大。"epsilon 分母造成巨大 contribution"需要数学正值浮点下溢到 0（lum ~1e-38 量级），理论存在但极端。
- 第 4 点：需要两个 pdf 同时溢出，brdf pdf 现实上界约 1e8，单边 Inf 时 ratio 形式正确给出 0。基本不可达。

**两者分歧**：问题方向一致；审计对第 3/4 点的风险评估偏高，评审认为风险极低。

修复选项（四个表现来自同一缺口，应一次定策略）：

| 方案 | 做法 | 优势 | 劣势 |
|------|------|------|------|
| 维持 floor | 保留 1e-7 下限，修正 emissive cos=0 方向（改返大值），清理外层死代码 | 改动最小 | floor 在极端通路仍会轻微低估 |
| 改 discard | `env_pdf` 返回真值；`emissive_light_pdf` cos≤0 返回 +Inf；evaluator 里 pdf ≤ 0 或非有限时丢弃；删外层死代码 | 语义更干净；ratio MIS 对 Inf 自然给出 weight 0 | 改动面更大 |

两种方案都需要删掉外层 fmaxf 死代码。当前 floor 没有已知的实际画质问题。

---

### 6. 低 throughput 截断

**代码位置**：`programs.cu:193-196`。

**功能**：throughput 过低时终止路径。

**现状**：`fmaxf(throughput.x, fmaxf(throughput.y, throughput.z)) < 1e-6f` 时终止。

**原始审计**：`< 1e-6` 不是非法值，是合法低能量路径，截断引入额外 bias。无效 BRDF sample 已显式产生精确零 throughput；低能量路径有 RR 处理。

**评审分析**：同意。补充：这个检查的真实用途是识别 invalid BRDF sample 的精确零标记，改成 `== 0` 语义更诚实。实际 bias 极小（RR 从 bounce 2 接管），卫生项。

**两者分歧**：无。

修复选项：改为精确零判断；或保留现状（实际 bias 极小）。

---

### 7. RNG [0,1) 契约违反

**代码位置**：`rng.cuh:172,180`；host 镜像 `renderer.cpp:72`。

**功能**：把 32-bit 整数映射到 [0,1) 浮点。

**现状**：`static_cast<float>(h) / static_cast<float>(0xFFFFFFFFu)`。`float(0xFFFFFFFFu)` 精确等于 2^32（4294967295 不可表示，舍入向上），顶部若干整数经此映射输出恰好 1.0f。现有调用点靠 min 等防住越界，但契约写的是 [0,1)。

**原始审计**：指出违反契约。

**评审分析**：确认。依据：IEEE 754 舍入规则。

**两者分歧**：无。

**修复方式**：标准做法是 `(h >> 8) * 0x1p-24f`（24-bit 尾数精确映射到 [0,1)），host/device 同步。

---

## 二、冷路径不变量缺口

### 8. UV 无 finite 检查

**代码位置**：`scene_loader.cpp:347`。

**现状**：TEXCOORD_0 直接写入 vertex，不检查 NaN/Inf。同函数里 position/normal/color/tangent 全都检查了。

**原始审计**：指出此缺口。

**评审分析**：确认，同函数其他属性全检查了唯独 UV 漏了，是 Step 14.5 资产校验的明确遗漏。

**两者分歧**：无。

**修复方式**：与同函数其他属性一致——加 finite 检查，NaN/Inf 替换为 (0,0)。

---

### 9. Index 范围未校验

**代码位置**：`scene_loader.cpp:413-425`。

**现状**：index 直接上传 GPU。emissive collector 自己检查范围，但 GPU 的 vertex fetch（closesthit/anyhit）无保护。

**原始审计**：指出此缺口。

**评审分析**：确认。

**两者分歧**：无。

**修复方式**：加载时校验 index count 是 3 的倍数、每个 index < vertex count。

---

### 10. Transform 无 finite/奇异检查

**代码位置**：`scene_loader.cpp:668-681`（`build_mesh_instances`）。

**现状**：world transform 直接用于 instance、AABB 和 emissive 世界空间顶点。

**原始审计**：指出此缺口。

**评审分析**：确认。

**两者分歧**：无。

**修复方式**：检查 transform 矩阵各元素 finite、行列式非零/非退化。

---

### 11. Vertex color 缺上界

**代码位置**：`scene_loader.cpp:364,376`。

**现状**：只做 `max(color, 0)`。

**原始审计**：指出缺上界，base color 可被推到 1 以上。

**评审分析**：确认。查证了 glTF 2.0 spec（`Specification.adoc:1310`）："All components of each `COLOR_0` accessor element **MUST** be clamped to `[0.0, 1.0]` range。"

**两者分歧**：无。审计只说了问题，评审补充了 spec 依据。

**修复方式**：改为 `clamp(color, 0, 1)`。

---

### 12. Tangent handedness / 单位长度缺校验

**代码位置**：`scene_loader.cpp:389-407`。

**现状**：只查 finite/非零。

**原始审计**：指出未保证 handedness 为 ±1、tangent 与 normal 正交、tangent 投影长度有效。

**评审分析**：部分同意。handedness 和单位长度有 spec 依据——spec 1306：W "**MUST** be set to 1.0 or -1.0"；spec 1289：XYZ "normalized"。正交性部分见第 15 条（评审认为不需要做）。

**两者分歧**：审计要求更多（含正交性），评审只认可 handedness 和单位长度。

**修复方式**：W 不为 ±1 时视为无效 tangent（走 MikkTSpace 重生成）；XYZ 归一化或超出合理范围时同样视为无效。

---

### 13. 无 NORMAL 资产违反 spec

**代码位置**：`scene_loader.cpp:333-337`。

**现状**：无 NORMAL 时填默认 +Z，且不忽略已有的 tangent。

**原始审计**：未提及。

**评审分析**：查证中新发现。glTF 2.0 spec 1382："When normals are not specified, client implementations **MUST** calculate flat normals and the provided tangents (if present) **MUST** be ignored。" 当前实现违反两个 MUST。

**修复方式**：无 NORMAL 时从 position 计算 flat normals（逐三角形面法线写入三个顶点）；设 `has_tangent = false` 忽略已有 tangent。

---

### 14. Alias 聚合 double→float 溢出

**代码位置**：`env_alias_table.cpp:56`、`emissive_alias_table.cpp` 对应行。

**现状**：`static_cast<float>(weight_sum)` / `static_cast<float>(power_sum)`。极端 HDR 或实例数量可使 double 累计值超出 float 范围变成 Inf。

**原始审计**：指出此缺口。

**评审分析**：确认。

**两者分歧**：无。

**修复方式**：转换后检查 `std::isfinite`，非有限时 log 警告并使用归一化尺度。

---

### 15. 逐像素 Gram-Schmidt

**代码位置**：`math_utils.cuh:130-149`（`get_shading_normal`）。

**功能**：TBN 按 `cross(N, T) * handedness` 构造。

**现状**：不做正交化。

**原始审计**：建议加逐像素 Gram-Schmidt，理由是合法插值和非均匀变换可能使 tangent 与法线不再正交。

**评审分析**：认为现状与两份一手材料一致——
- glTF 2.0 spec 1386 强制公式 `bitangent = cross(normal.xyz, tangent.xyz) * tangent.w`，无正交化步骤，与当前实现一致。
- MikkTSpace 头文件原文（`mikktspace.h:117-124`）："the normal map sampler must use the exact inverse of the pixel shader transformation ... using, directly, the 'unnormalized' interpolated tangent, bitangent and vertex normal"。运行时正交化会偏离 baker 假设。

**两者分歧**：审计建议做，评审认为不该做（spec 和 MikkTSpace 约定都不要求正交化）。

---

## 三、冗余 / 无消费者工作

### 16. DLSS OFF 仍生产全部 aux data

**代码位置**：closesthit `programs.cu:569-611`（aux G-buffer 写入块）和 raygen `programs.cu:331-375`（MV + sky aux 写入块）。

**功能**：为 DLSS-RR 生产 guide data（depth、motion vectors、diffuse/specular albedo、normals、roughness）。

**现状**：没有 `dlss_enabled` 门控。DLSS 关闭时仍然执行全部 aux 写入、当前与前帧两次 VP 投影、MV 计算。

**原始审计**：指出无消费者。建议用 launch-uniform 的 `dlss_enabled` 分支整体禁用。

**评审分析**：同意。aux textures 只在 DLSS-RR evaluate 路径读取（`renderer.cpp:823-840`），DLSS OFF 时无消费者。launch-uniform 分支不产生 warp divergence。

**两者分歧**：无。

**修复方式**：closesthit 和 raygen 的 aux 写入块外围加 `if (params.dlss_enabled)` 门控。

---

### 17. NgdotL ≤ 0 时仍然 trace shadow ray

**代码位置**：`nee.cuh:270-288`（env NEE）和 `nee.cuh:373-396`（emissive NEE），以及 `pt_common.cuh:160-162`（`shadow_terminator_factor`）。

**现状**：NgdotL ≤ 0 的判断发生在 shadow ray、纹理和 BRDF/PDF 工作之后（在 `shadow_terminator_factor` 内部），最终返回零。

**原始审计**：建议在 traversal 前做几何半球拒绝。

**评审分析**：同意。正确性中性（最终结果相同），纯删工作。

**两者分歧**：无。

**修复方式**：在 `evaluate_env_nee` 和 `evaluate_emissive_nee` 的 shadow ray 之前加 `if (dot(N_face, L) <= 0) return 0`。

---

### 18. Pass-through 判断过晚

**代码位置**：`programs.cu:408-496`。

**现状**：在判断穿透之前，已经完成了全部顶点字段读取、position/normal/UV/color/tangent 插值、世界空间变换。穿透只需要 position（面法线）、ray direction、hit distance 和 double_sided 标记。

**原始审计**：指出此冗余。

**评审分析**：同意。穿透路径纯删工作，正常路径零变化。

**两者分歧**：无。

**修复方式**：代码重排——先读 position 算面法线 + 判断穿透，再做其余插值。

---

### 19. 纯金属仍然保留 diffuse 策略

**代码位置**：`brdf.cuh:635`（`p_spec` 上限 0.99）；`brdf.cuh:726-735`（diffuse eval）；`brdf.cuh:825-870`（diffuse 采样分支）。

**现状**：纯金属的 `diffuse_weight == 0`，但 `p_spec` 被限制在 0.99，约 1% 的 sample 进入 diffuse 分支产生零 throughput。eval/PDF 也继续计算 EON 相关项。E_glossy 的三通道 39 系数多项式在所有 bounce 上都算，但纯金属只有 bounce 0 aux 写入需要。

**原始审计**：指出 `p_spec = 1` 是合法确定性策略。

**评审分析**：同意。combined pdf 同步退化为 pdf_spec，MIS 一致性保持。`diffuse_weight == 0` 时 diffuse 分支 throughput_update 恒零（`brdf.cuh:858-861`，分子含 `diffuse_weight`）。

**两者分歧**：无。

**修复方式**：`init_brdf_params` 里 `metallic >= 1` 时 `p_spec = 1.0`（跳过 0.99 clamp）；纯金属跳过 E_glossy 除非 DLSS bounce 0。

---

### 20. 最后一个 bounce 仍然构造 next-ray 状态

**代码位置**：`programs.cu:612-666`。

**现状**：`bounce == max_bounces - 1` 时，emissive 和 NEE 仍需计算，但 3 次 BRDF RNG、`brdf_sample()`、next direction、throughput update、BRDF 方向 env PDF + MIS weight 永远不会被消费。

**原始审计**：指出此冗余。

**评审分析**：同意。但指出 RR 下到达 max_bounces-1 的路径少，收益主要在低 max_bounces 档位。

**两者分歧**：问题一致，对收益预期不同。

**修复方式**：closesthit 里 `bounce == max_bounces - 1` 时跳过 BRDF 采样 + env MIS weight 部分。需要通过 payload 传入 `max_bounces` 或利用现有的 bounce payload（p17）。

---

### 21. 全黑环境仍然执行 env NEE

**代码位置**：`nee.cuh:245-291`，`evaluate_env_nee` 入口。

**现状**：`total_luminance ≤ 0` 直到 `env_pdf()` 内才处理（`nee.cuh:94-96`），此前已生成 RNG、读取 alias table、执行 shadow traversal。

**原始审计**：指出此冗余。

**评审分析**：同意。

**两者分歧**：无。

**修复方式**：`evaluate_env_nee` 入口增加 `env.total_luminance <= 0` 检查并提前返回零。

---

### 22. V_ts 替换 BrdfParams::V

**代码位置**：`brdf.cuh:651`（`BrdfParams::V`）；`brdf.cuh:759,791,823`（`brdf_eval/pdf/sample` 各自调 `world_to_tangent`）。

**现状**：同一个 closesthit 内 `world_to_tangent(T, B, N, V)` 最多执行 5 次（brdf_sample 1 次 + 两个 NEE 各 eval/pdf 各 1 次）。两次 NEE 之间隔着 shadow ray optixTrace，编译器无法跨 trace 优化。

**原始审计**：指出 V_ts 重复构造。建议在 BrdfParams 新增 V_ts.x 和 V_ts.y（加 8 字节）。

**评审分析**：同意要做，但改方案——三个消费者都只拿 V 做 world_to_tangent，没有任何调用点需要世界空间的 V（grep 全部 `params.V` 使用点确认）。应**替换** V 而非新增字段，同尺寸不增加活跃状态。

**两者分歧**：方向一致，形态不同。审计加字段（+8 bytes），评审替换字段（±0 bytes）。

**修复方式**：`BrdfParams::V` 改为 `BrdfParams::V_ts`，`init_brdf_params` 里一次性计算存入，`brdf_eval/pdf/sample` 直接使用。

---

### 23. brdf_eval 和 brdf_pdf 合并

**代码位置**：`brdf.cuh:757-798`（两个独立函数）；调用点 `nee.cuh:283-284` 和 `nee.cuh:388-394`。

**现状**：两个函数各自独立计算 L_ts（3 dot products）、H_ts（normalize = 3 加法 + dot + rsqrt）、NdotH，全部重复。

**原始审计**：建议新增融合函数 `brdf_eval_and_pdf`，共享中间量，保留原函数供非成对调用点使用。

**评审分析**：同意融合，但查过所有调用点——`brdf_eval` 和 `brdf_pdf` 只在两个 NEE evaluator 里出现且全部成对（`nee.cuh:283-284` 和 `nee.cuh:388-394`），没有单独调用点。可以直接替换原两个函数（函数总数减一），也可以保留原函数作备用。

**两者分歧**：审计保留原函数（总数+1），评审认为可以替换（总数-1），因为没有单独调用点。

**修复方式**：新建 `brdf_eval_and_pdf(params, L, NdotL)` 返回 `{float3 value, float pdf}`。原两个函数是保留还是删除取决于是否预期未来有单独调用点。

---

### 24. Env light sample 直接返回 PDF

**代码位置**：`nee.cuh:44-75`（`sample_env_alias_table` 返回 world_dir）；`nee.cuh:90-118`（`env_pdf` 做逆映射求 PDF）；`nee.cuh:262-286`（evaluator 先采样得 world_dir，再旋转回 env space 查 cubemap，再调 `env_pdf` 逆映射回 UV 查 luminance）。

**现状**：alias sampler 知道选中的 pixel 和 env-space direction，但只返回 world_dir。evaluator 随后把 world_dir 旋转回 env space（二次旋转，与 sampler 内部的逆旋转互消），然后 `env_pdf` 再做 atan2/asin 逆映射回 UV、反查 pixel luminance——这个 luminance 就是 sampler 里已经查过的那个。

**原始审计**：指出反向重建冗余。建议 sample 直接返回 world direction、env direction 和 PDF。

**评审分析**：同意。补充：除了省计算外，消除了"正逆映射必须严格互逆"的不变量负担。`env_pdf` 保留，只服务 closesthit 里 BRDF 方向的 env MIS weight 查询（`programs.cu:638`）。

**两者分歧**：方向一致，评审补充了结构性理由。

**修复方式**：`sample_env_alias_table` 增加返回 env_dir 和 pdf_light；evaluator 直接使用，省去正旋转和 `env_pdf` 调用。

---

### 25. Primary ray homogeneous divide 冗余

**代码位置**：`programs.cu:218-222`（`compute_primary_dir` 里的 `inv_w = 1/view_target.w`）。

**现状**：对目标点 `(ndc_x, ndc_y, 1, 1)` 做逆投影，除以 w，再 normalize。homogeneous divide 只是给 direction 乘了一个统一尺度，normalize 会消除掉。

**原始审计**：指出冗余。提到需要确认 w 符号安全性。

**评审分析**：确认冗余，并验证了 w 符号恒正——`glm::perspective` 默认 RH_NO（全仓库无 `GLM_FORCE_DEPTH_ZERO_TO_ONE`，`camera.cpp:24-27` 注释明确标准投影），对目标点 (x,y,1,1) 逆投影得 `w_view = 1/far`。normalize 吸收正尺度。

**两者分歧**：方向一致，评审补充了安全性验证。

**修复方式**：删除 218-222 行的 `inv_w` 计算和乘法，直接用 `view_target.xyz` 做后续变换。注释说明依赖"ndc_z=+1 + glm::perspective"约定。

---

### 26. Tonemap 分辨率比例每像素重复计算

**代码位置**：`tonemap.cu:153-154,164-165`。

**现状**：render/display 分辨率比例是 launch-uniform 常量，但每个 output pixel 执行两个浮点除法。

**原始审计**：指出冗余。

**评审分析**：同意。

**两者分歧**：无。

**修复方式**：`tonemap_kernel` 参数增加 `scale_x` 和 `scale_y`，host 预计算。

---

## 四、卫生类清理

### 27. 重复 alpha 下限

**代码位置**：`programs.cu:514-515`。

**现状**：`roughness >= 0.04` 已保证 `alpha >= 0.0016`，第二个 `fmaxf(roughness*roughness, 1e-4f)` 永远不起作用。

**原始审计**：指出死代码。

**评审分析**：确认全输入域死代码。MR 纹理走 BC7_UNORM（`texture.cpp:294`，UNORM 无法编码 NaN/Inf）；默认纹理为常量 [0,1]（`cuda_texture_upload.cpp:177-183`）；factor 已 spec_clamp（`scene_loader.cpp:600`）。全链条无 NaN 通路。

**两者分歧**：无。

**修复方式**：删除第二个 fmaxf。

---

### 28. Anyhit opaque 守卫

**代码位置**：`programs.cu:692-694`。

**现状**：`alpha_mode == 0` 时 return。opaque 几何是否调用 anyhit 已由 BLAS flag 决定，纯契约防御。

**原始审计**：指出此为上游契约防御。同时指出当前此分支在 stochastic alpha 接入前有避免 Blend 材质空工作的实际语义（`alpha_mode != Mask` early exit）。

**评审分析**：确认。清理时需注意上述副作用。

**两者分歧**：无。

**修复方式**：删除或改为 assert。注意 stochastic alpha（Step 16）接入前此分支兼有 `alpha_mode != Mask` early exit 的实际语义。

---

## 五、评审分析认为不需要改动的项

### World position 改 ray equation 重构

**原始审计**：建议用 `origin + t * direction` 代替重心插值重建 hit point，减少一次 point transform。

**评审分析**：RTG Ch.6（本项目 `offset_ray_origin` 的来源论文）原文："We **strongly advise against** this procedure, as the resulting new origin may be far off the plane of the surface." 论文推荐做法就是当前实现（重心插值 + object-to-world transform）。而且顶点 position 本来就要读（面法线需要），省下的只是三次 FMA 插值加一次 point transform。

---

### 逐像素 Gram-Schmidt

见第 15 条。

---

### Payload 携带 SER material hint

**原始审计**：列为候选，但自身也不建议改（closesthit 仍需同记录两个地址，payload 压力高）。

**评审分析**：同意不改。

---

### Blend anyhit 空工作（单独改）

**原始审计**：指出当前 Blend 材质 anyhit 读取 UV/color、采样纹理，但只有 Mask 分支能调用 `optixIgnoreIntersection()`。

**评审分析**：确认冗余存在，但 Step 16 Stochastic Alpha 会给这些工作接上消费者，在那之前单独改是过渡态。

---

## 六、有代码层面代价的候选

以下改动有结构或维护代价，收益主张需要 profiling 兑现。

**原始审计**列出但**评审分析**认为代价超过当前可确认收益的项：

| 项 | 代价 |
|---|---|
| Camera ray basis 预计算 | 给相机几何造第二套表示，违反单一来源（inv_view/inv_projection 因 aux depth/MV 仍需保留） |
| Emissive record 布局改造 | host/device 共享 POD 翻修 |
| Sobol set-bit 迭代改写 | host/device 两份镜像要同步维护，可读性变差 |
| Tonemap box filter reciprocal area | 边缘像素浮点裁剪正确性负担 |

---

## 七、审计列出但评审确认现状合理的部分

以下由原始审计列出的 fallback 经评审核对确认合理：

- 插值法线相消 fallback 到 N_face（`programs.cu:444-451`）
- Tangent 退化 fallback 到 N（`math_utils.cuh:139-142`）
- Normal map Z 重建 `fmaxf(0, ...)` 边界处理（`math_utils.cuh:136`）
- VNDF 下半球返回零 throughput（`brdf.cuh:830-835`）
- Emissive sample dist²=0 返回零贡献（`nee.cuh:345-347`）
- MV clip.w 守卫写零 MV（`programs.cu:352-364`）
- E_ss/E_glossy 多项式 clamp（`brdf.cuh:715-728`）
- Catmull-Rom 负值 clamp（`tonemap.cu:169`）
- 合法空状态（`traversable==0`、`cubemap==0`、`emissive.count==0`、`sample_count==0`）

以下"重计算替代访存"的现有做法经评审核对确认合理：

- Emissive triangle area 从 selection probability 和 PDF 中解析消去
- Emissive geometric normal 由已读取的三个顶点重算
- Path state 通过 payload registers 传递
- Sobol direction table 位于 LaunchParams constant memory
- CLTC 系数和多个 BRDF 中间量重计算而非塞入 BrdfParams
