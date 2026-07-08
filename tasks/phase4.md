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
- [ ] SceneLoader / Renderer 适配：load_scene 接收材质信息，`geom.opaque` 按 `alpha_mode == 0` 判断、BLAS geometry flag 按 opaque 设置
- [ ] 请求用户在 CLion 中编译验证（树叶/栅栏正确镂空，单面材质背面穿透）

## Step 7：NEE + MIS

- [ ] Emissive 三角形收集：SceneLoader 遍历材质 emissiveFactor > 0 的三角形、变换到世界坐标、计算面积 × luminance(emissive_factor) power、构建 EmissiveTriangle 数组 + AliasEntry alias table、上传 GPU
- [ ] Env NEE：closesthit 中 alias table O(1) 采样环境光方向、计算 env pdf（查 EnvAliasEntry.luminance）、发射 shadow ray、BRDF eval at light direction、MIS weight
- [ ] Emissive NEE：closesthit 中 alias table O(1) 采样发光三角形、uniform 三角形采样点、光源法线朝向检查（single-sided 背面拒绝）、计算 emissive pdf、发射 shadow ray、BRDF eval at light direction、MIS weight
- [ ] Shadow Terminator 修复：Chiang et al. 2019 几何因子平滑过渡，消除低模 + 平滑着色法线时 NEE 直接光照的硬阴影切边
- [ ] MIS 完成：BRDF hit emissive 时的反向 MIS weight（bounce > 0 hit emissive surface → power_heuristic(last_brdf_pdf, light_pdf)）、BRDF miss env 时的 env_mis_weight 应用
- [ ] 请求用户在 CLion 中编译验证（噪声大幅降低，小光源可见，4000 spp 大致收敛）

## Step 8：UI 参数面板

- [ ] ImGui 面板：max_bounces（滑块）、samples_per_frame（滑块）、exposure（滑块）、累积 sample 数显示（只读，读 accum_counts）
- [ ] 参数变化触发累积 reset（chain_count 归零，无需清零 buffer）
- [ ] 请求用户在 CLion 中编译验证（运行时参数可调，Phase 4 完成）
