# 代码阅读进度

> 按架构依赖方向（底层→上层），每层内按数据定义→工具→核心逻辑排列。
>
> 核心路径（★）：LaunchParams → RNG → BRDF → NEE → tonemap → megakernel → Renderer → Application

---

## 1. optix 层（CUDA/OptiX 封装）

- [x] `optix/include/.../cuda_check.h` — CUDA 错误处理宏
- [x] `optix/include/.../cuda_check_kernel.h` — device 侧错误检查
- [x] `optix/include/.../optix_check.h` — OptiX 错误处理宏
- [x] `optix/include/.../cuda_buffer.h` — 基础 CUDA 内存封装
- [x] `optix/include/.../cuda_texture.h` — 纹理与 mipmapped array 封装
- [x] `optix/include/.../cuda_array_buffer.h` — cudaArray 封装
- [x] `optix/include/.../cuda_texture_upload.h` + `optix/src/cuda_texture_upload.cpp` — 纹理上传
- [x] `optix/include/.../context.h` + `optix/src/context.cpp` — CUDA/OptiX context
- [x] `optix/include/.../pipeline.h` + `optix/src/pipeline.cpp` — Module/Pipeline/SBT
- [ ] `optix/include/.../accel_structure.h` + `optix/src/accel_structure.cpp` — BLAS/TLAS
- [ ] `optix/include/.../dlss_rr.h` + `optix/src/dlss_rr.cpp` — DLSS-RR 封装
- [x] `optix/src/optix_stubs_impl.cpp` — OptiX function table 加载

## 2. vulkan 层（Swapchain 呈现 + Interop）

- [ ] `vulkan/include/.../context.h` + `vulkan/src/context.cpp` — Instance/Device/Queue
- [ ] `vulkan/include/.../swapchain.h` + `vulkan/src/swapchain.cpp` — Swapchain 管理
- [ ] `vulkan/include/.../interop.h` + `vulkan/src/interop.cpp` — CUDA-Vulkan interop
- [ ] `vulkan/include/.../imgui_backend.h` + `vulkan/src/imgui_backend.cpp` — ImGui Vulkan 后端
- [ ] `vulkan/src/vma_impl.cpp` — VMA 编译单元

## 3. renderer 层 — 数据定义

- [ ] `renderer/include/.../vertex.h` — 统一顶点格式
- [ ] `renderer/include/.../material.h` — GPU 材质结构体
- [ ] `renderer/include/.../scene_types.h` — 场景数据结构
- [ ] `renderer/include/.../scene_stats.h` — 只读资产统计
- [ ] `renderer/include/.../render_settings.h` — 可调渲染参数
- [ ] ★ `renderer/include/.../launch_params.h` — host↔device 桥梁

## 4. renderer 层 — Device 代码（PT 内核）

- [ ] `renderer/include/.../math_utils.cuh` — device 数学工具
- [ ] `renderer/include/.../sobol_direction_data.h` — Sobol 方向数表（数据，快速扫过）
- [ ] ★ `renderer/include/.../rng.cuh` — Sobol + CP rotation + xxhash fallback
- [ ] `renderer/include/.../payload_helpers.cuh` — payload register 读写/打包
- [ ] `renderer/include/.../pt_common.cuh` — ray offset、法线处理
- [ ] ★ `renderer/include/.../brdf.cuh` — EON diffuse + GGX specular + Turquin 补偿
- [ ] ★ `renderer/include/.../nee.cuh` — env + emissive NEE、MIS
- [ ] ★ `renderer/include/.../tonemap.cuh` — tonemap kernel + Catmull-Rom 缩放
- [ ] ★ `renderer/src/device/programs.cu` — megakernel：raygen/closesthit/miss/anyhit

## 5. renderer 层 — Host 代码

- [ ] `renderer/include/.../camera.h` + `renderer/src/camera.cpp` — 相机
- [ ] `renderer/include/.../tonemap.h` + `renderer/src/tonemap.cu` — tonemap host 调度
- [ ] `renderer/include/.../debug_ui.h` + `renderer/src/debug_ui.cpp` — ImGui 面板
- [ ] ★ `renderer/include/.../renderer.h` + `renderer/src/renderer.cpp` — Renderer 本体

## 6. app 层（场景加载 + 帧循环）

- [ ] `app/src/stb_impl.cpp` — STB 编译单元
- [ ] `app/include/.../cache.h` + `app/src/cache.cpp` — 磁盘纹理缓存
- [ ] `app/include/.../texture.h` + `app/src/texture.cpp` — 图像解码 + BC 压缩 + mip
- [ ] `app/include/.../ktx2.h` + `app/src/ktx2.cpp` — KTX2 容器读写
- [ ] `app/include/.../mesh.h` + `app/src/mesh.cpp` — mesh 处理
- [ ] `app/include/.../env_alias_table.h` + `app/src/env_alias_table.cpp` — 环境光 alias table
- [ ] `app/include/.../emissive_alias_table.h` + `app/src/emissive_alias_table.cpp` — 发光三角形 alias table
- [ ] `app/include/.../equirect_to_cubemap.h` + `app/src/equirect_to_cubemap.cu` — equirect→cubemap kernel
- [ ] `app/include/.../scene_loader.h` + `app/src/scene_loader.cpp` — glTF 场景加载
- [ ] `app/include/.../config.h` + `app/src/config.cpp` — JSON 配置
- [ ] `app/include/.../camera_controller.h` + `app/src/camera_controller.cpp` — 相机控制
- [ ] ★ `app/include/.../application.h` + `app/src/application.cpp` — 帧循环编排
- [ ] `app/src/main.cpp` — 入口

## 7. 构建配置

- [ ] 顶层 `CMakeLists.txt` + 各层 `CMakeLists.txt`
