# Phase 4 任务清单

> 目标：核心 Path Tracer（BRDF、多 bounce、IBL、NEE+MIS、累积收敛）
>
> **范围判据**：4000 spp 大致收敛是 Phase 4/4.5 特性归属判据，非项目终点。项目长期目标是实时 PT 渲染器（见 `docs/roadmap.md`）。
> 详细设计见 `docs/current-phase.md`，决策记录见 `docs/phase4-discussion.md`，技术决策见 `docs/technical-decisions.md`。
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

---

## Step 1：Infrastructure & 代码重组

- [x] 扩展 `launch_params.h`：新增 PT 参数（max_bounces、samples_per_frame、sample_count、exposure）、环境光参数（env_cubemap、env_alias_table、env_alias_count、env_alias_width、env_alias_height、env_total_luminance）、emissive 参数（emissive_triangles、emissive_alias_table、emissive_count、emissive_total_power）。定义 EnvAliasEntry（prob + alias + luminance）、AliasEntry（prob + alias）、EmissiveTriangle 结构体
- [x] Pipeline 更新：numPayloadValues=18、numAttributeValues=2、maxTraceDepth 更新、stack size 调整
- [x] Renderer 语义拆分：frame_counter_（帧号永不 reset，%2 选 slot 兼上传 LaunchParams::frame_index 作 device temporal/RNG 源）、sample_count_（总累积数，camera/config 变化时 reset）
- [x] Renderer 累积 reset 逻辑：camera 或渲染参数变化时 sample_count_ = 0、清零累积 buffer
- [x] 创建 `math_utils.cuh`：将 programs.cu 中现有 float3/float2/float4 运算符、dot/cross/normalize、矩阵乘法、DeviceVertex 定义、get_shading_normal 移入。programs.cu 改为 `#include` 此文件
- [x] 请求用户在 CLion 中编译验证（现有渲染行为不变）

## Step 2：Tonemap + Exposure

- [x] 创建 `tonemap.cuh`：Khronos PBR Neutral tonemap 函数、exposure 应用（pre-tonemap multiply）
- [x] tonemap kernel 更新：累积 buffer 读取时除以 sample_count（Separate Sum）、应用 exposure + PBR Neutral
- [x] Raygen 改 Separate Sum 累加：LaunchParams 加 `accumulation_buffer_read` 指针（读旧总和），raygen 读旧总和 + 本帧 contribution 写新 buffer；与 tonemap 除法配对，保证 sample_count 与 buffer 内容一致
- [x] 请求用户在 CLion 中编译验证（ambient 着色经 tonemap 正确显示）

## Step 3：BRDF 函数

- [x] 创建 `brdf.cuh`：GGX NDF（D）、Smith height-correlated G2、Fresnel Schlick（标准形式 F90=1 单函数，不暴露 F90 参数重载，理由见 D28b）、EON diffuse（single-scatter FON + multi-scatter 补偿 lobe + CLTC 采样）、metallic/dielectric lobe weighting、VNDF 采样（GGX visible normal）、combined multi-lobe PDF
- [x] 多散射能量补偿：E_ss 有理多项式求值函数（Sforza 19 系数，specular 补偿）+ E_glossy 有理多项式求值函数（Sforza 39 系数，diffuse 耦合权重）+ Turquin 补偿系数计算（`1 + F0 * (1 - E_ss) / E_ss`），集成到 specular throughput_update 和 BRDF eval；diffuse 权重 = `1 - E_glossy(NdotV, roughness, F0)`
- [x] BRDF eval + sample 接口：build_orthonormal_basis、specular_probability、BrdfParams 结构体 + init_brdf_params 构造入口（集中 E_glossy/Turquin 逐通道补偿与 diffuse_weight 双重衰减）、brdf_eval（specular+diffuse BRDF 值，Step 7 NEE 用）、brdf_sample（specular VNDF / diffuse CLTC + throughput_update + combined PDF，Step 5 closesthit 用）
- [x] 请求用户在 CLion 中编译验证（函数编译通过，Step 5 集成到 closesthit）

