# 渲染管线只读审查报告

## 1. 审查约束

本报告记录对 Qualquer 渲染管线的只读审查结果。审查目标包括：

- 完整追踪场景上传、加速结构、OptiX 管线与 SBT、逐帧 launch、DLSS-RR、tonemap、CUDA–Vulkan 同步和呈现；
- 识别确定错误、次优实现和优化机会；
- 检查既有优化是否可能在 Ada 及更新 GPU 架构上产生反作用；
- 区分静态代码能够证明的问题与必须通过画面对照、编译资源报告或 Nsight 验证的性能判断。

审查不修改源码，不执行构建或运行。临时下载的参考资料在使用后清理。

## 2. 审查基线

- 审查基线：`061e55ac65340b1feb3a8857bfa6db4b5493c46a`
- `cc05acedbb3e3eff14394f5dd4eb9e9c1b36a319` 已完整移除 device 和 DLSS jitter 的 golden-ratio temporal offset。
- 当前源码与 `cc05ace` 一致；`061e55a` 只更新文档。
- 先前出现的五参数 `sobol_rng` 调用属于提交过程中的中间状态，当前代码不存在该问题。

## 3. 已确认问题

---

### QRP-004：改变 `max_bounces` 后继续混合旧累积与 DLSS history

- 严重度：高
- 置信度：高
- 类型：累积历史失效

#### 代码证据

- `renderer/src/renderer.cpp:659-681` 明确把 `max_bounces` 排除在 reset key 之外，理由是其不改变积分对象。
- `renderer/src/device/programs.cu:112` 以 `params.max_bounces` 截断路径。

#### 判断

最大路径深度改变被估计的光传输项集合，属于积分定义变化，不只是吞吐旋钮。将不同 `max_bounces` 的样本累积到同一 Separate Sum 中会混合不同估计目标。

#### 触发条件

已有累积或 DLSS temporal history 后修改 `max_bounces`。

#### 影响

- DLSS OFF：旧深度与新深度样本永久混合，除非发生其他 reset；
- DLSS ON：明显变化的 noisy signal 延续旧 temporal history，可能出现拖影、收敛延迟或亮度残留。

---

### QRP-005：动态修改 spp 会重复或回退 Sobol sequence index

- 严重度：中高
- 置信度：高
- 类型：采样序列正确性

#### 代码证据

- `renderer/src/device/programs.cu:270-273` 与 `renderer/src/device/programs.cu:286-288` 均计算：`sequence_index = frame_index * samples_per_frame + s`。
- `renderer/include/qualquer/renderer/rng.cuh:163-178` 将该值直接用作 Sobol index / hash 输入。
- `renderer/src/renderer.cpp:659-681` 允许 `samples_per_frame` 动态改变而不 reset。
- `Renderer::frame_counter_` 在暂停/空场景等没有新 sample 的 frame 也继续递增，而 sequence index 不使用真实累计 sample base；暂停后恢复会跳过整段 Sobol points。

#### 判断

只有 spp 固定时，`frame_index * spp + s` 才形成连续且唯一的样本索引。spp 改变后，该公式可以回退、重复或跳过索引。

#### 触发示例

前两帧 spp 为 2 时使用索引 `2,3`、`4,5`；后续帧把 spp 改为 1 时，公式可重新生成已使用的索引。

#### 影响

- 重复路径样本降低有效样本数并破坏 Sobol 序列质量；
- 当前 UI 手动调节 spp 可触发；
- 暂停 accumulation、长时间使用 DLSS 后切回 fallback，或空场景停留后加载场景会形成与实际收集样本数无关的大段跳跃；
- 后续自适应 spp 若逐帧变化会持续触发。

#### 长时运行边界

`frame_index * samples_per_frame` 与 `sample_count + effective_spp` 都在 `uint32_t` 中运算。即使 spp 固定，累计到 `2^32` samples 后也会回绕；64 spp、60 FPS 下约 13 天即可达到。sequence 会重复，Separate Sum 的 count 还会变成 0，使 tonemap 短暂输出黑并让后续 raygen误进入覆盖模式。

#### 与 temporal offset 提交的关系

该问题与移除 golden-ratio temporal offset 相互独立。`cc05ace` 已正确同步全部调用点；问题在于作为跨帧推进来源的 `sequence_index` 未保存累计样本基数，且缺少明确的溢出策略。

---

### QRP-006：单面材质背面在 bounce ray 与 shadow ray 中语义不一致

- 严重度：高
- 置信度：高
- 类型：可见性 / 阴影正确性

#### 代码证据

- `renderer/src/device/programs.cu:447-489` 对普通 radiance ray 命中的单面背面执行 pass-through。
- 该 pass-through 通过再次追踪实现，因此每层仍消耗当前路径的 bounce 预算。
- `renderer/src/device/programs.cu:489-490` 的注释明确指出 shadow ray 使用 `TERMINATE_ON_FIRST_HIT` 且禁用 closest-hit，无法执行同样的 pass-through。

#### 触发条件

从背面看到单面几何，或从着色点到光源的 shadow ray 穿过单面几何背面；多层重叠单面几何会放大问题。

#### 影响

- radiance ray 穿过每层不可见背面都消耗一次 bounce，可能在到达实际可见表面前耗尽 `max_bounces`；到第三层起还会执行原本只应作用于真实散射事件的 Russian Roulette，增加无意义终止/补偿方差并推进后续 Sobol bounce dimensions；
- shadow ray 把同一背面视为完全不透明遮挡，导致错误暗影和直接光缺失；
- 同一材质在相机/间接路径与 NEE 可见性中的语义不一致。

---

### QRP-007：运行时环境图加载失败后，旧环境已销毁但累积未重置

- 严重度：中高
- 置信度：高
- 类型：失败路径 / history invalidation

#### 代码证据

- `app/src/scene_loader.cpp:804-805` 的 `load_env_map()` 在验证新资源前先调用 `destroy_env_map()`。
- `app/src/application.cpp:234-250` 仅在运行时加载成功分支调用 `renderer_.reset_accumulation()`；失败分支只设置错误消息。
- 启动和 `switch_scene()` 后的 env reload（`application.cpp:129-133,562-566`）还忽略 `load_env_map()` 的 false 返回值；资源已清空时 UI/config 仍可显示旧路径。

#### 触发条件

运行时已有有效环境图和累积结果，随后选择损坏、不支持或无法读取的新环境图。

#### 影响

场景照明实际上从旧环境变为无环境，但 Separate Sum 和 DLSS history 仍含旧环境贡献，直到其他事件触发 reset；UI/config 与实际 GPU resource 还可能不一致。旧贡献会错误保留或逐帧淡出，而不是立即反映失败后的真实状态。

---

### QRP-008：`mis_power_heuristic()` 将单侧无穷 PDF 错误处理为 0.5

- 严重度：中
- 置信度：高
- 类型：数值边界

#### 代码证据

- `renderer/include/qualquer/renderer/pt_common.cuh:129-140` 计算 `r = pdf_b / pdf_a`，随后对任意 `!isfinite(r)` 返回 `0.5`。
- 注释只想处理 `Inf / Inf -> NaN` 的对称退化情形，但条件同时捕获普通正无穷。

#### 触发条件

`pdf_a` 为有限正数、`pdf_b` 溢出或成为正无穷时。

#### 影响

正确的 strategy-A 权重应趋近 `0`，当前返回 `0.5`，会错误放大该策略贡献。极端距离、掠射角或异常材质参数会提高触发概率。

---

### QRP-009：发光三角形 shadow-ray 终点空隙与距离成比例

- 严重度：中
- 置信度：高
- 类型：可见性鲁棒性

#### 代码证据

- `renderer/include/qualquer/renderer/nee.cuh:438` 与 `renderer/include/qualquer/renderer/nee.cuh:676` 使用 `dist * (1 - 1e-4)` 作为 shadow-ray `tmax`。

#### 判断

终点排除区长度为 `dist × 1e-4`，随光源距离线性增长，而不是依据起点/终点浮点误差或局部几何尺度。

#### 触发条件

远距离发光三角形，且遮挡物位于光源前方最后一段相对空隙内。

#### 影响

可能漏掉合法遮挡物并产生漏光；距离越远，未检测区绝对长度越大。

---

### QRP-010：环境 alias 的 cell 内纬度 jitter 与返回 solid-angle PDF 不一致

- 严重度：中
- 置信度：高
- 类型：采样 PDF 一致性

#### 代码证据

- `renderer/include/qualquer/renderer/nee.cuh:71-81` 在选中 alias cell 后，对 equirectangular `u/v` 均匀 jitter，并以 jitter 后的 `theta` 生成方向。
- 环境 alias 权重由 cell 中心或块代表纬度的 `sin(theta)` 建立：`app/src/env_alias_table.cpp:73-90`。
- 返回 PDF 使用 cell 离散概率到 solid angle 的换算，但未补偿 cell 内均匀 `v` jitter 对应的 `sin(theta_center) / sin(theta_sample)` 差异。

#### 触发条件

环境 importance sampling；alias 分辨率较低或样本靠近极区时误差增大。

#### 影响

实际方向采样密度与报告给 MIS/估计器的 PDF 不完全一致，形成小幅偏差。alias 下采样越激进，单 cell 纬度跨度越大。

---

### QRP-011：空场景早退保留上一场景的 TLAS instance 统计

- 严重度：低
- 置信度：高
- 类型：状态一致性 / UI

#### 代码证据

- `renderer/src/renderer.cpp:524` 只在构造新 TLAS instance 列表后更新 `tlas_instance_count_`。
- 空场景路径在该赋值前早退。
- `renderer/src/renderer.cpp:1205-1206` 直接向 UI 暴露该缓存值。

#### 触发条件

先加载非空场景，再加载没有可渲染 instance 的场景。

#### 影响

统计/UI 继续显示上一场景的 TLAS instance 数量，不反映当前渲染状态。

---

### QRP-012：纹理缓存键不包含转换与压缩实现版本

- 严重度：低至中
- 置信度：高
- 类型：缓存失效策略

#### 代码证据

- `app/src/cache.cpp:43-64` 缓存身份基础是源内容的 XXH3-128。
- `app/src/texture.cpp:317-329` 的缓存查找由 source hash 与目标 format 决定。
- `app/src/texture.cpp:301-306` 的后缀只区分 BC7 sRGB、BC7 UNORM、BC6H 等格式。
- mip 生成、色彩空间处理、bc7enc/ispc_texcomp 参数或实现版本未进入 key。

#### 触发条件

更新纹理预处理算法、压缩器版本、压缩 profile、mip filter 或修复实现后，再加载内容相同的资产。

#### 影响

旧 KTX2 结果会继续被命中，导致代码已经更新但画质/数据仍沿用旧实现。用户必须手动清缓存才能看到变化。

### QRP-013：single-sided primary pass-through 生成互相矛盾的 DLSS guides

- 严重度：高
- 置信度：高
- 类型：DLSS-RR guide 一致性

#### 代码证据

- `renderer/src/device/programs.cu:172-181` 只依据 bounce 0 返回的 `hit_distance >= 0` 判定 `primary_is_hit`，并保存 `cam_origin + primary_dir * hit_distance` 作为 motion-vector 世界位置。
- `renderer/src/device/programs.cu:460-488` 的 single-sided 背面 pass-through 返回正的 `hit_t`，因此 raygen 会把不可见背面判定为 primary surface。
- 同一路径在 `renderer/src/device/programs.cu:468-482` 把 depth、normal、roughness 和 albedo 写成“无表面”默认值。
- 真正可见的后方表面位于后续 bounce，而正常 aux 写入被 `renderer/src/device/programs.cu:633-640` 的 `bounce == 0` 条件排除。

