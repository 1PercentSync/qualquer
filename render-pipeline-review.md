
## 5. 既有优化可能产生反作用的检查项

以下不是静态代码即可定性的错误。必须通过 release 编译资源报告、Nsight Systems/Compute、GPU 时间线和图像 A/B 验证。

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


