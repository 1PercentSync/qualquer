## 3. 已确认问题

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

环境光与 glTF scene geometry/material 生命周期独立；普通 scene switch 不需要销毁不变的 env cubemap/alias table。拆分 scene resources 与 environment resources 的所有权可降低切换延迟、CPU 峰值与 GPU allocation churn。只有 env path 变化或显式卸载时才重建环境资源。

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