#### 触发条件

DLSS-RR 开启，primary ray 先从背面命中 single-sided 三角形，并继续命中后方表面或环境。

#### 影响

同一像素的 guides 描述了三个不同对象：

- motion vector 使用不可见背面的世界位置；
- depth/normal/roughness/albedo 表示“无表面”；
- noisy color 来自后方可见表面或环境。

这种不一致会破坏 DLSS-RR 的重投影和表面分类，可能造成拖影、边缘闪烁、错误历史接受或拒绝。多层 pass-through 还与 QRP-006 的 bounce 消耗问题叠加。

### QRP-014：合法的无材质 glTF primitive 被当作加载错误

- 严重度：中
- 置信度：高
- 类型：glTF 兼容性

#### 代码证据

- `app/src/scene_loader.cpp:527-531` 在 `primitive.materialIndex` 缺失时直接抛出异常，并要求每个 primitive 必须显式指定材质。
- 异常由场景加载顶层捕获后导致整个场景加载失败，而不是只影响该 primitive。

#### 规范依据

本地 Khronos glTF 2.0 规范 `/mnt/d/Github/glTF/specification/2.0/Specification.adoc:1249` 明确规定：当 primitive 的 `material` 未定义时，**必须**使用 default material。该 default material 在同文件 `2171-2179` 定义为所有字段采用规范默认值的普通材质。

#### 触发条件

加载任意省略 primitive `material` 字段的合法 glTF 2.0 资产。

#### 影响

符合规范的资产被整体拒绝，场景切换还会在旧场景已经销毁后落入空场景。该限制没有记录为项目资产子集约束，并与路线图声明的 glTF mesh/material 加载目标不符。

### QRP-015：所有材质纹理都强制使用 `TEXCOORD_0`

- 严重度：中
- 置信度：高
- 类型：glTF 材质正确性 / 兼容性

#### 代码证据

- `app/src/scene_loader.cpp:345-357` 只读取 `TEXCOORD_0`；`Vertex` 也只保存一套 UV。
- `app/src/scene_loader.cpp:698-714` 解析各纹理引用时只保存 texture index，没有保存各 `textureInfo.texCoord`。
- `renderer/src/device/programs.cu:546-573` 对 base color、metallic-roughness、normal 和 emissive texture 全部使用同一个插值 `uv`；any-hit alpha 在 `renderer/src/device/programs.cu:771` 亦如此。

#### 规范依据

- glTF 2.0 允许 `TEXCOORD_n` 多套属性；本地规范 `Specification.adoc:1278-1315` 建议客户端至少支持两套 texture coordinate。
- 本地 schema `textureInfo.schema.json:13-18` 定义每个 texture info 独立的 `texCoord`，其值选择对应的 `TEXCOORD_<set index>`；默认值才是 0。

#### 触发条件

任一材质纹理显式设置 `texCoord: 1` 或更高，并由 primitive 提供对应 `TEXCOORD_1` 等属性。

#### 影响

纹理会以错误 UV 采样。base-color alpha 的错误 UV 还会让 any-hit alpha mask 与几何可见性错误；normal、metallic-roughness 和 emissive 纹理则分别造成法线、BRDF 与光源辐射分布错误。

### QRP-016：glTF `magFilter` 被完全忽略

- 严重度：低至中
- 置信度：高
- 类型：纹理采样兼容性

#### 代码证据

- `app/src/scene_loader.cpp:168-196` 的 `convert_gltf_sampler()` 只检查 `sampler.minFilter`，从未读取 `sampler.magFilter`。
- CUDA texture object 只有一个 `filterMode` 同时服务放大与单 mip 内过滤；当前实现选择 minification 的 base filter，未记录这一不可表示性或采用显式退化策略。

#### 触发条件

glTF sampler 显式指定的 `magFilter` 与当前由 `minFilter` 推导出的 base filter 不同，或只指定 `magFilter = NEAREST`。

#### 影响

纹理放大时使用错误的 nearest/linear 模式，表现为本应像素化的纹理被平滑，或本应平滑的纹理出现块状边缘。glTF 2.0 要求实现遵循 wrapping，并建议遵循显式 filtering mode；这里属于可观测的材质呈现偏差。

### QRP-017：Blend alpha 当前按完全 opaque 渲染，同时仍支付无效 any-hit 成本

- 严重度：高（材质正确性）/ 中（性能）
- 置信度：高
- 类型：glTF alpha semantics / 确定的无效 GPU 工作

#### 代码证据

- `app/src/scene_loader.cpp:533-534` 只有 `Opaque` 材质标为 opaque；`Blend` 与 `Mask` 都进入非 opaque 路径。
- `optix/src/accel_structure.cpp:27-30` 对所有非 opaque geometry 设置 `OPTIX_GEOMETRY_FLAG_REQUIRE_SINGLE_ANYHIT_CALL`。
- `renderer/src/device/programs.cu:745-772` 的 any-hit 在检查 `alpha_mode` 前无条件读取 index/vertex、插值 UV/alpha 并执行 bindless `tex2D`。
- `renderer/src/device/programs.cu:774-778` 只有 Mask 模式会依据 alpha 调用 `optixIgnoreIntersection()`；Blend 模式当前不改变遍历结果。
- `tasks/phase4.5.md:52-55` 明确把 Blend stochastic alpha 留到尚未完成的 Step 16。

#### 触发条件

当前版本加载含 `alphaMode = BLEND` 的材质。

#### 影响

每个 Blend candidate intersection（包括 shadow ray）都会执行 any-hit、随机纹理访问与插值，但无论 alpha 值为何都接受命中：半透明/透明 texel 在 camera、indirect 与 shadow rays 中均表现为完全不透明，错误遮挡背景与光照。与此同时，该成本会削弱 RT core traversal、增加纹理/L1/L2 压力和 warp divergence；大量树叶、粒子或叠层 Blend 几何时尤其明显。

#### 判断边界

这是任务清单已知、尚未完成的 Step 16，不是隐藏的规划遗漏；但以当前 HEAD 为审查对象时，仍是确定的 glTF 呈现缺口和无效工作。完成 stochastic alpha 后才具有正确的期望透射语义；实际性能损失幅度需用 Nsight 实测。

### QRP-018：single-sided pass-through 的起点推进量随相机到命中点距离线性增长

- 严重度：中高
- 置信度：高
- 类型：光线起点鲁棒性 / 几何漏穿

#### 代码证据

- `renderer/src/device/programs.cu:463-466` 计算 `pass_eps = max(hit_t * 1e-4, 1e-6)`，随后把新起点设置为 `ray_origin + ray_dir * (hit_t + pass_eps)`。
- 同文件其他正常 surface bounce 已使用尺度鲁棒的 `offset_ray_origin()`；pass-through 没有采用等价的 ULP/坐标尺度方法。

#### 触发条件

primary 或 secondary ray 从背面命中较远的 single-sided 三角形，且它后方 `hit_t × 1e-4` 距离内还有应被命中的几何。命中距离为 10,000 world units 时，推进空隙达到 1 world unit。

#### 影响

新光线起点可能直接越过后方薄层、贴近表面或小尺度模型，造成几何消失、漏光或路径不连续。该问题与 QRP-006 的“每次 pass-through 消耗 bounce”独立存在。

### QRP-019：世界空间 tangent 保留模型缩放长度，导致 normal map 强度随实例缩放改变

- 严重度：高
- 置信度：高
- 类型：法线贴图正确性 / 错误优化判断

#### 代码证据

- `renderer/src/device/programs.cu:530-535` 用 object-to-world vector transform 得到 `T_world`，并刻意不归一化。
- `renderer/include/qualquer/renderer/math_utils.cuh:137-155` 直接计算 `B = cross(N, T_world) * tangent_w`，再组合 `T_world * tx + B * ty + N * tz`。
- normal `N` 已由 normal transform 后归一化，而 `T_world`/`B` 保留 object-to-world 的缩放长度，三条 TBN 轴不再具有同一尺度。

#### 判断

MikkTSpace 要求避免在共享索引顶点上错误平均 tangent，并不要求在最终世界空间 TBN 中保留模型缩放长度。即使实例只有均匀缩放 `s`，当前组合也变成 `s*T*tx + s*B*ty + N*tz`；归一化最终向量不能消除 tangent 分量与 normal 分量之间的相对缩放。因此 normal map 倾斜角会随实例 scale 改变。

object-to-world 变换后的 tangent 与 inverse-transpose normal 在精确数学下仍正交，但 tangent 仍需归一化；非均匀缩放和数值误差下还应确认正交化策略。

#### 触发条件

使用 normal texture 的实例具有任意非 1 的模型缩放；非均匀缩放更明显。

#### 影响

相同材质贴到不同 scale 的实例上会产生不同法线扰动强度，改变高光形状、能量、NEE 与 DLSS normal guide。放大实例会夸大 XY 扰动，缩小实例会把法线压回几何 normal。

### QRP-020：负 determinant 实例变换未翻转 triangle facing

- 严重度：高
- 置信度：高
- 类型：glTF 变换 / 正反面语义

#### 代码证据

- `renderer/src/renderer.cpp:244-261` 把 node transform 写入 `OptixInstance`，但所有实例都固定使用 `OPTIX_INSTANCE_FLAG_NONE`，没有依据变换 determinant 设置 `OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING`。
- `renderer/src/device/programs.cu:451-460` 通过 object-space edge cross 后调用 normal transform 得到 `N_face_raw`，再用其与 ray direction 的点积自行判断背面。
- inverse-transpose normal transform 不包含 `sign(det(M))`；对负 determinant，`cross(M e1, M e2) = det(M) M^{-T} cross(e1,e2)`，当前结果缺少负号。
- `renderer/src/device/programs.cu:529-534` 还直接沿用 object-space `tangent.w` 构造 world TBN；mirror transform 会翻转 tangent basis handedness，但代码没有乘以 `sign(det(M))`，因此 normal map 的 bitangent 方向也随镜像出错。

#### 规范依据

本地 glTF 2.0 规范 `/mnt/d/Github/glTF/specification/2.0/Specification.adoc:1686-1694` 明确规定：应用 node global transform 后，正 determinant 的 triangle winding 为 counterclockwise，负 determinant 时为 clockwise；这正是通过 negative scale 镜像几何的规范机制。

OptiX 9.1 `optix_types.h:976-978` 提供 `OPTIX_INSTANCE_FLAG_FLIP_TRIANGLE_FACING`，其语义会翻转 front/back classification。

#### 触发条件

任一 glTF node/global transform 含奇数次轴反射，即 3×3 线性部分 determinant 为负。

#### 影响

single-sided 材质的可见正面与规范 winding 不一致，可能显示本应剔除的一面并穿透本应可见的一面；double-sided normal 翻转和 emissive 单/双面方向也可能错误。即使 facing 修正，若未同步修正 tangent handedness，normal map 的 Y/bitangent 分量仍会镜像错误。镜像实例会与未镜像实例产生不同且不符合 glTF 的照明与阴影。

### QRP-021：逐帧 LaunchParams 使用 pageable stack memory 的 `cudaMemcpyAsync`，可能隐式同步 compute stream

- 严重度：中高（性能）
- 置信度：高
- 类型：CPU–GPU 提交并行性 / 既有优化反作用

#### 代码证据