## Step 4：环境光

- [x] AppConfig 新增 `env_map_path` 字段（HDR 环境贴图路径）
- [x] HDR equirect 加载：stb_image 加载 .hdr
- [x] 层归属重构：资产预处理（图片解码、stb_impl、BC 压缩、mip 生成、tangent 生成、cache、ktx2）renderer→app；CUDA 资源创建（finalize_texture、create_default_textures、共享类型）renderer→optix；ImGuiBackend renderer→vulkan；CMake 依赖迁移
- [x] Equirect → cubemap 转换：CUDA kernel 实现 equirect 到 6-face cubemap 转换
- [x] Env alias table 构建：全分辨率 luminance × sin(theta) 权重 + Vose's algorithm O(N) 构建 EnvAliasEntry 数组上传 GPU
- [x] SceneLoader 集成：HDR 加载 + cubemap 创建（equirect→cubemap + BC6H 压缩 + finalize_texture，含 KTX2 缓存）+ alias table 构建，资源生命周期管理（destroy / 场景切换时重建）
- [x] Renderer 集成：env cubemap + alias table 指针填入 LaunchParams
- [x] Miss shader 更新：`__miss__env` 用 `texCubemap<float4>` 采样环境光
- [x] Env map 加载 UI：DebugUI 中添加 env_map_path 显示 + Load 按钮 + Application 响应加载动作
- [x] 请求用户在 CLion 中编译验证（miss 显示 HDR 全景，命中表面仍为 ambient，运行时可加载 HDR）

## Step 5：Bounce loop + 累积 + 数据结构重构（AO 清理 + 顶点色接入）

- [x] 创建 `rng.cuh`：PCG hash RNG（pixel_index × sample_index × dimension）、维度分配方案（dim 0-1 subpixel jitter，per-bounce base = 2 + bounce × 12）
- [x] Material 重构：移除 `occlusion_tex`（uint32_t）和 `occlusion_strength`（float），host 端 `material.h` + device 端 `programs.cu` 镜像同步，更新 static_assert
- [x] Vertex 重构：新增 `color`（glm::vec4 / float4，默认 `{1,1,1,1}`），host 端 `vertex.h` + device 端 `math_utils.cuh` DeviceVertex 同步，更新 static_assert（48 → 64 字节）
- [x] SceneLoader 适配：`load_meshes` 读取 glTF `COLOR_0`（VEC3/VEC4，缺失默认白）；`load_materials` 移除 AO 纹理收集、加载、默认值回退
- [x] Closesthit 适配：移除 AO 采样，ambient 着色改为 `base_color × vertex_color`（插值顶点色后乘入 base_color）
- [x] 请求用户在 CLion 中编译验证（重构核销：现有渲染行为正确，顶点色生效，AO 已清除）
- [x] 创建 `pt_common.cuh`：PathState 结构体、ray offset（Wächter & Binder）、shading normal consistency check、MIS power heuristic
- [x] 创建 `payload_helpers.cuh`：18-register payload pack/unpack 内联函数（float↔uint bit cast helpers + per-field set/get）
- [x] Closesthit 重写（基础）：顶点插值（含顶点色）+ normal mapping + material 参数提取 + base_color × vertex_color + back-face flip + ray offset + normal consistency + 18-register payload 写回（emissive 直写 emissive_factor，throughput_update = 0 终止 path）
- [x] Closesthit 重写（BRDF 采样）：build_orthonormal_basis + specular_probability + lobe selection + VNDF/cosine sampling + throughput_update 计算 + env_mis_weight + last_brdf_pdf 写入 payload
- [x] Raygen 重写（单 sample 骨架）：单 sample + bounce loop + PathState 驱动
- [x] Raygen 完整：sample loop（samples_per_frame 次）+ subpixel jitter + accum_counts 更新
- [x] SER 集成：optixTraverse + optixReorder + optixInvoke 替换 optixTrace
- [x] 请求用户在 CLion 中编译验证（多 bounce PT 在 HDR env 下工作，累积收敛可见）

