# Phase 4 任务清单

> 目标：核心 Path Tracer（BRDF、多 bounce、IBL、NEE+MIS、累积收敛。4000 spp 大致收敛）
> 详细设计见 `docs/current-phase.md`，决策记录见 `docs/phase4-discussion.md`，技术决策见 `docs/technical-decisions.md`。
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

---

## Step 1：Infrastructure & 代码重组

- [x] 扩展 `launch_params.h`：新增 PT 参数（max_bounces、samples_per_frame、sample_count、frame_seed、exposure）、环境光参数（env_cubemap、env_alias_table、env_alias_count、env_alias_width、env_alias_height、env_total_luminance）、emissive 参数（emissive_triangles、emissive_alias_table、emissive_count、emissive_total_power）。定义 EnvAliasEntry（prob + alias + luminance）、AliasEntry（prob + alias）、EmissiveTriangle 结构体
- [x] Pipeline 更新：numPayloadValues=18、numAttributeValues=2、maxTraceDepth 更新、stack size 调整
- [ ] Renderer 语义拆分：frame_counter_（slot 索引，永不 reset）、sample_count_（总累积数，camera/config 变化时 reset）、frame_seed_（temporal scramble，永不 reset）
- [ ] Renderer 累积 reset 逻辑：camera 或渲染参数变化时 sample_count_ = 0、清零累积 buffer
- [ ] 创建 `math_utils.cuh`：将 programs.cu 中现有 float3/float2/float4 运算符、dot/cross/normalize、矩阵乘法、DeviceVertex 定义、get_shading_normal 移入。programs.cu 改为 `#include` 此文件
- [ ] 请求用户在 CLion 中编译验证（现有渲染行为不变）

## Step 2：Tonemap + Exposure

- [ ] 创建 `tonemap.cuh`：Khronos PBR Neutral tonemap 函数、exposure 应用（pre-tonemap multiply）
- [ ] tonemap kernel 更新：累积 buffer 读取时除以 sample_count（Separate Sum）、应用 exposure + PBR Neutral
- [ ] 请求用户在 CLion 中编译验证（ambient 着色经 tonemap 正确显示）

## Step 3：BRDF 函数

- [ ] 创建 `brdf.cuh`：GGX NDF（D）、Smith height-correlated G2、Fresnel Schlick（F0 + F90）、Lambertian diffuse、metallic/dielectric lobe weighting、VNDF 采样（GGX visible normal）、cosine hemisphere 采样、combined multi-lobe PDF
- [ ] BRDF eval + sample 接口：specular_probability、sample + throughput_update 计算（specular lobe 和 diffuse lobe）、build_orthonormal_basis
- [ ] 请求用户在 CLion 中编译验证（函数编译通过，Step 5 集成到 closesthit）

## Step 4：环境光

- [ ] AppConfig 新增 `env_map_path` 字段（HDR 环境贴图路径）
- [ ] HDR equirect 加载：stb_image 加载 .hdr
- [ ] Equirect → cubemap 转换：CUDA kernel 实现 equirect 到 6-face cubemap 转换
- [ ] Cubemap 纹理创建：cudaMipmappedArray（cubemap flag）→ cudaTextureObject_t
- [ ] Env alias table 构建：半分辨率降采样 + luminance × sin(theta) 权重 + Vose's algorithm O(N) 构建 EnvAliasEntry 数组上传 GPU
- [ ] SceneLoader 集成：HDR 加载 + cubemap 创建 + alias table 构建，资源生命周期管理（destroy / 场景切换时重建）
- [ ] Renderer 集成：env cubemap + alias table 指针填入 LaunchParams
- [ ] Miss shader 更新：`__miss__env` 用 `texCubemap<float4>` 采样环境光
- [ ] 请求用户在 CLion 中编译验证（miss 显示 HDR 全景，命中表面仍为 ambient）

## Step 5：Bounce loop + 累积