- `renderer/src/renderer.cpp:755-815` 每帧在栈上构造 `LaunchParams`，其中包含 4096 个 Sobol direction words（约 16 KiB）。
- `renderer/src/renderer.cpp:819` 通过 `CudaBuffer::upload()` 上传该栈对象。
- `optix/include/qualquer/optix/cuda_buffer.h:102-108` 的 `upload()` 无条件调用 `cudaMemcpyAsync`，但 host source 没有通过 `cudaHostAlloc`/`cudaHostRegister` page-lock。

#### 官方依据

CUDA 官方 API synchronization behavior 说明：pageable host-to-device copy 需要先 staging 到内部 pinned memory；driver 可能先同步目标 stream，再执行 staging。只有 page-locked host memory 才能提供可靠的真正异步 H2D copy 与 copy/compute overlap。

- <https://docs.nvidia.com/cuda/cuda-driver-api/api-sync-behavior.html>
- <https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html>

#### 触发条件

每一个产生新样本的 frame 都会执行；LaunchParams 内嵌 Sobol 表使 staging 数据量远大于普通 launch constants。

#### 影响

host 侧 `submit_cuda()` 可能在参数上传处等待此前 compute-stream 工作，削弱 CPU 提交提前量和双 stream 流水线；所谓 async upload 不能与 GPU 工作可靠重叠。场景/SBT 初始化上传最终有显式同步，主要帧时风险集中在逐帧 LaunchParams。

#### 与 QRP-O05 的关系

QRP-O05 关注 16 KiB 不变 Sobol 表的 constant-cache 与重复传输收益；本项进一步确认重复传输还走 pageable staging。可将不变 Sobol 数据改为一次上传的独立只读/constant 资源，并为真正变化的小型参数使用持久 pinned staging ring；若直接把当前栈地址注册或改成临时 pinned 对象，则必须重新保证其生命周期覆盖异步 copy。

### QRP-022：并行纹理压缩可并发写入同一固定 `.tmp` 缓存路径

- 严重度：中
- 置信度：高
- 类型：缓存并发 / 场景加载

#### 代码证据

- `app/src/scene_loader.cpp:580-624` 以 `(texture_index, role)` 去重；两个不同 glTF texture 可以引用同一 image、使用不同 sampler，因此会成为两个并行 entry。
- cache key 却由 image 内容 hash 与目标 format 构成：`app/src/texture.cpp:317-323`、`app/src/texture.cpp:387-390`。相同 image 和 role 会得到同一目标路径。
- `app/src/scene_loader.cpp:620-626` 使用 OpenMP 并行压缩所有 cache miss。
- `app/src/cache.cpp:84-104` 的 `atomic_write_file()` 对同一目标统一使用固定的 `<path>.tmp`，没有每 writer 唯一名称或按 key 加锁。

#### 触发条件

同一源 image 通过多个 glTF texture object 以相同 role 使用，且缓存尚未存在。不同 sampler 是产生多个 texture object 但共享 image 的合法常见原因；内容完全相同的重复 image 也会触发。

#### 影响

多个 OpenMP worker 可同时 truncate/write/rename 同一临时文件，导致 rename 失败、缓存缺失，或依赖平台文件共享语义的竞争结果。当前帧使用各线程内存中的压缩结果，通常仍能完成加载；主要影响缓存可靠性、日志噪声和下一次启动重复压缩。

#### 补充

“write temp then rename”只保证单 writer 下的原子替换，不自动提供多 writer 安全。可在收集阶段按 `(source_hash, format)` 共享一次压缩结果，或使用每 writer 唯一 temp 名并协调最终发布。

### QRP-023：损坏或不兼容的现有纹理缓存无法被新结果修复

- 严重度：低至中
- 置信度：高
- 类型：缓存恢复路径

#### 代码证据

- `app/src/texture.cpp:317-330` 在缓存文件存在但 KTX2 解析失败、format 或 face count 不匹配时只返回 cache miss，不删除/隔离旧文件。
- 随后重新压缩并调用 `atomic_write_file()`。
- `app/src/cache.cpp:103-108` 直接把 `.tmp` rename 到目标路径；在 Windows 上目标已存在时该 rename 失败，函数删除临时文件并保留旧目标。

#### 触发条件

缓存文件截断、元数据损坏、由旧实现写出但仍使用同一 key，或格式检查失败。

#### 影响

每次加载都会重新解码和压缩，然后发布阶段再次失败；坏缓存永久存在，程序无法自愈，用户只能手工清理缓存。大型纹理会反复支付显著 CPU 压缩时间。

#### 与 QRP-012 的关系

QRP-012 是 key 缺少算法版本导致“合法旧结果被误命中”；本项是已经判定无效的文件仍不能被替换，属于独立恢复缺陷。

### QRP-024：present semaphore 的 signal stage 没有与最终 PRESENT layout transition 建立完整依赖

- 严重度：高
- 置信度：中高
- 类型：Vulkan Synchronization2 / presentation

#### 代码证据

- `renderer/src/renderer.cpp:1163-1190` 最终 barrier 把 swapchain image 从 `COLOR_ATTACHMENT_OPTIMAL` 转为 `PRESENT_SRC_KHR`，其 `srcStageMask = COLOR_ATTACHMENT_OUTPUT`，`dstStageMask = BOTTOM_OF_PIPE`。
- 在 Synchronization2 中，`BOTTOM_OF_PIPE` 用于第二 synchronization scope 时等价于 `NONE`。
- `app/src/application.cpp:416-422` 随后 signal `render_finished`，但 signal stage 仅为 `VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT`，不是 `ALL_COMMANDS`。

#### 官方依据

Khronos Synchronization2 指南给出两种完整链：

1. signal 使用 `ALL_COMMANDS`，则 semaphore 自动覆盖其前的最终 layout transition；或
2. signal 收窄到 `COLOR_ATTACHMENT_OUTPUT` 时，render-pass 外部依赖的两侧 stage 都调整为 `COLOR_ATTACHMENT_OUTPUT`，让最终 transition 与 signal stage 对接。

`vkQueueSubmit2` 的 signal first synchronization scope 虽包含同 batch 中更早的 commands，但仍受 signal `stageMask` 及其逻辑更早 stages 限制。当前代码混合了两种写法：最终 transition 的第二 scope 是 `NONE`，signal 又收窄为 graphics stage，没有按指南建立 transition → signal 的明确执行依赖。

- <https://docs.vulkan.org/guide/latest/synchronization_examples.html>
- <https://docs.vulkan.org/refpages/latest/refpages/source/VkPipelineStageFlagBits2.html>

#### 触发条件

GPU/driver 把最终 layout transition 推迟到 color output 完成之后，而 `ALL_GRAPHICS` signal 已满足；present queue 随后消费已 signal 的 semaphore。

#### 影响

presentation engine 可能在 image 完成 `PRESENT_SRC_KHR` transition 前开始使用它。表现依赖实现，可能是同步验证 hazard、间歇画面异常，或在当前驱动上因保守调度而暂时不可见。

#### 修复方向

最清晰的修复是将 `render_finished` signal stage 设为 `VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT`。若确需收窄，应严格按 Khronos 示例让最终外部依赖/explicit barrier 与 signal stage 对接，并用 synchronization validation 复核。

### QRP-025：DLSS 关闭或不可用时仍常驻分配双份完整 guides 与 DLSS output

- 严重度：高（VRAM / 可用性）
- 置信度：高
- 类型：资源生命周期 / 次优实现

#### 代码证据

- `renderer/src/renderer.cpp:282-301` 的 `FrameSlot::alloc/resize()` 无条件同时分配 color 与全部六个 `AuxBufferSet` guides。
- `renderer/include/qualquer/renderer/renderer.h:285-311` 每个 slot 的 guides 为：R32F depth、RG32F motion、三张 RGBA32F、R32F roughness，合计 64 bytes/pixel。
- 两个 slot 的 guides 因此占 128 bytes/pixel；两张 RGBA32F color 再占 32 bytes/pixel。
- `renderer/src/renderer.cpp:545-557` 又无条件按 display resolution 分配 RGBA32F `dlss_output_`，即使 `dlss_enabled == false` 或 `DlssRR::available() == false`。
- device 程序在 DLSS off 时不生成/消费 aux guides，这些分配没有渲染用途。

#### 触发条件

所有正常运行都会触发；默认 DLSS 为关闭状态，unsupported GPU/driver 上同样触发。

#### 影响

native render resolution 等于 display resolution 时，仅这些数组约占 `176 bytes/pixel`：

- 1920×1080：约 348 MiB；
- 3840×2160：约 1.36 GiB。

这还不含 AS、材质纹理、NGX feature、自身临时内存和驱动开销。在 8 GiB GPU 上会显著挤压场景预算、增加 resize 分配成本，并可能把原本可加载的场景推向 OOM；DLSS 完全不可用时浪费仍然存在。

#### 改进方向

color ping-pong 与事件可保持常驻；aux guides 和 `dlss_output_` 应只在 DLSS 实际可用且启用时按需创建，在 feature release 后按策略立即释放或进入明确缓存预算。若保留热切换缓存，应在 UI/统计中显示其 VRAM 成本并允许回收，而不是无条件常驻。

### QRP-026：environment rotation 变化不会向 DLSS-RR 发送 history reset

- 严重度：中
- 置信度：高
- 类型：DLSS temporal history / 画质

#### 代码证据

- `renderer/src/renderer.cpp:675-680` 把 `env_rotation` 变化计入 `content_changed`/`needs_reset`。
- 但 DLSS active 时 `chain_count` 本来固定为 0；真正传给 NGX 的 reset 来自 `dlss_reset_requested_`。
- `renderer/src/renderer.cpp:682-704` 只在 camera 或 `max_clamp` 变化时调用 `invalidate_dlss_history()`；environment rotation 被明确排除。
- `renderer/src/renderer.cpp:734-747` 因而给旋转后生成的 metadata 写入 `.reset = false`，`optix/src/dlss_rr.cpp:308` 最终将 `InReset=0` 传给 NGX。
- `app/src/application.cpp:600-625` 的 IBL drag 可逐帧连续改变 `env_rotation`。

#### 触发条件

DLSS-RR 开启时拖动环境旋转；暂停期间旋转后再恢复也会触发。加载全新 env map 走显式 `reset_accumulation()`，不属于本项。

#### 影响

环境照明相对于所有表面发生全局不连续变化，但 motion vectors 仍描述几何运动且通常接近零。DLSS 会把旧照明历史重投影到新照明，产生拖影、滞后、残留高光或短时错误去噪；暂停恢复时旧历史还可能跨越整个旋转跨度。

#### 修复方向

当 `dlss_active && env_rotation_changed` 时请求 history reset，并让第一张旋转后、可评估的 input metadata 携带 `reset=true`。连续拖动期间每个输入都发生光照不连续，是否逐帧 reset 或在交互期间采用更专门的响应策略可做画质 A/B，但当前完全不 reset 没有 motion-vector 补偿依据。

### QRP-027：BC block 对齐通过重采样整张纹理实现，改变非 4 倍数图像的内容与纵横比

- 严重度：高（纹理正确性）
- 置信度：高
- 类型：纹理预处理

#### 代码证据

- `app/src/texture.cpp:116-120` 定义 4×4 BC block 对齐。
- `app/src/texture.cpp:134-162` 把 level 0 的逻辑尺寸从 `(w,h)` 向上取整为 `(align4(w),align4(h))`，并用 `stbir_resize_*` 将整张源图重采样到新尺寸。
- `app/src/texture.cpp:414-437` 随后把该对齐后尺寸作为 texture 的真实 `base_width/base_height` 写入缓存和 CUDA array。
- 同文件 `extract_block()` 已能为部分 BC block 填充越界 texel，说明压缩器本身不要求通过整体缩放来补齐 block。