MUSTREAD:4
## Step 6：Alpha 处理 + SBT 扩展

- [x] SBT + Pipeline 扩展：Pipeline 新增 `__miss__shadow` program group，miss SBT 扩展为 2 条 record，optixTraverse stride 改 0，hitgroup 保持 1 个（CH + AH，BLAS flag 控制 anyhit）
- [x] 创建 `__anyhit__alpha`：alpha_mode==Mask 时采样 alpha 纹理、< alphaCutoff 则 optixIgnoreIntersection
- [x] Closesthit 扩展：back-face + !double_sided 时 pass-through（throughput 不变，消耗一次 bounce）
- [x] 创建 `__miss__shadow`（missIndex=1）：设 visible=1
- [x] SceneLoader / Renderer 适配：Mesh 新增 `opaque` 字段，SceneLoader 按 `alpha_mode == 0` 填充，`group_meshes` 从 `mesh.opaque` 读取（不再硬编码 `true`）
- [x] 请求用户在 CLion 中编译验证（树叶/栅栏正确镂空，单面材质背面穿透）

## Step 7：NEE + MIS

- [x] Emissive 三角形收集：SceneLoader 遍历材质 emissiveFactor > 0 的三角形、变换到世界坐标、计算面积 × luminance(emissive_factor) power、构建 EmissiveTriangle 数组 + AliasEntry alias table、上传 GPU
- [x] Env NEE：closesthit 中 alias table O(1) 采样环境光方向、计算 env pdf（查 EnvAliasEntry.luminance）、发射 shadow ray、BRDF eval at light direction、MIS weight
- [x] Emissive NEE：closesthit 中 alias table O(1) 采样发光三角形、uniform 三角形采样点、光源法线朝向检查（single-sided 背面拒绝）、计算 emissive pdf、发射 shadow ray、BRDF eval at light direction、MIS weight
- [x] Shadow Terminator 修复：Chiang et al. 2019 几何因子平滑过渡，消除低模 + 平滑着色法线时 NEE 直接光照的硬阴影切边
- [x] MIS 完成：BRDF hit emissive 时的反向 MIS weight（bounce > 0 hit emissive surface → power_heuristic(last_brdf_pdf, light_pdf)）、BRDF miss env 时的 env_mis_weight 应用
- [x] 请求用户在 CLion 中编译验证（噪声大幅降低，小光源可见，4000 spp 大致收敛）

## Step 8：UI 参数面板

- [x] ImGui 面板：max_bounces（滑块）、samples_per_frame（滑块）、exposure（滑块）、FOV（滑块）、累积 sample 数显示（只读，读 accum_counts）
- [x] 只读统计面板：Camera position + yaw/pitch、场景资产统计（Meshes/BLAS、Instances、TLAS Instances、Materials、Textures、Triangles、Vertices、Emissive triangles、Env map resolution）
- [x] 功能开关：开/关累积（关闭时 spp=0 冻结画面，保留已收敛结果）、手动 Reset 累积按钮
- [x] 参数变化触发累积 reset（chain_count 归零，无需清零 buffer）；滑块使用 deferred 模式（拖拽立即生效，Ctrl+Click 文本输入延迟到确认）
- [x] SER 性能测试：对比 SER 开/关 以及 hint/无 hint 的帧率差异
- [x] 请求用户在 CLion 中编译验证（运行时参数可调，Phase 4 完成）

---

## Phase 4.5：收敛质量 + DLSS-RR + 自适应

> 目标：DLSS-RR 集成（时域累积+去噪+放大）、采样质量提升、自适应帧率
>
> 决策记录见 `docs/phase4-discussion.md`（D31-D37）。
> 设计详见 `docs/current-phase.md` Phase 4.5 节。
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

### 前半部分

## Step 9：IBL 旋转 + Russian Roulette

