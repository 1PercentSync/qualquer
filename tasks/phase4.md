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
> 决策记录见 `docs/phase4-discussion.md`（D31-D41）。
> 设计详见 `docs/current-phase.md` Phase 4.5 节。
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

### 前半部分

## Step 9：IBL 旋转 + Russian Roulette

- [x] IBL 旋转：LaunchParams 新增 `env_rotation`（float, radians），`__miss__env` 和 env NEE 采样方向应用 Y 轴旋转（cos/sin），左键拖拽旋转 IBL（Himalaya UX：灵敏度 ~0.003 rad/px，面板只读显示角度），参数变化触发累积 reset
- [x] Russian Roulette：bounce ≥ 2 时启用，存活概率 = `clamp(max_component(throughput), 0.05, 0.95)`，存活时 `throughput /= survival_prob`，死亡时终止 path，使用 RNG 维度 base + 3（已预留）
- [x] 请求用户在 CLion 中编译验证（IBL 旋转可调，RR 减少长路径计算）

## Step 10：Sobol + Hash 去相关 RNG

- [x] Sobol direction numbers 数据准备：Joe & Kuo direction numbers `inline constexpr` C 数组（128 维 × 32 bit，4096 entries，16 KB）
- [x] LaunchParams 内嵌 Sobol 数组（`uint32_t sobol_directions[4096]`）+ Renderer 初始化填充
- [x] `rng.cuh` 重写：新增 xxhash32 device 函数（96-bit 多维混合）；`sobol_sample(dimension, sample_index)` 查询 Sobol 序列 + `pcg_hash(pixel_index)` per-pixel 加法 Cranley-Patterson rotation + golden-ratio temporal offset（`frame_index * 2654435769u`）；dim ≥ 128 fallback xxhash32
- [x] 验证：维度分配不变（dim 0-1 subpixel jitter, per-bounce base = 2 + bounce × 12），现有采样行为正确
- [x] 请求用户在 CLion 中编译验证（低 spp 下噪声更均匀，收敛更快）

## Step 11：Render Resolution Decoupling

- [x] 新增 `render_width` / `render_height` 参数（独立于 swapchain extent），UI 滑块直接控制渲染高度（如 1080），宽度按输出纵横比自动计算
- [x] 累积 buffer 按渲染分辨率分配（不再跟随 swapchain）
- [x] OptiX launch 维度使用渲染分辨率
- [x] Tonemap kernel 输入渲染分辨率、输出显示分辨率
- [x] Camera jitter 基于渲染分辨率像素大小
- [x] 窗口 resize 时仅重建显示 buffer，累积/aux buffers 按需（渲染分辨率变化时才重建）
- [x] 请求用户在 CLion 中编译验证（可配置渲染分辨率，画面正确缩放显示）

## Step 12：Aux Data 写入

- [x] Aux input buffers 分配（CUDA array + `cudaTextureObject_t`，渲染分辨率）：depth（R32F）、motion vectors（RG32F）、diffuse albedo（float4）、specular albedo（float4）、normals（float4）、roughness（R32F）；specular hit distance 不分配（optional 输入，传 nullptr）
- [x] DLSS-RR output buffer 分配（CUDA array + `cudaSurfaceObject_t`，输出分辨率，float4）：中间 HDR buffer，DLSS-RR 写入、tonemap 读取
- [x] LaunchParams 扩展：aux buffer 指针 + 前帧 VP 矩阵
- [x] Closesthit bounce==0 写入：view-space Z depth、diffuse albedo、specular albedo、shading normal、linear roughness
- [x] Motion vectors：raygen 计算屏幕空间 MV，hit 像素和 miss 像素均需写入
- [x] 多 spp jitter 策略（D37）：raygen sample loop 内所有 sample 共享同一 subpixel jitter（per-frame），aux data 写一次即可；BRDF/NEE 维度仍 per-sample
- [x] Debug view 验证：临时修改 tonemap 输出各 aux buffer 内容，确认数据正确后丢弃改动（depth / diffuse albedo / specular albedo / normals / roughness / MV 六项均验证通过；发现并定位 hit_distance 语义混用 bug）
- [x] hit_distance 语义修复（debug view 验证中发现）：closesthit 无效 BRDF 采样分支的 `payload_set_hit_distance(-1.0f)` 改为写真实 `optixGetRayTmax()`，路径终止由 throughput_update=0 承担（raygen throughput < 1e-6 检查 break，本跳 emissive/NEE 贡献保留）；`hit_distance < 0` 从此只表示几何 miss。原混用导致无效采样像素被 raygen 首跳捕获误判为 sky，aux data 被 sky 默认值覆盖成随机噪点
- [x] 请求用户在 CLion 中编译验证（debug view 下各 aux buffer 内容正确）