#### 触发条件

任一 LDR glTF image 的宽或高不是 4 的倍数。glTF 对 image 尺寸没有此限制；小贴图、条带贴图和程序生成资产很容易触发。

#### 影响

BC 存储只需要末端 block 补齐，不应改变 logical image：

- 5×7 会被重采样为 8×8，纵横比从 5:7 变成 1:1；
- 1×2 会变成 4×4，发生极严重拉伸；
- base color、metallic/roughness、emissive 与 normal map 都会在归一化 UV 下采到被扭曲的内容；
- mip chain 也从错误的对齐后尺寸继续生成，缓存会永久保存该失真结果。

#### 修复方向

保持 KTX2/CUDA resource 的 logical base dimensions 为原始 `(w,h)`，按原始逻辑尺寸生成 mip；压缩每级时只对最后一个 4×4 block 做 edge padding。对颜色/标量纹理宜复制边缘 texel而不是补透明黑，避免 clamp 与 mip 边界产生暗边；具体 CUDA native BC extent/copy 单位应再按 CUDA 13.2 API 契约验证。

### QRP-028：部分 BC block 用透明黑补齐，生成的尾部 mip 内容不可靠

- 严重度：低（当前路径）/ 中（启用 LOD 后）
- 置信度：高
- 类型：纹理压缩边界处理 / 潜伏缺陷

#### 代码证据

- `app/src/texture.cpp:179-194` 的 `extract_block()` 对超出 logical mip 范围的 texel 写入全零 RGBA。
- `compress_bc7()` 与 `compress_bc5()` 都把这 16 个 texel 作为真实 block 输入交给有损编码器。
- 即使 base image 已是 4 的倍数，完整 mip chain 最终也通常包含 2×N、1×N、2×2、1×1 等尺寸，因此本项并不限于 QRP-027 的非对齐源图。

#### 触发条件

启用 mip 的任意 LDR texture 在某级宽或高小于 4 或不是 4 的倍数；当前所有 LDR texture 都生成完整 mip chain。

#### 影响

硬件不会把 logical extent 外的 texel 作为独立图像区域采样，但 block encoder 会用有限 endpoints/partitions 同时拟合真实 texel与伪造黑 texel。人工黑色因此消耗编码精度并可改变有效边缘 texel。当前 QRP-029 表明这些尾部 mip 尚未被材质采样，所以现阶段主要表现为错误 cache 数据与无效压缩开销；一旦实现显式 LOD，就会直接变成远距离变暗、颜色/alpha coverage 改变及 normal/roughness 失真的画质问题。

#### 修复方向

block padding 应复制最近边缘 texel，或按 sampler wrap 语义构造边界；至少不能统一补透明黑。alpha-tested texture 还需单独考虑 mip alpha coverage，而不是把 padding 与 coverage 问题混为一体。

### QRP-029：材质纹理生成并上传完整 mip chain，但所有 OptiX 采样固定走隐式 base level

- 严重度：高（画质与资源）
- 置信度：高
- 类型：texture LOD / path tracing

#### 代码证据

- `app/src/texture.cpp:134-176` 为每张 LDR texture 生成完整 mip chain，随后逐级 BC 压缩、缓存和上传。
- `optix/src/cuda_texture_upload.cpp:101-173` 创建 mipmapped CUDA array 并设置 mipmap filter/clamp。
- 但所有材质、alpha any-hit 和 emissive NEE 读取均调用不带 LOD/gradient 的 `tex2D<float4>()`：`renderer/src/device/programs.cu:546-573,771`、`renderer/include/qualquer/renderer/nee.cuh:449,625`。
- 工作区没有任何材质路径调用 `tex2DLod()` 或 `tex2DGrad()`，也没有传播 ray cone、ray differential 或显式 footprint。

#### 官方依据

CUDA 为 mipmapped texture 提供 `tex2DLod()`（显式 level）和 `tex2DGrad()`（由 gradients 推导 level）；普通 `tex2D()` 没有可用于选择 path-traced ray footprint 的梯度输入。任意 ray program 也不存在 raster quad 的隐式屏幕导数。

- <https://docs.nvidia.com/cuda/cuda-c-programming-guide/#texture-functions>
- NVIDIA 的 DLSS integration checklist 还明确要求在 RR/SR 启用时设置合适 mip-map bias，否则纹理会显得模糊、涂抹或低分辨率；当前连基础 footprint LOD 都不存在，更无法针对低 render resolution 正确偏置：<https://developer.nvidia.com/blog/how-to-integrate-nvidia-dlss-4-into-your-game-with-nvidia-streamline>

#### 触发条件

所有纹理化材质，尤其高频纹理、斜视角、远距离物体、反射/间接 bounce 和低 render resolution。

#### 影响

- 远距离和缩小时持续采样最高分辨率，产生 aliasing、闪烁和高频噪声；DLSS-RR 会收到本可由正确 footprint 预滤除的噪声。
- glTF `minFilter` 的四种 mipmap 模式及 `mipmapFilterMode` 实际没有发挥预期作用。
- CPU 仍支付 mip 生成与每级 BC 压缩，磁盘 cache 和 GPU VRAM 仍存放所有不可达 mip，形成纯开销。

#### 改进方向

引入 ray cones 或 ray differentials，在每次 hit 根据 UV gradients/texture dimensions 计算显式 LOD，再使用 `tex2DLod`/`tex2DGrad`；primary 与 secondary/specular ray 的 footprint 更新规则需统一设计。若暂不实现 LOD，应至少明确只存 base level，避免把不可达 mip 伪装成已支持的 glTF filtering。

### QRP-030：DLSS motion vector 接受 previous clip space 中位于相机后方的投影

- 严重度：中
- 置信度：高
- 类型：DLSS disocclusion / motion vectors

#### 代码证据

- `renderer/src/device/programs.cu:346-370` 将当前 primary hit/world direction 同时投影到 current 与 previous unjittered VP。
- perspective divide guard 使用 `fabsf(prev_clip.w) >= 1e-4`；它只排除接近零的 `w`，却把明显为负的 `w` 当成有效。
- 对当前 GLM 右手透视矩阵，camera 前方点的 `clip.w = -view_z > 0`；负 `w` 表示该点/方向位于 previous camera 后方，没有合法 previous screen position。

#### 触发条件

快速相机旋转、较大帧间位移、穿过近处几何，或 sky direction 在 previous view 中落到相机后半球。正常小幅运动不一定触发。

#### 影响

相机后方点仍会被透视除法映射到有限 NDC；正后方方向甚至可映射到画面中心。DLSS 因而沿错误 motion vector 读取本不对应的历史，产生拖影或错误稳定，而不是把该像素视为 disocclusion。

#### 修复方向

至少要求 `curr_clip.w > epsilon && prev_clip.w > epsilon` 后才生成重投影 MV；previous point 不可投影时需采用与 DLSS 契约一致的 disocclusion 表达（可选 disocclusion mask、history reset 或经画质验证的 fallback），不能把“负且远离零”误认为有效。

### QRP-031：firefly clamp 对 Inf 产生 `Inf × 0 = NaN`，对既有 NaN 完全失效

- 严重度：中（数值鲁棒性）
- 置信度：高（行为）/ 中（实际触发频率）
- 类型：path sample 非有限值处理

#### 代码证据

- `renderer/include/qualquer/renderer/pt_common.cuh:199-209` 先计算平均 RGB luminance；超过阈值时返回 `radiance * (max_clamp / lum)`。
- 任一正无穷分量可使 `lum=Inf`、比例变成 0，随后该分量执行 `Inf*0` 得到 NaN。
- 输入已经含 NaN 时，`lum > max_clamp` 为 false，函数原样返回 NaN。
- `renderer/src/device/programs.cu:274-300` 在累加前使用该函数；NaN 随后进入 frame sum，DLSS-off 时还会写入持久 Separate Sum。

#### 触发条件

极端近零 PDF、很长且高增益路径、数值溢出、异常材质/纹理值或其他 BRDF/NEE 非有限中间量。正常资产上未必常见，需要 device validation/画面统计确认频率。

#### 影响

单个 non-finite sample 可永久污染该像素的 accumulation；tonemap 的比较/clamp 同样不能可靠修复 NaN。DLSS 开启时会把非有限 color 输入交给 NGX，结果未定义于当前集成层。

#### 修复方向

在 firefly clamp/累加边界显式检测 RGB 是否 finite，并定义稳定策略：丢弃异常 sample、饱和到有限阈值或记录 device diagnostic。不能依赖普通 luminance 比较消除 NaN/Inf；同时应定位产生 non-finite 的上游而不是只掩盖。

### QRP-032：secondary BRDF samples 未约束到 geometric-normal hemisphere

- 严重度：高
- 置信度：高
- 类型：shading normal / path correctness

#### 代码证据

- `renderer/src/device/programs.cu:543-544` 只保证 mapped shading normal 与 `N_face` 位于同一半球；这不代表以 shading normal 为轴采样出的所有方向也位于 geometric hemisphere。
- `renderer/include/qualquer/renderer/brdf.cuh:917-978` 的 specular branch 只检查 tangent-space `L_ts.z`（相对 `N_brdf`），`BrdfParams` 本身不携带 geometric normal；diffuse branch 甚至不拒绝负 `L_ts.z`，而是把 `NdotL` 强行 clamp 到 `1e-4` 后继续追踪。
- EON 原论文的 CLTC 目标是禁止 negative-hemisphere samples，但当前 rational-fit 边界可静态构造反例：`r=1, mu=1` 时 `d≈+0.00345`；conditional random 接近 1、azimuth 接近 0 时 `wh→(-1,0,0)`，故变换后 `wi.z=d*wh.x+wh.z<0`。24-bit RNG 可进入该非零邻域。此时 `sample_EON()` 还把仅定义于正半球的 uniform PDF 常数加入 mixture PDF。
- `renderer/src/device/programs.cu:707-744` 接收 `bs.next_dir` 后不再检查 `dot(N_face, next_dir)`，直接作为下一条 path ray。
- 相比之下，NEE 路径在 `nee.cuh` 中显式拒绝 `dot(N_face,L) <= 0`，两条积分策略的有效方向域不一致。

#### 触发条件

平滑 shading normal 与低多边形 face normal 偏差较大、强 normal map、silhouette/grazing angle，或代码的 grazing fallback 进一步调整 `N_brdf` 时。

#### 影响

采样方向可能位于 shading normal 上方却穿入真实几何表面下方；CLTC 数值边界下甚至可位于 shading normal 自身的负半球。ray origin 仍沿 `N_face` 向外偏移，随后 ray 朝内发射，可能立即自交、穿透壳体、采到物体内部/背后环境。diffuse branch 对负方向使用正 cosine 与不适用的 mixture PDF，还会直接形成 estimator 偏差。

#### 修复方向

先保证 CLTC fit/sampling 在其声明域内（对 `d` 的边界、sample hemisphere 与 PDF 做一致处理），再在生成 secondary direction 后拒绝 geometric-backside samples，并让无效概率体现在 estimator/PDF 中；更完整方案应采用一致的 shading-normal energy/Jacobian correction。仅把方向硬翻回半球会改变分布，同样可能引入偏差，需按所选理论方案实现并做 furnace/normal-map 测试。EON 依据：<https://jcgt.org/published/0014/01/06/>。

### QRP-033：shadow-terminator correction 只作用于 NEE，MIS 的 BRDF-hit 分支估计了不同 integrand

- 严重度：中高
- 置信度：高
- 类型：MIS / shading-normal heuristic

#### 代码证据

