# Phase 3 任务清单

> 目标：场景加载 + 完整材质系统 + 加速结构 + Primary Ray
> 详细设计见 `docs/current-phase.md`，技术决策见 `docs/technical-decisions.md`。
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

---

## Step 1：第三方依赖集成

- [x] `vcpkg.json` 新增依赖：fastgltf、stb、glm、mikktspace、xxhash
- [x] 从 Himalaya 复制 `third_party/bc7enc/`，适配 include 路径（`himalaya/bc7enc/` → `qualquer/bc7enc/`）、创建 `CMakeLists.txt`
- [x] 顶层 `CMakeLists.txt` 新增 ISPC 语言支持 + `add_subdirectory(third_party/bc7enc)`
- [x] 请求用户在 CLion 中编译验证

## Step 2：数据结构 + Camera

- [x] 创建 `renderer/include/qualquer/renderer/vertex.h`（Vertex struct：position/normal/uv0/tangent，host + device 共享）
- [x] 创建 `renderer/include/qualquer/renderer/scene_types.h`（AABB、Mesh、MeshInstance、AlphaMode）
- [x] 创建 `renderer/include/qualquer/renderer/material.h`（Material、GPUGeometryInfo，含 static_assert）
- [x] 创建 `renderer/include/qualquer/renderer/camera.h` + `renderer/src/camera.cpp`（Camera struct：update_view/projection、forward/right、compute_focus_position），并注册 camera.cpp 到 `renderer/CMakeLists.txt`
- [x] 创建 `app/include/qualquer/app/camera_controller.h` + `app/src/camera_controller.cpp`（CameraController：右键旋转、WASD、Shift 加速、F 对焦），并注册 camera_controller.cpp 到 `app/CMakeLists.txt`
- [x] 请求用户在 CLion 中编译验证

## Step 3：纹理系统

- [x] 创建 `optix/include/qualquer/optix/cuda_texture.h`（CudaTexture struct：mipmap_array + texture_object + destroy）
- [x] 创建 `renderer/include/qualquer/renderer/cache.h` + `renderer/src/cache.cpp`（cache_root、content_hash、cache_path、atomic_write_file），并注册 cache.cpp 到 `renderer/CMakeLists.txt`（链接 xxhash）
- [x] 创建 `renderer/include/qualquer/renderer/ktx2.h` + `renderer/src/ktx2.cpp`（Ktx2Data、read_ktx2、write_ktx2），并注册 ktx2.cpp 到 `renderer/CMakeLists.txt`。支持格式集：BC7 UNORM/SRGB、BC5 UNORM、BC6H UFloat、B10G11R11_UFLOAT_PACK32、R16G16_UNORM，2D 与 cubemap（faceCount=6）
- [x] 从 KTX2/纹理模块中移除 `B10G11R11_UFLOAT_PACK32` 和 `R16G16_UNORM` 格式支持（Split-Sum 近似产物：irradiance cubemap 和 BRDF LUT，纯 PT 不需要）
- [x] 从 ISPCTextureCompressor 复制 `kernel.ispc` + `ispc_texcomp.h/cpp`，创建 `third_party/ispc_texcomp/CMakeLists.txt`，顶层 `CMakeLists.txt` 新增 `add_subdirectory(third_party/ispc_texcomp)`
- [x] 创建 `renderer/include/qualquer/renderer/texture.h` + `renderer/src/texture.cpp`，并注册 texture.cpp 到 `renderer/CMakeLists.txt`（链接 stb、bc7enc、ispc_texcomp）。LDR（TextureRole、ImageData、load_image、load_image_from_memory、ensure_bc_init、generate_cpu_mip_chain、compress_bc7/compress_bc5、load_cached_texture、compress_texture）+ HDR（compress_bc6h，共享缓存层）
- [x] 纹理 GPU 上传：在 texture.cpp 中实现 `finalize_texture`（`cudaMallocMipmappedArray` + 逐 level 上传 + `cudaCreateTextureObject`）。LDR 与 HDR（BC6H）均经此上传。可与上一项同文件实现，验收仍按复选框顺序
- [x] Default textures：实现 `create_default_textures`（1×1 white/flat_normal/black，float32×4 CUDA 纹理）。可与 `texture.cpp` 同文件实现，验收仍按复选框顺序
- [x] 请求用户在 CLion 中编译验证
- [x] 检查压缩参数是否最优（bc7enc preset、ISPCTextureCompressor BC6H profile）。并行策略：确认 OpenMP 在 SceneLoader 纹理级（Step 4），不在 `texture.cpp` 内

## Step 4：材质系统 + 场景加载

- [x] 创建 `renderer/include/qualquer/renderer/mesh.h`（generate_tangents 声明）+ `renderer/src/mesh.cpp`（MikkTSpace 实现），并注册 mesh.cpp 到 `renderer/CMakeLists.txt`（链接 mikktspace）
- [x] 创建 `app/include/qualquer/app/scene_loader.h` + `app/src/scene_loader.cpp`（SceneLoader：load / destroy / meshes / mesh_instances / scene_bounds 等），并注册 scene_loader.cpp 到 `app/CMakeLists.txt`
- [x] SceneLoader::load_meshes（遍历 glTF primitive，提取顶点，生成 tangent，创建 CudaBuffer，计算 AABB）
- [x] SceneLoader::load_materials（sampler 转换、纹理加载 + BC 压缩 + 缓存 + CUDA 上传、Material 填充 + default fallback、材质数组上传）。纹理级 OpenMP：`#pragma omp parallel for schedule(dynamic)` 并行调用 `compress_texture`（照搬 Himalaya `scene_loader.cpp` Phase 2c；单张纹理内部串行）
- [x] SceneLoader::build_mesh_instances（场景图遍历、transform 收集、scene AABB 计算）
- [x] 请求用户在 CLion 中编译验证