- [ ] IBL 旋转：LaunchParams 新增 `env_rotation`（float, radians），`__miss__env` 和 env NEE 采样方向应用 Y 轴旋转（cos/sin），UI 滑块（deferred, 0-360°），参数变化触发累积 reset
- [ ] Russian Roulette：bounce ≥ 2 时启用，存活概率 = `clamp(max_component(throughput), 0.05, 0.95)`，存活时 `throughput /= survival_prob`，死亡时终止 path，使用 RNG 维度 base + 3（已预留）
- [ ] 请求用户在 CLion 中编译验证（IBL 旋转可调，RR 减少长路径计算）

## Step 10：Sobol + Hash 去相关 RNG

- [ ] Sobol direction numbers 数据准备：Joe & Kuo 标准文件解析为 C 数组，CMake binary embedding（128 维 × 32 bit direction vectors）
- [ ] `__constant__` memory 声明 + 主机端初始化上传
- [ ] `rng.cuh` 重写：`sobol_sample(dimension, sample_index)` 查询 Sobol 序列 + `pcg_hash(pixel_index)` per-pixel Cranley-Patterson rotation + golden-ratio temporal scramble（`frame_index * GOLDEN_RATIO`）；dim ≥ 128 fallback PCG hash
- [ ] 验证：维度分配不变（dim 0-1 subpixel jitter, per-bounce base = 2 + bounce × 12），现有采样行为正确
- [ ] 请求用户在 CLion 中编译验证（低 spp 下噪声更均匀，收敛更快）

## Step 11：Render Resolution Decoupling

- [ ] 新增 `render_width` / `render_height` 参数（独立于 swapchain extent），UI 滑块直接控制渲染高度（如 1080），宽度按输出纵横比自动计算
- [ ] 累积 buffer 按渲染分辨率分配（不再跟随 swapchain）
- [ ] OptiX launch 维度使用渲染分辨率
- [ ] Tonemap kernel 输入渲染分辨率、输出显示分辨率
- [ ] Camera jitter 基于渲染分辨率像素大小
- [ ] 窗口 resize 时仅重建显示 buffer，累积/aux buffers 按需（渲染分辨率变化时才重建）
- [ ] 请求用户在 CLion 中编译验证（可配置渲染分辨率，画面正确缩放显示）

## Step 12：Aux Data 写入

- [ ] Aux input buffers 分配（CUDA array + `cudaTextureObject_t`，渲染分辨率）：depth（R32F）、motion vectors（RG32F）、diffuse albedo（float4）、specular albedo（float4）、normals（float4）、roughness（R32F）、specular hit distance（R32F）
- [ ] DLSS-RR output buffer 分配（CUDA array + `cudaSurfaceObject_t`，输出分辨率，float4）：中间 HDR buffer，DLSS-RR 写入、tonemap 读取
- [ ] LaunchParams 扩展：aux buffer 指针 + 前帧 VP 矩阵
- [ ] Closesthit bounce==0 写入：view-space Z depth、diffuse albedo、specular albedo、shading normal、linear roughness、specular hit distance
- [ ] Motion vectors：raygen 计算屏幕空间 MV，hit 像素和 miss 像素均需写入
- [ ] 多 spp jitter 策略（D37）：raygen sample loop 内所有 sample 共享同一 subpixel jitter（per-frame），aux data 写一次即可；BRDF/NEE 维度仍 per-sample
- [ ] Debug view：UI enum 切换显示各 aux buffer 内容（depth / diffuse albedo / specular albedo / normals / roughness / specular hit distance / motion vectors）
- [ ] 请求用户在 CLion 中编译验证（debug view 下各 aux buffer 内容正确）

## Step 13：DLSS-RR SDK 接入