## Step 13：DLSS-RR SDK 接入

- [x] DLSS SDK CMake 集成：`third_party/dlss` 预编译二进制（`include/` + `lib/` + `bin/`），链接 `nvsdk_ngx` 静态 loader，DLL 部署到运行目录
- [x] DLSS-RR 封装类：NGX 初始化（`NVSDK_NGX_CUDA_Init_with_ProjectID`）、capability 查询、feature 创建（`NGX_CUDA_CREATE_DLSSD_EXT`，传入 CUcontext + display_stream）、release、shutdown
- [x] 资源管理：窗口 resize 或渲染分辨率变化时 release + recreate feature；InPerfQualityValue 根据实际放大比率自动选取
- [x] 请求用户在 CLion 中编译验证（DLSS-RR 初始化成功、feature 创建成功，渲染输出不变）

## Step 14：DLSS-RR 管线接入

- [x] Ping-pong buffer 迁移：`CudaBuffer<float4>` → `CudaArrayBuffer<float4>`；LaunchParams 累积 buffer 字段改为 `color_output`（surf）/ `color_input`（tex）+ 新增 `dlss_enabled`；Renderer `accum_buffers_` 类型与 init/destroy/resize 适配
- [x] Raygen 适配：写入改 surf2Dwrite、Separate Sum 读改 tex2D；`dlss_enabled` 为 1 时单帧输出（不读 read buffer、不累加），为 0 时保留 Separate Sum
- [x] DlssRR evaluate：新增 `evaluate()` 方法，填充 `NVSDK_NGX_CUDA_DLSSD_Eval_Params`（aux inputs 传 `CUtexObject*`、output 传 `CUsurfObject*`），调用 `NGX_CUDA_EVALUATE_DLSSD_EXT`
- [x] Tonemap 适配：读取源改为 `cudaTextureObject_t`；DLSS ON 读 `dlss_output` tex（1:1 显示分辨率，无除法），OFF 读 accum slot tex（resampling + 除 count）；Renderer `submit_cuda` 双路径分支；DLSS ON 全局 jitter（host Sobol 无 CP rotation）、DLSS OFF 恢复 per-sample jitter
- [x] UI 适配：DLSS-RR 面板（开/关、render preset 选择、只读显示 quality mode 名称和渲染/输出分辨率）；accumulated samples 改为 DLSS OFF 时显示
- [x] UI 适配：VRAM 占用只读显示；`create_feature` 使用 `resolve_render_height` 返回的 clamped height；resolve 算法修正（单轮 clamp+optimal 距离）；DLSS OFF 时分辨率变化不创建 feature；optimal settings 提前 cache；slider 弹回 resolved 值
- [x] InReset：拆除 `needs_reset` → `eval.InReset` 绑定，改为 `reset_accumulation()` 统一触发（场景切换 / F 键瞬移 / HDR 重载 / Reset 按钮），连续相机运动和参数变化仅 reset 累积不丢弃 DLSS 历史
- [ ] 请求用户在 CLion 中编译验证（ON 输出干净放大画面，OFF 保持原有累积行为，preset 可切换）

## Step 14.5：Step 11–14 正确性修复