- [ ] 创建 `rng.cuh`：PCG hash RNG（pixel_index × sample_index × dimension）、维度分配方案（dim 0-1 subpixel jitter，per-bounce base = 2 + bounce × 12）
- [ ] 创建 `pt_common.cuh`：PathState 结构体、ray offset（Wächter & Binder）、shading normal consistency check、MIS power heuristic
- [ ] 创建 `payload_helpers.cuh`：18-register payload pack/unpack 内联函数（float↔uint bit cast helpers + per-field set/get）
- [ ] Closesthit 重写（基础）：顶点插值 + normal mapping + material 参数提取 + back-face flip + ray offset + normal consistency + 18-register payload 写回（emissive 直写 emissive_factor，throughput_update = 0 终止 path）
- [ ] Closesthit 重写（BRDF 采样）：build_orthonormal_basis + specular_probability + lobe selection + VNDF/cosine sampling + throughput_update 计算 + env_mis_weight + last_brdf_pdf 写入 payload
- [ ] Raygen 重写（单 sample 骨架）：单 sample + bounce loop + PathState 驱动 + Separate Sum 累积（验证多 bounce + 累积正确性）
- [ ] Raygen 完整：sample loop（samples_per_frame 次）+ subpixel jitter + sample_count 递增
- [ ] SER 集成：optixTraverse + optixReorder + optixInvoke 替换 optixTrace
- [ ] 请求用户在 CLion 中编译验证（多 bounce PT 在 HDR env 下工作，累积收敛可见）

MUSTREAD:4
## Step 6：Alpha 处理 + SBT 扩展

- [ ] SBT 扩展为 2 hitgroup：opaque（DISABLE_ANYHIT）+ non-opaque（closesthit + anyhit），SBT record 数组重建
- [ ] Pipeline 更新：anyhit program 注册
- [ ] 创建 `__anyhit__alpha`：alpha_mode==Mask 时采样 alpha 纹理、< alphaCutoff 则 optixIgnoreIntersection
- [ ] Closesthit 扩展：back-face + !double_sided 时 pass-through（throughput 不变，消耗一次 bounce）
- [ ] 创建 `__miss__shadow`（missIndex=1）：设 visible=1。Shadow ray 调用时用 DISABLE_CLOSESTHIT + TERMINATE_ON_FIRST_HIT flag
- [ ] SceneLoader / Renderer 适配：几何体按 alpha_mode 分配 sbtOffset（0=opaque, 1=non-opaque）、BLAS opaque flag 按 material 判断
- [ ] 请求用户在 CLion 中编译验证（树叶/栅栏正确镂空，单面材质背面穿透）

## Step 7：NEE + MIS

- [ ] Emissive 三角形收集：SceneLoader 遍历材质 emissiveFactor > 0 的三角形、变换到世界坐标、计算面积 × luminance(emissive_factor) power、构建 EmissiveTriangle 数组 + AliasEntry alias table、上传 GPU
- [ ] Env NEE：closesthit 中 alias table O(1) 采样环境光方向、计算 env pdf（查 EnvAliasEntry.luminance）、发射 shadow ray、BRDF eval at light direction、MIS weight
- [ ] Emissive NEE：closesthit 中 alias table O(1) 采样发光三角形、uniform 三角形采样点、计算 emissive pdf、发射 shadow ray、BRDF eval at light direction、MIS weight
- [ ] MIS 完成：BRDF hit emissive 时的反向 MIS weight（bounce > 0 hit emissive surface → power_heuristic(last_brdf_pdf, light_pdf)）、BRDF miss env 时的 env_mis_weight 应用
- [ ] 请求用户在 CLion 中编译验证（噪声大幅降低，小光源可见，4000 spp 大致收敛）

## Step 8：UI 参数面板

- [ ] ImGui 面板：max_bounces（滑块）、samples_per_frame（滑块）、exposure（滑块）、sample_count 显示（只读）、env_map_path 输入 + 加载按钮
- [ ] 参数变化触发累积 reset
- [ ] 请求用户在 CLion 中编译验证（运行时参数可调，Phase 4 完成）