## Step 5：OptiX 加速结构

- [x] 创建 `optix/include/qualquer/optix/accel_structure.h` + `optix/src/accel_structure.cpp`（AccelStructure 类：build_blas / build_tlas / destroy_blas / destroy_tlas），并注册 accel_structure.cpp 到 `optix/CMakeLists.txt`
- [x] BLAS 构建：multi-geometry per group_id、PREFER_FAST_TRACE + ALLOW_COMPACTION、compaction
- [x] TLAS 构建：OptixInstance 组装（instanceId = group base offset、transform 转 3×4 row-major）、按 (group_id, transform) 去重
- [x] 请求用户在 CLion 中编译验证

## Step 6：LaunchParams + 设备程序 + Pipeline 更新

- [x] launch_params.h 新增 `float4x4` 类型定义 + 扩展 LaunchParams（traversable、geometry_infos、materials、texture_objects、inv_view、inv_projection）
MUSTREAD:4
- [x] 更新 Pipeline 参数：`numPayloadValues` = 3、`maxTraceDepth` = 1、stack size 更新
- [x] `__raygen__rg`：inverse matrix primary ray 生成 → `optixTrace` → payload 读取 → 写累积 buffer
- [x] `__closesthit__ch`：geometry info 查找 → 顶点插值 → 材质采样（全部 PBR 纹理）→ normal mapping → ambient 着色（baseColor × occlusion）→ payload 写入
- [x] `__miss__ms`：输出背景色
- [x] SBT 更新：HitGroupSbtRecord 新增 HitGroupData（geometry_infos、materials、texture_objects 指针）
- [x] 请求用户在 CLion 中编译验证

## Step 7：Renderer + Application 核心集成

- [x] Renderer 新增 `load_scene` 公开方法：接收 SceneLoader 公开接口（meshes、mesh_instances、material_buffer、texture_objects_buffer），构建 AS（按 group_id 分组 BLAS、按 (group_id, transform) 去重组装 OptixInstance、构建 TLAS），构建 GeometryInfo buffer（per-group 连续排列，与 instanceId 对应），重建 SBT。独立于 init，支持运行时场景切换
- [x] 修复：SBT 死数据清理——closesthit 读 LaunchParams（非 SBT），HitGroupSbtRecord 回归 header-only（SbtRecord），移除 HitGroupData 与 rebuild_hitgroup_sbt 及 load_scene 的 material/texture 参数（仅服务 SBT 重建），更新 current-phase.md SBT 设计节
- [x] Renderer::submit_cuda 更新：LaunchParams 填入相机矩阵 + TLAS handle + 数据指针
- [x] `vcpkg.json` 新增 nlohmann-json；创建 `app/include/qualquer/app/config.h` + `app/src/config.cpp`（AppConfig + load_config + save_config，`%LOCALAPPDATA%\qualquer\config.json`），注册到 `app/CMakeLists.txt`
- [x] Application init + destroy 扩展：Config 加载、DefaultTextures 创建与持有、Camera + CameraController 初始化（aspect 从 swapchain）、SceneLoader 加载（config.scene_path + default_textures）、Renderer load_scene、Camera 初始定位（auto_position_camera + set_focus_target）；destroy 同步扩展（DefaultTextures 销毁 + SceneLoader::destroy）
- [x] Application 帧循环：camera.aspect 从 swapchain 更新、CameraController::update（ImGui DeltaTime）→ submit_cuda（传入相机 + 场景数据指针）
- [x] 请求用户在 CLion 中编译验证（编译通过；空场景运行：不崩溃 / resize 正常 / 退出干净；场景 PBR 着色 + 交互相机浏览场景待配置 scene_path 端到端验证，顺延 Step 8）

## Step 8：场景切换 + 清理 + 最终验证

- [x] DebugUI 场景路径输入 + 加载按钮；Application switch_scene（GPU idle → destroy 旧场景 → 加载新场景 → load_scene → auto_position → save_config）
- [x] 请求用户在 CLion 中编译验证（场景切换正常）
- [x] 验证 ISPC 压缩运行时正确（BC7/BC5 via bc7enc、BC6H via ISPCTextureCompressor，确认 ISPC dispatch 与压缩产出正常；场景加载后端到端可验证）
- [x] 确认无 validation / OptiX / CUDA 报错
- [x] 确认纹理缓存生效（第二次加载从 KTX2 读取）
- [x] 确认 BLAS compaction 生效（日志对比 compaction 前后大小）
- [x] 确认 resize 正常（累积 buffer 重建，AS / 材质 / 纹理无影响）
- [ ] 代码文档检查（所有新增公开接口有 Doxygen 注释）