- `renderer/include/qualquer/renderer/nee.cuh:347-350,468-471,667-670,696-699` 仅在 light-sampled NEE contribution 上乘 `shadow_terminator_factor(N_face,N_shading,L)`。
- BRDF sampling 的 throughput 不包含该 factor，也不携带前一命中的 geometric/shading normals。
- secondary ray 随后命中 emissive triangle 或 environment 时，`renderer/src/device/programs.cu:572-594` 与 miss shader 只应用 MIS weight，无法为前一 shading point 补上相同 correction。

#### 官方依据与判断

Disney 原论文把该项定义为额外的 shadowing factor `G`，用于让以 shading normal 计算的 incident illumination 在 geometric terminator 平滑归零；项目实现的除以 `dot(Ng,Ns)` 公式及 Hermite 多项式本身与论文一致。

- <https://media.disneyanimation.com/technology/publications/2019/TamingtheShadowTerminator.pdf>

标准 MIS power heuristic 要求参与组合的策略估计同一 integrand。当前 light-sampled strategy 估计 `f × G`，BSDF-hit strategy 估计未修正的 `f`；权重相加不再恢复任一目标函数。并且所有只经 BRDF sampling 到达的间接入射光也完全绕过 `G`。结果会随 proposal PDF/路径生成方式改变，而不是只由场景传输决定。

#### 触发条件

smooth/normal-mapped surface 的 geometric 与 shading terminator 有明显差异，且同一 env/emissive contribution 同时可由 NEE 与 BRDF path 采到。

#### 影响

shadow terminator 附近产生有方向/PDF依赖的能量偏差；改变 roughness 或 light sampling 分布甚至可能改变该偏差，而不只是噪声。BRDF-hit 与纯间接分支还会重新引入本想消除的亮边/漏光。

#### 修复方向

选择一个对两种策略一致的 shading-normal/terminator 模型，并在 BRDF eval、sample throughput 与双方 PDF/贡献中统一。若保留经验性 direct-light attenuation，也需明确接受偏差并避免再用标准 MIS 将它与未修正 integrand 混合。

### QRP-034：没有 TLAS 时 raygen 固定输出黑色，已加载 environment 也不可见

- 严重度：中
- 置信度：高
- 类型：empty scene / environment rendering

#### 代码证据

- `renderer/src/device/programs.cu:237-248` 在 `params.traversable == 0` 时不生成 primary direction、不执行 environment lookup，直接向 color 写黑并返回。
- `renderer/src/renderer.cpp:716-719,954-965` 同时把这类 frame 标成非 DLSS input、sample count 0。
- 但 environment 是独立的 scene light resource；miss shader 已具备 cubemap lookup，并不要求存在任何几何命中。

#### 触发条件

合法空 glTF、所有 mesh 都未被 default scene 实例化、所有可建实例为空，或用户只加载 HDR environment 而没有 TLAS。

#### 影响

本应可见的 sky/environment 被替换成纯黑；无法用空场景查看 HDR、验证 env rotation 或获得环境背景。该行为不是 `optixTrace(null)` 的必要结果，而是当前 null-traversable fast path 丢掉了 miss 等价逻辑。

#### 修复方向

保留对 null traversable 的 trace 防护，但在 raygen 直接执行 primary environment lookup（并写 sky guides/motion）或提供合法的无几何 miss 路径；仅当 environment 也不存在时才输出黑色。

### QRP-035：node transform 用绝对 determinant 阈值拒绝合法的小尺度实例

- 严重度：中
- 置信度：高
- 类型：glTF transforms / scene completeness

#### 代码证据

- `app/src/scene_loader.cpp:760-774` 在 `abs(determinant(world_mat)) < 1e-12` 时跳过整个 node。
- 对 affine transform，该 determinant 包含三个 scale 分量的乘积；均匀 scale `s` 的 determinant 为 `s^3`。因此大约 `s < 1e-4` 的有限、非零且数学上可逆的变换会被固定阈值判成 singular。

#### 触发条件

资产通过很小 node scale 配合较大 object-space 坐标换算单位，或祖先层级 scale 乘积很小但最终 world-space geometry 仍具有正常尺寸。glTF 并未禁止这种非零 scale。

#### 影响

整个 node 及其 mesh primitives 被静默跳过，仅产生 warning；场景缺件，相关 BLAS/TLAS、emissive lights 和 bounds 也全部消失。反之，determinant 尚可但 condition number 极差的强各向异性矩阵仍可能通过，因此该阈值也不能可靠代表 normal transform 的数值稳定性。

#### 修复方向

对 affine 3×3 linear part 做尺度相关的可逆性/condition 检查，或仅拒绝真正零/非有限变换并在 normal transform 处采用稳定策略。阈值应相对于矩阵尺度，而不是固定比较 dimensionful determinant。

### QRP-036：设备筛选未验证实际 blit/swapchain usage capability，创建路径也无 fallback

- 严重度：低（目标 NVIDIA Windows 驱动）/ 中（能力协商鲁棒性）
- 置信度：高
- 类型：Vulkan format/surface capability

#### 代码证据

- `vulkan/src/context.cpp:107-124` 的 `is_presentable_device()` 只检查 graphics+present queue、swapchain extension 与 Vulkan 1.4；它也未检查随后无条件启用的 `dynamicRendering`/`synchronization2` feature bits、Win32 external-memory/external-semaphore extensions 与 handle compatibility。
- `vulkan/src/swapchain.cpp:113-133` 无条件请求 `VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT`，未检查 `VkSurfaceCapabilitiesKHR::supportedUsageFlags`。
- `vulkan/src/interop.cpp:21-48` 固定创建 optimal-tiling `R16G16B16A16_SFLOAT` transfer-source image；`renderer/src/renderer.cpp:1050-1081` 固定用 linear-filter blit 到 `B8G8R8A8_SRGB`。
- 初始化前没有查询 source 的 `BLIT_SRC`/linear-filter format features、destination 的 `BLIT_DST` feature、`supportedCompositeAlpha`，也没有用 `VkPhysicalDeviceExternalImageFormatInfo`/external semaphore properties 验证 OPAQUE_WIN32 interop；capability failure 没有替代 copy/shader path。

#### 规范边界与触发条件

`vkCmdBlitImage2` 的 source/destination format 与 linear filter 各自受 `VkFormatProperties` feature bits 约束；swapchain `imageUsage` 必须是 surface `supportedUsageFlags` 的子集。当前 NVIDIA Windows Ada/更新驱动预计支持这组常见能力，但项目自己的设备资格判定并未证明该前提。

- <https://docs.vulkan.org/spec/latest/chapters/formats.html>
- <https://docs.vulkan.org/refpages/latest/refpages/source/VkSurfaceCapabilitiesKHR.html>
- <https://docs.vulkan.org/refpages/latest/refpages/source/vkCmdBlitImage2.html>

#### 影响与修复方向

在不满足任一能力的设备/WSI 组合上，设备会先通过“presentable”筛选，随后 logical-device/swapchain/external-image create 或 CUDA import 失败，或 blit 触发 validation/未定义结果。初始化时应一次性查询并验证完整呈现路径；不支持时明确拒绝该 GPU，或选择受支持的 intermediate/swapchain format 和 copy/fullscreen conversion fallback。

### QRP-037：DLSS availability probe 可降级，但 feature create/evaluate/release 失败会直接终止进程

- 严重度：中高（可用性）
- 置信度：高
- 类型：NGX runtime failure path

#### 代码证据

- `optix/src/dlss_rr.cpp:58-81` 同时定义 non-fatal `NGX_CHECK_WARN` 与调用 `abort()` 的 `NGX_CHECK`。
- 初始化/availability probe 会在不支持时返回 `available_=false`，允许 fallback path；但 `create_feature()`、每帧 `evaluate()` 及 `release_feature()` 分别在 `dlss_rr.cpp:245,332,346` 使用 fatal `NGX_CHECK`。
- `Renderer::submit_cuda()` 没有接收 feature create/evaluate 的失败状态并切回原生 tonemap 的机制。

#### 触发条件

NGX feature OOM、driver/runtime error、输入资源或尺寸不被接受、feature release 失败，或运行期间设备状态变化。QRP-025 的常驻 VRAM 开销会提高 create-time OOM 的实际概率。

#### 影响

本应属于可选增强功能的 DLSS-RR 会使整个 renderer 立即 `abort()`；用户无法继续以 DLSS-off path 渲染，且正常错误 UI/config fallback 不会执行。

#### 修复方向

让 feature create/evaluate 返回可传播状态；安全同步后释放/失效 NGX state、禁用本次 DLSS 路径并回退到 noisy accumulation/tonemap，同时向 UI 报告错误。只有无法保持资源一致性的 CUDA/OptiX core failure 才保留 fatal policy。

### QRP-038：DLSS preset 配置漏掉 UltraQuality mode

- 严重度：中
- 置信度：高
- 类型：NGX feature configuration

#### 代码证据

- `optix/src/dlss_rr.cpp:220-230` 声称“Render preset for all quality modes”，实际只设置 DLAA、Quality、Balanced、Performance、UltraPerformance 五个 parameter key。
- SDK 本地头 `third_party/dlss/include/nvsdk_ngx_defs_dlssd.h:159-164` 还定义了独立的 `NVSDK_NGX_Parameter_RayReconstruction_Hint_Render_Preset_UltraQuality`。
- 项目枚举、optimal-settings cache 和自动 mode resolver 都包含 `DlssQualityMode::UltraQuality`，因此该模式可实际被选中。

#### 触发条件

用户选择 preset D/E，且请求 render height 最终解析到 UltraQuality mode。

#### 影响

UI 显示已选择的 render preset，但 UltraQuality feature create 不读取对应 hint，可能继续使用 Default/driver 当前模型；同一 preset 在不同 quality mode 下行为不一致，画质 A/B 与问题诊断会被误导。

#### 修复方向

与其他 mode 一样，在 feature create 前设置 UltraQuality preset key；同时避免手写不完整列表，可把 mode→parameter key 显式集中并用 SDK 枚举/静态检查覆盖全部支持模式。

### QRP-039：emissive NEE 把 alpha-masked 透明区域也当作发光面

- 严重度：高
- 置信度：高
- 类型：emissive geometry / alpha visibility / MIS

#### 代码证据

- `app/src/emissive_alias_table.cpp:39-114` 只按 `emissiveFactor × world area` 收集整张三角形；`EmissiveTriangle` 仅保存 emissive texture/UV 与 double-sided 状态，没有 base-color alpha texture、factor、vertex alpha、alpha cutoff/mode。
- 普通 radiance/shadow intersection 的 any-hit 会在 `renderer/src/device/programs.cu:746-781` 按 base-color alpha 对 Mask texel 调用 `optixIgnoreIntersection()`。
- emissive NEE 在 `renderer/include/qualquer/renderer/nee.cuh:420-459,615-638` 均匀采样三角形后只读取 emissive texture RGB，从不执行同一 Mask test；shadow `tmax` 又刻意停在目标 triangle 前，目标 any-hit 不会替它过滤透明点。

#### 触发条件

`alphaMode=MASK` 的 emissive primitive，且 base-color alpha、baseColorFactor.a 或 vertex color alpha 在三角形部分区域低于 cutoff。

#### 影响

几何上已被 any-hit 挖空的区域仍通过 NEE 发光，产生悬空/过强直接照明；BRDF rays 无法命中同一透明区域，light-sampled 与 BRDF-hit strategies 描述不同的发光域，进一步破坏 MIS 对应关系。大面积 cutout 会同时增加无效 alias 权重和噪声。

#### 修复方向

