# DLSS-RR 调试报告（Step 14）

> 临时文档：Step 14 调试结论整理。修复收尾后可清理。
>
> 代码未落地——工作区已重置到 `e0eeef9`，实现时以本报告为准。

---

## 问题现象

开启 DLSS-RR 后出现以下问题：

1. **视觉伪影**：画面上出现扫描式闪动的暗色方框。高 spp 下方框加深并带蓝/黄/红色偏，框内含噪点，左缘有垂直条带将底下内容染成蓝紫色。方框锚定在场景几何体上（跟随物体移动），天空区域不受影响。伪影只出现在 DLSS 输出画面中，直接查看输入数据（noisy color）看不到。
2. **偶发崩溃**：NGX 内部 `TransitionResourceImpl_Pure` 处触发 illegal memory access。
3. **Debug overlay 上下颠倒**：DLSS debug overlay 的渲染行序颠倒。
4. **崩溃时日志被吞**：NGX 内部错误不可见，崩溃时 spdlog 来不及 flush，排查困难。

---

## 根因定位过程

通过逐输入替换法二分定位：

1. 用全合成中性输入（灰色常量 + 零 MV + 固定矩阵等）喂给 DLSS → 画面干净，无方框
2. 逐项还原真实输入 → 仅还原 color 时方框立刻复现
3. 保留灰色 color + 真实的其余所有 guide buffer → 画面干净
4. 对 color 做负值钳制（`fmaxf(c, 0)`）后 → 方框消失

**结论：noisy color 中的负值辐射度是方框的唯一触发源。**

---

## 已确认的修复方案

### 修复 2：Color 输出负值钳制

**问题**：扫描式闪动暗色方框（上述根因）。

**原因**：渲染管线中某处产生了负值辐射度样本，写入 DLSS 的 color 输入。此前的 `isfinite` 检查和幅值上限 clamp 都拦不住负值（负有限数既通过 `isfinite` 又低于上限）。

**修复**：raygen DLSS 路径 color 写入前加 `fmaxf(c, 0)` 负值钳制，与 `isfinite` 检查在同一位置。

**注意**：这是症状层面的修复。负值的真正源头需要定位并在源头修复（见下方"正确性问题 1"）。


---

## 正确性问题（必须修复）

以下问题无论当前是否有可见症状都必须修复，否则渲染结果存在正确性缺陷。

### 1. 负值辐射度的源头需要定位（最高优先级）

修复 2 只是在 DLSS 写入点钳制负值，治标不治本。负值样本同样会污染：

- **DLSS OFF 模式的 Separate Sum 累积**——累积路径没有钳制，长期累积负值会造成系统性偏暗（有 bias 的错误结果）
- 任何依赖"辐射度 ≥ 0"假设的下游逻辑

**可能的源头**：

- EON diffuse 多项式在极端角度（接近掠射）下算出负值
- BC6H signed 格式解码出微小负 texel
- CLTC 或 E_FON 拟合多项式的边角输入
- Shadow terminator 因子在特殊几何下输出负值

**静态代码审计发现：Shadow Terminator 修正存在确定的负值路径。**

`shadow_terminator_factor()` 的接口契约声明返回 `[0,1]`，实现却直接计算：

```cpp
const float G = NgdotL / (NdotL * dot(N_geo, N_shading));
return G + G * G - G * G * G;
```

实现中的分支只检查 `NgdotL > 0` 且 `NgdotL < NdotL`，这不足以保证注释声称的 `G ∈ (0,1)`，因为分母还包含 `dot(N_geo, N_shading)`。当 `G > (1 + sqrt(5)) / 2` 时，多项式 `G + G² - G³` 为负。该情况在几何法线、着色法线和光照方向均为有效单位向量时也能出现，并非仅由非法输入触发。

该函数依赖的法线一致性前提也与调用点不一致：`programs.cu` 使用 `ensure_normal_consistency(N_mapped, N_interp)` 修正着色法线，但调用 `shadow_terminator_factor()` 时传入的几何法线是 `N_face`。因此，函数注释所依赖的 `dot(N_geo, N_shading) > 0` 并未由上游保证；该点积接近零或为负时，`G` 还会进一步越界或变号。

返回的修正因子直接乘入环境光 NEE 和 emissive NEE 的 `nee_radiance`，随后经 payload 进入 `path.radiance`，构成从该函数到最终 color 负值的完整传播路径。

**建议做法**：在 device 端做临时统计（负值出现频率 × 归属调用点），精确定位后在源头修复。源头修好后再评估是否仍保留写入端钳制作为防御层。

### 2. E_glossy 缺失 clamp（潜伏 bug）

排查过程中发现。与方框伪影无直接关联，但是独立的正确性缺陷。

**问题**：`brdf.cuh` 中 `E_glossy()` 的文档契约写明返回值在 [0,1]，但实现未做 clamp。39 系数有理多项式的分母含 ±400 量级系数，靠正负相消平衡——在 (F0, roughness, NdotV) 的单位立方体内存在过零等值线，分母可以变成零甚至翻转符号，导致输出 ±Inf 或 NaN。

**影响**：

- 污染 `diffuse_weight`（渲染正确性：diffuse 层能量权重错误）
- 污染 DLSS specular albedo guide buffer