- [x] `slider_uint_on_release` / `slider_float_deferred` 弹回修复：widget active 期间将拖拽值存入 ImGui StateStorage，释放帧从 StateStorage 取回最后的拖拽值用于提交（两个函数使用同一模式，一起修复）
- [x] 单面 back-face pass-through aux 默认值：closesthit 中 pass-through return 前，若 `bounce == 0 && first sample`，写入 sky 默认值（depth=inf, normal=0, roughness=0, diffuse albedo=0, specular albedo=0），语义为「此像素无有意义的表面信息」
- [x] DLSS color、aux data 与 host metadata 同槽 ping-pong，消除跨 stream 竞争和帧错配
- [x] 首次 evaluate 有效性状态机：仅消费完整有效的 DLSS input slot，历史失效后的首个输入以 reset 和有效零 MV 启动
- [x] 修复 DLSS ON 单帧 radiance 均值语义，并在 0 spp 暂停时保持最后有效输出
- [x] DLSS ON primary ray 提出 sample loop，loop body 抽为 `__forceinline__` 函数
- [x] MV Y 分量符号修正：`eval.InMVScaleY = -1.0f`
- [x] Render preset 变化触发 feature 重建：加 `prev_dlss_preset_` 检测
- [x] `cache_optimal_settings` 错误处理：当前任一 mode 查询失败会提前 return 跳过剩余 mode，改为单 mode 失败不影响其余 mode 的查询
- [x] 按 feature 生命周期缓存 DLSS-RR VRAM 统计，移除每帧 NGX stats 查询
- [x] NGX 崩溃诊断：NGX init 时提供日志回调（桥接 spdlog，`ON`，`DisableOtherLoggingSinks`）；所有 abort 宏（CUDA/OPTIX/VK/NGX）在 abort 前 flush spdlog
- [x] display stream 排序修复：保留 SER 并恢复 NGX/default-stream 所需顺序，多轮压力测试稳定且吞吐无显著回退
- [x] 修复 `SceneLoader` 异步上传的 host source 生命周期，确保局部 source 析构前复制完成
- [x] 重构：FrameSlot 封装——将 `accum_buffers_`、`aux_buffers_`、`accum_counts_`、`dlss_frame_metadata_`、`event_raygen_done_`、`event_tonemap_done_` 六组平行 `std::array<X,2>` 合并为 `std::array<FrameSlot, 2>`，FrameSlot 持有 alloc/resize/free/invalidate/create_events/destroy_events 方法
- [x] 重构：closesthit 瘦身——`brdf.cuh` 新增 `brdf_pdf(BrdfParams, L)` 消除 3 处 NEE/MIS PDF 重复；`nee.cuh` 新增 `evaluate_env_nee` / `evaluate_emissive_nee` 两个独立内联函数将 ~140 行 NEE 逻辑提出 closesthit；device helper `write_aux_no_surface(sx, sy, diffuse_albedo)` 参数化 diffuse_albedo 消除 pass-through 与 raygen sky 的 aux 默认值写入重复
- [x] 重构：累积 reset 检测重分类——从检测中移除 `max_bounces` / `samples_per_frame`（质量/吞吐旋钮，不改变积分对象，自适应会每帧变化）；camera 打包为 `CameraKey`（`inv_view` + `inv_projection`，C++20 defaulted `==`），`env_rotation` / `dlss_enabled` 保持独立 prev_ 成员；暂停期间仅 camera 变化触发 `invalidate_dlss_history`，content 变化不触发
- [ ] 重构（草案，执行前需讨论）：SceneRenderInput 场景资源打包——`launch_params.h` 新增 `EnvLightData` / `EmissiveLightData` POD 结构体，SceneLoader 返回、SceneRenderInput 持有、LaunchParams 内嵌，替代 10 个散装字段
- [ ] 重构（草案，执行前需讨论）：DlssRR 所有权移入 Renderer——`dlss_rr_` 从 Application 移入 Renderer 成员（init 时从 Context 取 device_id 初始化），`dlss_preset` 移入 RenderSettings，submit_cuda 不再接收 `DlssRR&` 参数，Renderer 暴露 `const DlssRR& dlss() const` 供 DebugUIContext 借用
- [ ] 重构（草案，执行前需讨论）：LaunchParams sobol 外移验证——将 `sobol_directions[4096]` 改为 global memory 指针（LaunchParams 内 8 bytes 替代 16384 bytes），benchmark 对比 1spp/32spp 吞吐确认无回退后合入
- [ ] 约束资产辐射度与材质输入的物理数值域
- [ ] 保证几何与 shading frame 在退化输入下仍有效
- [ ] 保证 BRDF 与能量模型在完整输入域内输出合法
- [ ] 保证 importance distribution 的构建、采样与 PDF 一致且有效
- [ ] 保证直接光照与 path estimator 的 contribution、概率及权重合法
- [ ] 保证时域投影与 DLSS guide data 有限、同帧且语义有效
- [x] 呈现链路计时（debug only）：CUDA timing events 测量 PT（compute_stream）和 display_stream 耗时；Vulkan timestamp queries 测量 blit + ImGui 耗时；CPU frame chrono 测量活跃工作时间；UI 显示各项 ms/占比/理想帧率，FrameStats 窗口平滑
- [ ] 请求用户在 CLion 中编译验证（render height / exposure / FOV 滑块拖拽释放后值正常提交；相机进入单面几何体内部时无 aux data 陈旧伪影；垂直相机运动下 DLSS-RR 无 ghosting）

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