- [ ] DLSS SDK CMake 集成：FetchContent 或 `DLSS_ROOT` 本地路径，链接 `nvsdk_ngx_cuda` 库，DLL 部署到运行目录
- [ ] DLSS-RR 封装类：NGX 初始化（`NVSDK_NGX_CUDA_Init_with_ProjectID`）、capability 查询、feature 创建（`NGX_CUDA_CREATE_DLSSD_EXT`，传入 CUcontext + display_stream）、release、shutdown
- [ ] 资源管理：窗口 resize 或渲染分辨率变化时 release + recreate feature；InPerfQualityValue 根据实际放大比率自动选取
- [ ] 请求用户在 CLion 中编译验证（DLSS-RR 初始化成功、feature 创建成功，渲染输出不变）

## Step 14：DLSS-RR 管线接入

- [ ] Raygen 改单帧输出：移除 Separate Sum 累加逻辑，raygen 每帧输出单帧 noisy HDR 到 write buffer；ping-pong 保留——raygen 写 buffer A 时 DLSS-RR 读上一帧的 buffer B
- [ ] 每帧执行：`NGX_CUDA_EVALUATE_DLSSD_EXT` 在 display_stream 上，填充 eval params，读 noisy buffer → 写中间 HDR buffer
- [ ] Tonemap 适配：移除 sum/count 除法，tonemap kernel 在 display_stream 上读中间 HDR buffer、应用 exposure + PBR Neutral、写 LDR display buffer
- [ ] UI 适配：移除 accumulated samples 显示，新增 DLSS-RR 面板——开/关（不支持时 disable）、render preset 选择（默认 E）、只读显示：渲染分辨率、输出分辨率、VRAM 占用
- [ ] InReset：场景切换时触发（连续相机运动由 motion vectors 处理，不触发 InReset）
- [ ] 请求用户在 CLion 中编译验证（DLSS-RR 输出干净放大的画面，render preset 可切换）

## Step 15：自适应 Sample 数

- [ ] 刷新率查询：GLFW `glfwGetVideoMode` 获取当前显示器刷新率
- [ ] 帧时间测量：CUDA events 测量 raygen kernel 执行时间
- [ ] Mode 选择逻辑：根据实测帧时间判断 Mode 1 / 2 / 3，计算目标帧率
- [ ] Ping-pong / 串行切换：Mode 1 保持双 stream 并行，Mode 2/3 切换到单 stream 串行
- [ ] UI：当前 mode 显示、手动/自动切换开关
- [ ] 请求用户在 CLion 中编译验证（自适应切换正常，各 mode 帧率符合目标）

### 后半部分

## Step 16：Stochastic Alpha

- [ ] anyhit 扩展：alpha_mode==Blend 时 `hash(pixel_index, sample_index, primitive_id) > texel_alpha → optixIgnoreIntersection()`（hash 而非 Sobol，D11 决策）
- [ ] 请求用户在 CLion 中编译验证（blend 材质正确半透明）

## Step 17：Ray Cone LOD

- [ ] Payload 扩展至 19 registers：p16 → cone_width（float），p18 → cone_spread（float）
- [ ] Raygen 初始化：cone_width = 0，cone_spread = 2 × tan(0.5 × fov) / render_height（primary ray pixel footprint）
- [ ] Closesthit 更新：cone_width += cone_spread × hit_distance，bounce 时 cone_spread 根据 BRDF 散射特性更新
- [ ] 纹理采样改 `tex2DLod`：LOD = log2(cone_width × texture_resolution / triangle_footprint)
- [ ] 请求用户在 CLion 中编译验证（高频纹理 aliasing 减少，远处纹理正确模糊）

## Step 18：Normal Map Specular AA

- [ ] 基于 ray cone footprint 估算法线贴图方差（Kaplanyan 2016 方案）
- [ ] 方差叠加到 roughness²：`roughness² += variance`，clamp 后重算 alpha
- [ ] 请求用户在 CLion 中编译验证（法线贴图接缝处 specular 闪烁减少）

## Step 19：DLSS-RR 后处理

- [ ] postProcess 背景修正：sky 像素 3×3 膨胀检测（depth == inf）+ 重着色环境光（参考 optix-subd `postProcessKernel` WAR）
- [ ] 请求用户在 CLion 中编译验证（sky 伪影消除）