让 light sample 在目标 UV/vertex alpha 上执行与 any-hit 完全相同的 Mask 判定，透明 sample 返回零；proposal 仍可覆盖整 triangle，保持无偏但可能高方差。若要从 alias 权重中剔除透明 coverage，应预积分纹理 alpha 并同步更新可评估的 area PDF，不能只改权重而不改 PDF。Step 16 落地 stochastic Blend 后，同一发光域一致性要求也必须扩展到 Blend，而不能只修 Mask。

### QRP-040：`vkAcquireNextImageKHR` 的 `SUBOPTIMAL` 状态被丢弃，并假定 present 会再次报告

- 严重度：低至中
- 置信度：高
- 类型：swapchain lifecycle

#### 代码证据

- `app/src/application.cpp:296-346` 只在 acquire 返回 `VK_ERROR_OUT_OF_DATE_KHR` 时 drain/recreate；`VK_SUBOPTIMAL_KHR` 被当作成功继续，注释声称交给 present 处理。
- `Application` 没有保存本帧 acquire 的 suboptimal 状态。
- `app/src/application.cpp:453-470` 只有 present 自身返回 `SUBOPTIMAL/OUT_OF_DATE` 或 framebuffer extent 变化时才 recreate。

#### 判断与触发条件

`VK_SUBOPTIMAL_KHR` 表示当前 swapchain 仍可使用但已不再精确匹配 surface；acquire 与 present 是两次独立 API 返回，规范不保证后者必然重复前者状态。surface capability/transform 等变化但 framebuffer extent 未变化时，present 可以成功，acquire 的信号遂永久丢失；若驱动持续只在 acquire 报告，代码会每帧忽略。

#### 影响与修复方向

程序可能长期保留仍可呈现但非最佳/不完全匹配的 swapchain，表现为未应用新的 surface 配置、额外 compositor 路径或平台相关显示异常。记录 `acquire_suboptimal`，完成该帧 submit/present 后无条件 recreate；仍需保留现有 semaphore 消费链，不能在已提交 CUDA work 后直接跳帧。

### QRP-041：core glTF skin 与非零 morph weights 被静默当作普通静态 base mesh

- 严重度：中至高
- 置信度：高
- 类型：glTF geometry deformation / scene correctness

#### 代码证据

- `app/src/scene_loader.cpp:267-530` 只读取 base `POSITION/NORMAL/TEXCOORD_0/COLOR_0/TANGENT`，不读取 primitive morph targets、`JOINTS_n` 或 `WEIGHTS_n`。
- `app/src/scene_loader.cpp:698-789` 实例化 node 时始终使用 node global matrix，没有检查 `node.skinIndex`、`node.weights` 或 `mesh.weights`。
- loader 也没有拒绝/警告这些已解析但不支持的 core fields，因而场景表面上会“成功加载”。

#### 规范依据

- glTF 2.0 §Morph Targets 要求 instantiated mesh 使用 `node.weights`，缺失时使用 `mesh.weights`。
- §Skins 要求按 joints/weights 与 inverse bind matrices 计算 per-vertex transform，并明确规定 skinned mesh node 自身的 transform 必须忽略。
- 本地官方规范位置：`/mnt/d/Github/glTF/specification/2.0/Specification.adoc:1448-1460,1552,1595-1600,1696`。

#### 触发条件与影响

带 skin 的角色/物体，或含非零 default/node morph weights 的静态资产。当前会渲染 bind/base geometry，skinned node 还会套用规范要求忽略的 node transform；BLAS/TLAS bounds、法线/tangent 和 emissive triangles 全部基于错误几何，可能出现严重错位、错误姿态、裁剪/照明不一致。动画播放本身可不在当前静态 renderer 范围，但初始有效几何不能静默错误。

#### 修复方向

短期在 load 时检测并明确拒绝或 UI 警告 unsupported skin/morph，而不是声称加载成功。若支持静态初始 pose，应在 BLAS 与 emissive table 构建前 CPU/GPU bake morph（先于其他 transforms）及 skinning，并重算 normals/tangents/bounds；动态 animation 则另需 AS update/rebuild 生命周期设计。

### QRP-042：将 perceptual roughness 强制到 0.04，改变合法的光滑/镜面 glTF 材质

- 严重度：中
- 置信度：高
- 类型：material semantics / BRDF domain clamp

#### 代码与规范事实

- `renderer/src/device/programs.cu:551-554` 对 `texture.g × roughnessFactor` 使用 `fmaxf(..., 0.04f)`，随后才平方得到 GGX alpha；所有低于 0.04 的值因此被压成同一 lobe。
- archived design 把 0.04 称为“glTF 下限/业界通用值”，但 glTF schema 的实际范围是 `[0,1]`，官方规范还直接给出 `roughnessFactor: 0.0` 的 gold material。
- 依据：`/mnt/d/Github/glTF/specification/2.0/schema/material.pbrMetallicRoughness.schema.json:35-40` 与 `Specification.adoc:1981-1986`。

#### 触发条件与影响

`roughnessFactor` 或 metallic-roughness texel G 小于 0.04 的抛光金属、镜面与锐利高光。当前实现系统性加宽高光/反射，无法表达规范允许的零 roughness；EON、GGX、energy compensation 及 DLSS roughness guide 都读取 clamp 后值，所以这是整体材质外观变化而非单一 guide mismatch。

#### 判断边界与修复方向

GGX alpha 精确为零会造成 delta singularity，保留数值下限有工程依据；问题是把较大的经验阈值包装成 glTF 语义并无条件改变资产。选择显著更小、只防除零的 alpha floor，或为 roughness≈0 实现 delta-specular 分支；把阈值与理由写成 renderer approximation，并用 polished-metal/highlight reference A/B 确认可接受误差。

## 4. 已识别但仍需完成边界核实的问题

当前没有未决候选。原 QRP-P03 已完成能力链核实并提升为 QRP-036；RGBA16F → sRGB 的格式转换规则本身成立，缺陷在于项目未把所需 format/surface feature 纳入设备资格检查。

## 5. 既有优化可能产生反作用的检查项

以下不是静态代码即可定性的错误。必须通过 release 编译资源报告、Nsight Systems/Compute、GPU 时间线和图像 A/B 验证。

### QRP-O01：单策略 NEE 可能以寄存器和重复工作换取一条 shadow ray

#### 代码事实

当前实现先生成环境和发光体两个完整候选，再选择其中一个发射 shadow ray。选择前需要同时保留两套方向、PDF、radiance、距离、法线及有效性状态，并执行两次 alias/sample 与纹理读取。

#### 可能反作用

- 增大 raygen/closest-hit 的寄存器活跃集；
- 增大 continuation stack 或 local-memory spill；
- 降低 occupancy；
- shadow traversal 很便宜、候选构造昂贵或 env/emissive 势均力敌时，总时间可能高于分别采样。

#### 验证要求

比较优化前后：寄存器数、stack size、local load/store、occupancy、shadow traversal 时间、总 frame time，并在纯 env、纯 emissive、混合场景分别测量。

### QRP-O02：双 CUDA stream 重叠可能形成资源争用而非净并行

#### 代码事实

`renderer/src/renderer.cpp` 让 `compute_stream` 执行 OptiX raygen，同时让 `display_stream` 执行 DLSS/tonemap 与 semaphore signal；FrameSlot 和事件链避免了数据竞争。

#### 可能反作用

- OptiX megakernel 与 DLSS/tonemap 争用 SM、L2、显存带宽和调度资源；
- 单项 kernel 时间可能同时增长；
- “时间线上有重叠”不等于端到端吞吐提高，尤其在 Ada 满占用 path tracing 下。

#### 验证要求

对照强制串行与当前并行版本的总帧时间、各 kernel duration、SM/L2/DRAM 指标和 overlap 区间。

### QRP-O03：SER 使用固定 10-bit material hint 可能挤占更有价值的排序信息

#### 代码事实

当前 SER reorder 对 material coherence 使用 10 个 hint bits。

#### 官方约束

OptiX Programming Guide 指出 coherence hint bits 会占用 hit-object 排序 key 的位，应尽量减少；排序优先级还包括 shader ID 与空间信息。

#### 可能反作用

- 实际 material 数远少于 1024 时，10 bits 过宽；
- 牺牲空间局部性排序信息；
- 小波前或本来已相干的命中上，reorder 固定开销可能超过收益。

#### 官方依据

- OptiX Programming Guide，Shader Execution Reordering：<https://raytracing-docs.nvidia.com/optix9/guide/index.html>

### QRP-O04：为缩短 live range 重算 CLTC invariants 未必优于保存

#### 代码事实

当前策略在 shadow trace 后重算一组 CLTC/BRDF invariant，以避免跨 trace 保留状态。

#### 可能反作用

重算包含多次 FMA、除法和幂运算。若编译器本可有效保存，或 continuation stack/寄存器并未成为瓶颈，重算会增加 ALU 与 special-function 压力。

#### 验证要求

比较重算版与保存版的寄存器、continuation stack、spill、issued instructions、occupancy 和总 GPU 时间。

### QRP-O05：16 KiB Sobol direction table 随 LaunchParams 每帧上传的收益需量化

#### 代码事实

Sobol direction data 内嵌在 LaunchParams/constant 参数路径，每帧随参数上传；高维采样随后退化到 hash。

#### 潜在收益

warp 内相同维度读取具有 constant-memory broadcast 优势。

#### 可能反作用

- 每帧重复上传不变表；
- 占据 constant cache 工作集；
- 分支和表访问成本可能在高 bounce/低相干路径中抵消低差异收益。

#### 验证要求

对比 constant 参数、独立只读全局表和编译期常量方案的上传成本、constant-cache 命中率、kernel 时间与收敛效率。

### QRP-O06：emissive alias 权重忽略发光纹理，纹理化光源可能产生高方差

#### 代码事实

- `app/src/emissive_alias_table.cpp:47-55` 只从 `Material::emissive_factor` 计算 luminance。
- `app/src/emissive_alias_table.cpp:111` 用 `emissive_factor luminance × triangle area` 构建 triangle alias 权重。
- `renderer/include/qualquer/renderer/nee.cuh` 只有在三角形已被选中并生成 UV 后才读取 emissive texture；纹理空间亮度不参与三角形选择。

#### 判断

该估计器只要所有实际发光三角形仍具有非零选择概率，就不因此产生偏差；但对亮度集中在少量 texel、不同三角形覆盖纹理亮度差异很大或大面积多数为黑的 emissive texture，采样概率与实际贡献严重失配。

#### 优化方向与代价

可在场景预处理时估算每个三角形覆盖区域的平均 emissive luminance，并把它纳入 alias power。需要权衡纹理解码数据生命周期、UV wrap/filter、mip 代表性和构建成本；不能简单使用整张纹理的全局平均替代每三角形覆盖平均。

#### 验证要求

在高对比 emissive atlas、文字/灯带纹理和大面积稀疏发光纹理场景中比较 variance、有效 light sample 比例、shadow-ray 浪费率及预处理成本。

### QRP-O07：BLAS 逐个 build/compact 并两次同步，场景上传被完全串行化

#### 代码事实

- `renderer/src/renderer.cpp:175-183` 对每个 glTF mesh group 依次调用一次 `AccelStructure::build_blas()`。
- `optix/src/accel_structure.cpp:80-111` 每个 BLAS build 后立刻 `cudaStreamSynchronize()`，回读 compacted size。
- `optix/src/accel_structure.cpp:120-133` 若执行 compact，又对同一 BLAS 再同步一次，之后才开始下一个 BLAS。

#### 可能代价