**修复**：返回值 clamp 到 [0,1]。`fmaxf` 同时可以吞掉 NaN。

**参照**：`init_brdf_params` 中对 `E_ss` 已有调用方 clamp，`E_glossy` 是对称的遗漏。

### 3. N_shading NaN（潜伏 bug）

同样是排查中发现，与方框无关。

**问题**：镜像 UV 接缝处，相邻顶点的 tangent 方向相反，插值后可能相消为零向量。`normalize(零向量)` 结果为 NaN。顶点法线插值在极端退化网格上也有同样风险。

**影响**：

- NaN 传播到整条 BRDF 计算链
- 污染 DLSS normals guide buffer（Streamline 指南明确要求 "shading normals must be normalized"）

**修复**：N_shading 计算完成后做 `isfinite` 检查，退化时回退到 face normal。

**可选加固**：`get_shading_normal` 中 z 重建（`sqrt(1-x²-y²)` 在 x²+y² > 1 时对负数开方）未审计；SceneLoader/mikktspace 对退化 UV 的 tangent 输出可考虑加载期检测。

### 4. DLSS ON 时 color 输出必须写均值

**问题**：D32 明确规定 DLSS 的 color 输入是"同一亚像素位置 N 个 sample 的辐射度**平均**"。如果 raygen 写的是总和而非均值，画面会亮 spp 倍。

**修复**：raygen DLSS 路径在写 color 前除以 `samples_per_frame`（除数要防 0）。帧末 write-slot count 置 1（以防切回 DLSS OFF 时 tonemap 除数正确）。

### 5. Aux buffer 需要改为 ping-pong 双份

**问题**：当前 aux buffer（depth、MV、albedo 等）各只有一份。在并行架构（compute_stream + display_stream 同时工作）下，raygen 在 compute_stream 上写当前帧的 aux，同时 DLSS eval 在 display_stream 上读上一帧的 aux——两条 stream 同时访问同一份 buffer，存在跨 stream 的读写竞争。此外，guide 数据和 color 来自不同帧会导致时域错配。

**修复**：aux buffer 全部改为与 color 一样的 ping-pong 双份，共用 `accum_index` 同步翻转。跨帧保护复用现有 event 链。配套新增 `DlssPrevFrame` host 缓存（上一帧的 jitter、view/projection 矩阵、frame_time），确保 evaluate 消费的是完整配套的 N-1 帧数据集。

### 6. DLSS ON/OFF 模式间的 jitter 一致性

**问题**：DLSS 模式下需要全局统一 jitter（host 端 Sobol 不加 per-pixel Cranley-Patterson rotation），使所有像素共享同一个 jitter 偏移，与 `InJitterOffset` 一致。per-pixel rotation 会使 DLSS 收到的单一 jitter 值失真。DLSS OFF 模式则需要恢复 per-sample jitter 以加速收敛。

---

## 待定事项

以下事项需要后续实验或决策，非阻塞修复。

### 1. blocking stream 回测

修复 1 中将 display_stream 改为 blocking 可能引入不必要的序列化点（blocking 流与 null stream 隐式同步）。负值修复稳定后回测 non-blocking：稳定则恢复以消除性能隐患，复现则保留并记录。

### 2. InCUStream 选择

实测 display_stream 和 0（null stream）都能跑。需要与 blocking 回测联动决策后写入文档。

### 3. 矩阵传递约定

静止相机 + DLAA（1:1 不放大）场景下，转置与否无可见差异（矩阵未被显著使用）。但两个 NVIDIA 官方参考的做法互相矛盾：

- vk_gltf_renderer：`dlss_wrapper.cpp:488-491` 做了转置
- vk_denoise_dlssrr：不转置，并附注释给出"双转置抵消"的论证

SDK 约定"Row Major + left multiplication"按推导支持**不转置**。需要在相机运动 + upscale 场景做 A/B 对比后定稿，并更新 `current-phase.md` 矩阵传递节（当前文档记载的是 vk_gltf 的转置做法，与推导矛盾）。

### 4. Sky depth 用 INF 还是 65504

当前实现（D32 规格）写 IEEE float infinity。调试过程中作为 NaN 源嫌疑改成了 65504.0（FP16 max，vk_denoise_dlssrr 的做法），但**未解决方框**——真凶是负值，不是 INF。INF 产生问题的前提条件从未被验证。65504 与参考约定对齐且无成本，可选采用。定稿后更新 D32。

### 5. Specular albedo guide 有效性复核

方框存在期间观察到"specular albedo 置零无视觉差异"，但该观察可能被伪影掩盖。DLSS 正常工作后需要复核 specular albedo guide 的实际影响，D32 的 E_glossy 选型（vs SDK 附录的 EnvBRDFApprox）画质对比留待调优阶段。

### 6. DLSS OFF 路径的 NaN 防御

Separate Sum 下单个 NaN 样本会永久"卡死"该像素——NaN 加到 sum 里后 sum 永远是 NaN。负值源头修复后评估是否需要与 DLSS 路径对称的 sanitize。

### 7. 并行消费路径的回归验证

并行消费逻辑（read slot + prev 缓存）是在方框存在期间实现的，从未在"无方框"状态下验证过。落地后需专项确认无新伪影——时域一致性依赖 prev jitter/矩阵/aux 三者配套正确。