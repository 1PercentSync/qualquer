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
- [ ] 创建 `renderer/include/qualquer/renderer/cache.h` + `renderer/src/cache.cpp`（cache_root、content_hash、cache_path、atomic_write_file），并注册 cache.cpp 到 `renderer/CMakeLists.txt`（链接 xxhash）
- [ ] 创建 `renderer/include/qualquer/renderer/ktx2.h` + `renderer/src/ktx2.cpp`（Ktx2Data、read_ktx2、write_ktx2），并注册 ktx2.cpp 到 `renderer/CMakeLists.txt`
- [ ] 创建 `renderer/include/qualquer/renderer/texture.h` + `renderer/src/texture.cpp`（TextureRole、ImageData、load_image、load_image_from_memory、ensure_bc_init、generate_cpu_mip_chain、compress_bc7/bc5、load_cached_texture、compress_texture），并注册 texture.cpp 到 `renderer/CMakeLists.txt`（链接 stb、bc7enc）
- [ ] 纹理 GPU 上传：在 texture.cpp 中实现 `finalize_texture`（`cudaMallocMipmappedArray` + 逐 level 上传 + `cudaCreateTextureObject`）
- [ ] Default textures：实现 `create_default_textures`（1×1 white/flat_normal/black，R8G8B8A8 非压缩 CUDA 纹理）
- [ ] 请求用户在 CLion 中编译验证
- [ ] 验证 bc7enc/ISPC 运行时正确（Step 1 仅编译通过，此处首次实际调用 BC 压缩，确认 ISPC dispatch 与压缩产出正常）
- [ ] 检查 bc7enc 使用是否最优（压缩 preset/参数、并行策略等）

## Step 4：材质系统 + 场景加载

- [ ] 创建 `app/include/qualquer/app/gltf_utils.h` + `app/src/gltf_utils.cpp`（parse_gltf、transform_aabb），并注册 gltf_utils.cpp 到 `app/CMakeLists.txt`（链接 fastgltf）
- [ ] 创建 `renderer/src/mesh.cpp`（generate_tangents — MikkTSpace），并注册 mesh.cpp 到 `renderer/CMakeLists.txt`（链接 mikktspace）
- [ ] 创建 `app/include/qualquer/app/scene_loader.h` + `app/src/scene_loader.cpp`（SceneLoader：load / destroy / meshes / mesh_instances / scene_bounds 等），并注册 scene_loader.cpp 到 `app/CMakeLists.txt`
- [ ] SceneLoader::load_meshes（遍历 glTF primitive，提取顶点，生成 tangent，创建 CudaBuffer，计算 AABB）
- [ ] SceneLoader::load_materials（sampler 转换、纹理加载 + BC 压缩 + 缓存 + CUDA 上传、Material 填充 + default fallback、材质数组上传）
- [ ] SceneLoader::build_mesh_instances（场景图遍历、transform 收集、scene AABB 计算）
- [ ] 请求用户在 CLion 中编译验证

## Step 5：OptiX 加速结构

- [ ] 创建 `optix/include/qualquer/optix/accel_structure.h` + `optix/src/accel_structure.cpp`（AccelStructure 类：build_blas / build_tlas / destroy_blas / destroy_tlas），并注册 accel_structure.cpp 到 `optix/CMakeLists.txt`
- [ ] BLAS 构建：multi-geometry per group_id、PREFER_FAST_TRACE + ALLOW_COMPACTION、compaction
- [ ] TLAS 构建：OptixInstance 组装（instanceId = group base offset、transform 转 3×4 row-major）、按 (group_id, transform) 去重
- [ ] GeometryInfo buffer 构建（per-group 连续排列，与 instanceId 对应）
- [ ] 请求用户在 CLion 中编译验证

## Step 6：LaunchParams + 设备程序 + Pipeline 更新

- [ ] launch_params.h 新增 `float4x4` 类型定义 + 扩展 LaunchParams（traversable、geometry_infos、materials、texture_objects、inv_view、inv_projection）
MUSTREAD:4
- [ ] 更新 Pipeline 参数：`numPayloadValues` = 6、`maxTraceDepth` = 1、stack size 更新
- [ ] `__raygen__rg`：inverse matrix primary ray 生成 → `optixTrace` → payload 读取 → 写累积 buffer
- [ ] `__closesthit__ch`：geometry info 查找 → 顶点插值 → 材质采样（全部 PBR 纹理）→ normal mapping → ambient 着色（baseColor × occlusion）→ payload 写入
- [ ] `__miss__ms`：输出背景色 + hit_distance = -1
- [ ] SBT 更新：HitGroupSbtRecord 新增 HitGroupData（geometry_infos、materials、texture_objects 指针）
- [ ] 请求用户在 CLion 中编译验证

## Step 7：Renderer + Application 集成

- [ ] Renderer::init 扩展：接收场景数据，构建 AS，上传 geometry info / 材质 / 纹理 object 数组，重建 SBT
- [ ] Renderer::submit_cuda 更新：LaunchParams 填入相机矩阵 + TLAS handle + 数据指针
- [ ] Application 新增：Camera + CameraController 初始化、SceneLoader 加载、delta time 计算
- [ ] Application 帧循环：CameraController::update → Camera 矩阵更新 → submit_cuda
- [ ] Application destroy：SceneLoader::destroy
- [ ] 请求用户在 CLion 中编译验证（窗口显示场景 PBR ambient 着色 + ImGui，交互式相机可浏览，resize 不崩溃）

## Step 8：清理 + 最终验证

- [ ] 确认无 validation / OptiX / CUDA 报错
- [ ] 确认纹理缓存生效（第二次加载从 KTX2 读取）
- [ ] 确认 BLAS compaction 生效（日志对比 compaction 前后大小）
- [ ] 确认 resize 正常（累积 buffer 重建，AS / 材质 / 纹理无影响）
- [ ] 代码文档检查（所有新增公开接口有 Doxygen 注释）
- [ ] 请求用户做最终编译 + 运行验证