大量 mesh 的场景会产生每 BLAS 两个 host/device round trip，GPU build、CPU 调度和内存分配无法批处理；小 BLAS 场景尤其容易被同步延迟主导。这不影响逐帧 tracing 性能，但会显著增加首次加载和切场景时间。

#### 优化方向与取舍

可批量提交多个 build、统一回读 compacted sizes，再批量 compact，或至少把每阶段同步合并。代价是同时保留更多 uncompacted buffer 与 scratch memory，峰值显存更高；应以加载时间与峰值显存 A/B 决定批量大小，而非无条件全量并行。

### QRP-O08：GPU 选择把 compute capability 置于实际吞吐能力之上，可能选择更慢的新架构 GPU

#### 代码事实

- `optix/src/context.cpp:119-157` 先按 compute capability `(major, minor)` 选择最大值，只有 capability 完全相同时才调用 `rate_device()`。
- `rate_device()` 仅用 discrete 标志和 `totalGlobalMem` 做同架构 tiebreak，没有比较 SM 数、显存带宽、时钟或实际 ray-tracing 吞吐。

#### 可能反作用

在多张可呈现 NVIDIA GPU 共存时，低端但 compute capability 更新的 GPU 会无条件压过高端上一代 GPU。例如低端 Blackwell 与高端 Ada 同机时，当前策略可能选择前者，即使其 OptiX、DLSS 和显存吞吐明显更低。compute capability 表示功能/ISA 世代，不是跨产品级别的性能分数。

#### 验证与改进方向

设备选择应首先尊重用户显式选择；自动模式可综合 multiprocessor count、核心时钟、memory bus/bandwidth、VRAM 与架构能力，或进行极短校准。不能仅以理论峰值字段替代实测，但至少不应把 capability 作为绝对性能排序。

### QRP-O09：DLSS-RR 未提供 specular hit distance，也未实现 Primary Surface Replacement

#### 代码事实

- `optix/src/dlss_rr.cpp:265-267` 明确将 `pInSpecularHitDistance = nullptr`。
- 当前 guides 总是描述 camera ray 的第一处 shaded surface；除 single-sided pass-through 外，不会沿近镜面路径寻找反射中的 primary replacement surface。
- 本地 NVIDIA `vk_denoise_dlssrr` 参考实现沿最多 5 层镜面路径寻找 Primary Surface Replacement，并让 depth、normal、motion 与 albedo 描述该 virtual surface；同时写入 reflection path length。
- 本地 NVIDIA `vk_gltf_renderer` 也捕获第一次 specular bounce 的 hit distance，环境 miss 写 fp16 最大距离，并把该可选 guide 传给 NGX。

#### 优化机会

在高 specular、低 roughness 材质上，加入 specular hit distance 可帮助 RR 区分反射几何与 primary geometry；进一步实现受控深度的 Primary Surface Replacement，可让 motion/depth/normal guides 与镜中实际颜色来源一致，减少镜面拖影与错误重投影。

#### 取舍与验证

该输入是可选项，缺失不构成 API 错误。实现会增加 payload/live state、额外路径分类和至少一张 guide；QRP-025 已表明当前 guide VRAM 很高，因此应先采用 R16F 等 SDK/参考实现支持的紧凑格式，并对镜面场景 A/B 比较 ghosting、disocclusion 与总帧时，不能无条件照搬 5 层 PSR。

### QRP-O10：DLSS guides 全部使用 32-bit channels，存在显著带宽与 VRAM 压缩空间

#### 代码事实

当前每 slot guides 为 64 bytes/pixel：R32F depth、RG32F motion、三张 RGBA32F、R32F roughness；双 slot 为 128 bytes/pixel。`vk_gltf_renderer` 官方参考则使用 RGBA8 diffuse、RGBA16F specular、RGBA16F packed normal/roughness、RG16F motion、R16F depth/specular distance。

#### 优化机会

若 CUDA RR 路径对对应格式的 texture/surface object 支持经当前 SDK 验证，可将 normal 与 roughness 打包，并把适合的 guides 降到 fp16/UNORM；这会同时降低 QRP-025 的常驻 VRAM、raygen surface-write 带宽和 NGX input read 带宽。按上述参考布局，现有 64 bytes/pixel guides 理论上可降至约 26 bytes/pixel（不含新增 specular distance）。

#### 风险与验证

格式支持不能从 Vulkan wrapper 直接外推到 CUDA wrapper；必须用当前 DLSS SDK 官方格式契约或 feature create/evaluate 验证。depth 量程、motion 精度、world normal 量化和高动态 specular albedo 均需画质 A/B；R16F depth 在大尺度场景可能不够，不能只按 VRAM 最小化。

### QRP-O11：OptiX pipeline 未把 primitive type 限定为 triangles

#### 代码事实

- `optix/src/pipeline.cpp:72-81` 构造 `OptixPipelineCompileOptions` 时没有设置 `usesPrimitiveTypeFlags`，保持零值。
- 项目所有 BLAS build input 都是 `OPTIX_BUILD_INPUT_TYPE_TRIANGLES`，没有 custom primitives、curves、spheres 或 micromesh primitive program。
- OptiX 9.1 Programming Guide 明确建议：场景只有 built-in triangles 时，将 `usesPrimitiveTypeFlags` 设为 `OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE` 以获得最佳性能。

#### 优化机会与验证

按实际 primitive 集合收窄 compile options，可让 OptiX 针对确定的 triangle traversal/program 路径优化。收益需通过 pipeline compile log、OptiX/Nsight shader 指标和 frame time A/B 验证；它应是低风险编译配置优化，但必须与 module 和 pipeline 使用的同一组 `OptixPipelineCompileOptions` 保持一致。

### QRP-O12：场景切换无条件销毁并重新加载独立的 environment map

#### 代码事实

- `app/src/scene_loader.cpp:899-918` 的 `SceneLoader::destroy()` 同时调用 `destroy_env_map()`。
- `app/src/application.cpp:534-563` 每次 `switch_scene()` 先销毁整个 loader，场景/AS 重建后再按同一路径重新调用 `load_env_map()`。
- BC6H cubemap 可命中磁盘 cache，但 `load_env_map()` 仍重新读取并解码完整 HDR、从原始像素重建 env alias table、重新分配和上传资源。

#### 优化机会

环境光与 glTF scene geometry/material 生命周期独立；普通 scene switch 不需要销毁不变的 env cubemap/alias table。拆分 scene resources 与 environment resources 的所有权可降低切换延迟、CPU 峰值与 GPU allocation churn，并减少 QRP-007 的失败窗口。只有 env path 变化或显式卸载时才重建环境资源。

### QRP-O13：BRDF lobe selection 只看 Schlick Fresnel，可能把多数 samples 分给近零能量 lobe

#### 代码事实

- `renderer/include/qualquer/renderer/brdf.cuh:734-739` 的 `specular_probability()` 仅使用 `luminance(F_Schlick(NdotV,F0))`，再 clamp 到 `[0.01,0.99]`。
- 该概率不看 diffuse `base_color`/`diffuse_weight`、roughness、Turquin compensation 或已经计算出的 directional `E_glossy_rgb`。
- 纯 metal 被单独强制 `p_spec=1`，但 dielectric/mixed material 没有等价的零能量 lobe fast path。

#### 反作用示例与验证

黑色 dielectric 的 diffuse BRDF 近零，但 normal-incidence `p_spec≈0.04`，约 96% bounce samples 会选择并评估零贡献 diffuse lobe；剩余 specular samples 再以约 25 倍权重补偿，虽保持无偏却显著增大方差。深色 mixed-metal、rough compensated specular 也可能失配。

应以两 lobe 的估计 directional energy/throughput 构造 selection probability，至少对近零 diffuse/specular lobe 做确定性选择。用相同 ray budget 对黑色 dielectric、深色 rough material 和 furnace scene 比较 RMSE/firefly percentile，并同步观察分支相干性；更精确概率若增加过多寄存器/算术，需以端到端 frame-time × variance 评估净收益。

### QRP-O14：所有 CUDA/OptiX device code 固定以 compute 8.9 为前端目标，新架构仅依赖 JIT forward compatibility

#### 代码事实

- 顶层 `CMakeLists.txt:9` 固定 `CMAKE_CUDA_ARCHITECTURES 89`。CMake 官方语义是无 `-real/-virtual` 后缀时同时生成 sm_89 real code 与 compute_89 virtual PTX，因此更新架构可通过 PTX forward JIT 运行，并非静态兼容性错误。
- `cmake/CompileOptiXIR.cmake:75-101` 也固定 `--gpu-architecture=compute_89` 生成 OptiX IR。
- 这保证 Ada 最低基线，却没有为已知更新目标生成其 native cubin/更高 virtual architecture 前端代码。

#### 可能反作用与验证

Blackwell/后续 GPU 会在首次使用普通 CUDA kernels 时承担 PTX JIT/cache 成本；更重要的是，前端按 8.9 feature set 编译，无法生成受更高 `__CUDA_ARCH__` 条件控制的代码或利用只在更新 virtual architecture 暴露的能力。driver/OptiX backend 仍可针对实际 GPU 重新优化，因此不能仅凭配置断言 steady-state 变慢。

保留 8.9 PTX 作为最低兼容 fallback，同时为明确支持的新架构生成 native code，并评估 OptiX IR 是否应按部署目标拆分。分别比较冷启动 JIT 时间、warm-cache kernel/OptiX frame time、register/local-memory 和 SASS；不要用 `native` 取代可分发的 Ada fallback，也不要无测量地假设更高 `compute_xx` 必然更快。

### QRP-O15：1:1 presentation blit 仍请求 linear filtering

#### 代码事实

- `display_buffer_` 始终按当前 swapchain extent 创建；CUDA tonemap 也直接输出 display resolution。
- `renderer/src/renderer.cpp:1050-1081` 的 blit source/destination rectangles 完全同尺寸，却指定 `VK_FILTER_LINEAR`。
- 1:1 pixel-center 映射没有缩放，linear 与 nearest 应读取同一 source texel；linear 不提供额外重建质量，却额外要求 source format 的 linear-filter blit capability（QRP-036）。

#### 优化机会与验证

改用 `VK_FILTER_NEAREST` 可简化 capability 前提，并避免实现无法折叠 linear filter 时的多余过滤工作；float→sRGB format conversion 仍由 blit 执行。先做逐像素图像 diff 确认 1:1 映射完全一致，再看 Vulkan transfer timestamp；预期性能收益很小，主要价值是更准确的语义和更宽能力覆盖。

### QRP-O16：除精确 `Release` 外都部署 DLSS development DLL，可能污染常用 profiling 配置

#### 代码事实

- `app/CMakeLists.txt:62-68` 仅当 `$<CONFIG:Release>` 为真时复制 `nvngx_dlssd.dll` 的 rel 版本；`RelWithDebInfo`、`MinSizeRel` 和自定义优化配置均得到 dev DLL。
- 其他代码以 `NDEBUG` 区分 release/debug，因此 `RelWithDebInfo` 通常已经走优化后的 renderer/OptiX 路径，却仍搭配 NGX development runtime。

#### 可能反作用与验证

CLion/Nsight 常用 `RelWithDebInfo` 保留符号做 profiling；此时 DLSS timing、VRAM、日志与画质行为可能不代表 release runtime，导致错误归因。部署规则应按“仅 Debug 用 dev，其余优化配置用 rel”或显式 profile option 选择，并在启动日志中记录实际 DLL/version。用同一 scene 对 dev/rel DLL 比较 evaluate 时间与输出，正式性能结论必须基于可发布 runtime。

### QRP-O17：静态 alpha-mask geometry 未利用 Ada 的 opacity micromaps

#### 代码事实与官方依据

- Mask geometry 当前始终走 any-hit，并为每个候选 intersection 插值 UV/vertex alpha、执行 bindless BC7 texture fetch 与 cutoff：`programs.cu:746-781`。
- `OptixPipelineCompileOptions::allowOpacityMicromaps` 保持 false，BLAS build input 也没有 OMM attachment；在当前“不使用 OMM”的实现下保持 false 本身是正确且更高效的配置。
- OptiX 9.1 Programming Guide §5.12 明确说明 OMM 可把 triangle microregions 预分类为 opaque/transparent，只让 unknown 区域调用昂贵 any-hit，专门降低高分辨率 alpha content 的 traversal 开销。

#### 优化机会与取舍

对静态 foliage/fence 等 `alphaMode=MASK` 资产，可从 base alpha、factor、vertex alpha 与 cutoff 预构建 2-state/4-state OMM；透明区由 traversal 直接跳过、实心区直接接受，边界 unknown 才运行现有 any-hit。这尤其适合 Ada 及更新 RT core，但会增加 scene preprocessing、OMM array/GAS 显存、cache/version 管理，并必须正确处理 UV wrap、texture transform、alpha coverage 与 QRP-039 的 emissive mask。

用高-overdraw foliage scene 比较 any-hit invocation 数、RT core busy、texture requests、GAS/OMM VRAM、build time 和稳态 frame time；简单低覆盖 mesh 可能因额外内存/构建成本得不偿失，应按材质/triangle coverage 选择性启用。

### QRP-O18：normal-map variance 未反馈到 microfacet roughness，specular aliasing 只能交给采样/DLSS

#### 代码事实

- normal map mips 只对 encoded RG 做普通 resize/BC5 压缩；device 端重建 Z 并归一化 shading normal。
- GGX `roughness/alpha` 完全来自 metallic-roughness texture/factor，不依据当前 ray footprint 内的 normal variance 调整。
- QRP-029 又表明当前尚未选择正确 normal-map mip；即使先补 LOD，单纯平均并重新归一化 normal 也会丢失被滤掉的法线方差。

#### 优化机会与验证

Ray Cone LOD 落地后同步加入 Toksvig/LEAN 类 normal-map specular AA 或等价 variance-to-roughness 策略，可把 subpixel normal 高频转为更宽的 microfacet lobe，降低远景/运动中的闪烁、firefly 与 DLSS 输入噪声。代价包括额外 mip metadata/channel、roughness 算术与材质偏软风险；用高频 normal、grazing highlight、不同 render scale 做 temporal flicker/RMSE 与 frame-time A/B。该项已在 Phase 4.5 路线中规划，当前属于尚未落地的画质优化，不单独视为 API 错误。

### QRP-O19：DLSS-off Catmull–Rom upscale 每个 display pixel 执行 16 次 point fetch

#### 代码事实

- `renderer/src/tonemap.cu:40-78` 显式遍历 4×4 taps，并从 RGBA32F accumulation array 做 16 次 `tex2D<float4>` point read。
- 仅在 `render < display` 且 DLSS 未产出结果时触发；native 1:1 与 downscale 不走该分支。
- `tasks/phase4.5.md` Step 14.9 已把“Catmull-Rom tonemap 16→4 fetch”列为当前未完成项。

#### 优化机会与验证

利用 bilinear texture fetch 合并同号/相邻 taps 可把多数 Catmull–Rom reconstruction 降到 4 次 fetch，显著减少低分辨率 fallback 的 texture traffic；但负权重、edge clamp 与精确 kernel 等价必须推导正确，不能用普通 bicubic 近似冒充。对边界/半像素坐标做逐像素 reference diff，并用 CUDA display event、L1/L2/texture throughput 比较 16-fetch 与 4-fetch；若 fallback 很少使用，工程收益应与实现复杂度一并评估。

## 6. 已检查且目前未发现错误的部分

- FrameSlot 的 color、aux 和 metadata 采用同槽生产/消费边界，事件依赖方向基本闭合。
- forward semaphore 按 frame 分配、reverse semaphore 单份交替使用；正常帧与 acquire-out-of-date drain 路径目前未发现明确死锁或二进制 semaphore 重用错误。
- Vulkan external queue-family acquire/release 的所有权方向与 CUDA 外部访问模式一致。
- RGBA16F 到当前固定 `B8G8R8A8_SRGB` swapchain 的 Vulkan blit 会按规范进行浮点格式转换/通道映射，并在写入 sRGB destination 时执行线性到非线性编码；在 QRP-036 的 capability 前提成立时，该色彩转换思路正确。
- OptiX typed payload 的当前 16-word 语义与 closest-hit/miss 的读写用途大体匹配。
- `cc05ace` 对 temporal offset 的移除已同步 device、DLSS jitter 和调用点；当前不存在旧中间态中的参数数量错误。
- 已核对项目使用的 Dear ImGui Vulkan backend v1.92.6：backend 的 main-window render buffers 按初始化 `ImageCount` 自行轮转，并不按本项目的 acquired image index 索引；本项目最多仅有两个 frame slot，旧 count 大于新 count 只会多保留 buffer，旧 count 小于新 count 时两个 buffer 仍受 N-2 fence 保护。未调用 `ImGui_ImplVulkan_SetMinImageCount()` 在当前无 multi-viewport、自管 swapchain 的路径中暂未形成正确性问题。

## 7. 本地官方参考实现

后续对照直接读取本地仓库源码，不使用 DeepWiki 的二次归纳：

- `/mnt/d/Github/optix-subd`，origin `https://github.com/NVIDIA/optix-subd.git`，本地提交 `946b664`
- `/mnt/d/Github/vk_denoise_dlssrr`，origin `https://github.com/nvpro-samples/vk_denoise_dlssrr.git`，本地提交 `791f631`
- `/mnt/d/Github/vk_gltf_renderer`，origin `https://github.com/nvpro-samples/vk_gltf_renderer.git`，本地提交 `5717e5d`

已初步确认这些参考实现共同采用：

- 线性正 view depth；
- unjittered motion vector，并把 jitter 单独传给 NGX；
- motion vector 指向 previous position；
- world-space normal；
- 相机突变、模式/尺寸变化时 reset history。

Qualquer 的 motion-vector X/Y 符号与 `InMVScaleX/Y` 组合目前能还原为 previous-pixel minus current-pixel，尚未发现确定的方向错误。参考实现对 sky guide 的处理不完全相同，不能仅凭实现差异判定 Qualquer 错误。

## 8. 审查结论与修复顺序

本轮静态审查已覆盖既定链路并完成收口：共确认 **42 项问题（QRP-001～QRP-042）**，其中 15 项含“高”严重度；另有 **19 项不能靠静态代码定量的性能/架构候选（QRP-O01～QRP-O19）**。当前没有未决候选。所有结论均基于 `061e55ac65340b1feb3a8857bfa6db4b5493c46a`；未构建、未运行，性能候选不冒充实测结论。

建议按依赖关系而非编号顺序处理：

1. **先消除 API 未定义/同步风险**：QRP-001、QRP-024；同时补齐 QRP-036 的完整 device capability gate，避免修复只在当前驱动偶然成立。
2. **再统一 light transport estimator**：QRP-006、QRP-008～010、QRP-020、QRP-032、QRP-033、QRP-039。这里包含几何/着色法线域和 alpha-emissive domain 两组相互影响的问题，必须一起做无偏 reference 验证。
3. **修复跨帧状态与 DLSS inputs**：QRP-004、QRP-005、QRP-007、QRP-013、QRP-026、QRP-030、QRP-034、QRP-037、QRP-038；先保证 color/depth/MV/material guides 描述同一 surface，再判断 QRP-O09/O10 的额外 guides/格式优化。
4. **修复资产与纹理语义**：QRP-002、QRP-014～017、QRP-019、QRP-027～029、QRP-035、QRP-041、QRP-042。Ray Cone LOD 前必须先解决 level-0 重采样、tail padding 与 sampler 语义，否则启用 mip 只会暴露更多错误数据。
5. **最后处理资源与吞吐**：QRP-021、QRP-025 及 QRP-O01～O19。先取得 release resource report/Nsight 基线，再逐项 A/B；不因“理论上更少工作”就默认在 Ada/Blackwell 上更快。
6. 低风险 bookkeeping/cache/swapchain recovery（QRP-011、QRP-012、QRP-022、QRP-023、QRP-028、QRP-040）可穿插处理，但不得与 estimator 修复混在同一画面对照中。

## 9. 验证矩阵

| 验证组 | 覆盖项 | 最低验证内容 |
|---|---|---|
| Vulkan/CUDA interop | 001、024、036、040 | 开启 Vulkan synchronization validation；循环 resize/minimize/restore、切 present mode、强制 acquire/present stale；确认无 layout/queue-family/semaphore hazard。验证 Vulkan image usage 与 CUDA array flags 一一对应。 |
| 光输运 reference | 006、008～010、018、020、032、033、039、042 | 关闭 firefly clamp，用 CPU/reference path tracer 或高样本 ground truth 对比；分别测纯 env、纯 emissive、混合光源、纬度高亮 env、负 scale、single-sided shell、强 normal map、alpha-masked emissive 与 roughness 0。记录均值偏差与 RMSE，不只看降噪输出。 |
| 数值与 PDF | 008、009、010、031、032、033 | 对 BRDF/CLTC/MIS 做 finite-property、hemisphere-support、sample/eval PDF 一致性和 white-furnace tests；覆盖 `r=1, NdotV≈1`、grazing、极远/极近发光体及 Inf/NaN 注入。 |
| glTF conformance | 002、014～017、019、020、027、035、039、041、042 | 使用无 material、`TEXCOORD_1`、冲突 mag/min filter、非 4 倍尺寸、negative/small scale、skin、非零 morph weights、Mask/Blend emissive、roughness 0 的最小资产逐项截图与几何/bounds 检查。 |
| 纹理与 cache | 012、022、023、027～029 | checker/grid 与 alpha coverage 图像逐 mip diff；远距、斜视和 secondary reflection temporal A/B；并发相同 source/role、截断 cache、旧算法 cache 与多进程 writer 恢复测试。 |
| DLSS-RR | 004、007、013、026、030、034、037、038、O09、O10 | 使用 release NGX DLL；相机快速旋转/穿 near plane、env rotation、max-bounce change、pause/resume、scene/HDR failure、empty geometry、UltraQuality/preset 切换；观察 debug overlay、history reset、disocclusion 与 guide dump。注入 create/evaluate failure 验证 fallback。 |
| Sobol/累积 | 004、005 | host 单元测试模拟动态 spp、暂停、空场景、DLSS toggle 与 `uint32_t` 边界，断言 sample indices 单调唯一、count 不回绕为“首帧”；画面做固定 sample-budget convergence A/B。 |
| GPU 资源/吞吐 | 021、025、O01～O19 | Release 与 RelWithDebInfo 分开；记录 OptiX register/continuation stack/local memory、occupancy、SER/any-hit 数、L1/L2/texture traffic、stream overlap、冷/热 JIT、BLAS build/compact、DLSS/tonemap 时间及 peak VRAM。每个候选只改变一个变量。 |
| 生命周期 | 007、011、023、034、037、040 | 启动无 scene、scene load failure、HDR failure、反复 scene switch、同尺寸/异尺寸 swapchain recreate 和正常退出；确认无 stale stats/handles/history、无永久坏 cache、无 optional-feature failure 导致进程终止。 |

构建和运行验证由用户在 CLion/Nsight 中执行；本报告不把尚未执行的验证写成“通过”。
