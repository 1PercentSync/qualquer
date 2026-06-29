# 当前阶段

> 目标：场景加载 + 完整材质系统 + 加速结构 + Primary Ray（glTF，纹理 BC 压缩，PBR 材质，BLAS/TLAS，交互式相机，ambient 着色）
> 任务清单见 `tasks/phase3.md`。

---

## 实现步骤

### 依赖关系

```
Step 1 → Step 2 → Step 3 → Step 4 → Step 5 → Step 6 → Step 7 → Step 8
                     ↘
                    Step 3 可与 Step 4 并行（Camera 不依赖纹理/材质）
```

### 总览

| Step | 主题 | 验证标准 |
|------|------|----------|
| 1 | 第三方依赖集成 | 编译通过 |
| 2 | 数据结构 + Camera | 编译通过 |
| 3 | 纹理系统（CUDA BC 纹理） | 编译通过 |
| 4 | 材质系统 + 场景加载 | 编译通过，日志输出加载统计 |
| 5 | OptiX 加速结构 | 编译通过 |
| 6 | LaunchParams + 设备程序 + Pipeline 更新 | 编译通过 |
| 7 | Renderer + Application 集成 | 窗口显示场景 PBR ambient 着色 + ImGui，交互式相机可浏览，resize 不崩溃 |
| 8 | 清理 + 最终验证 | 无 validation / OptiX / CUDA 报错，场景正确渲染 |

---

## 技术要点

### 第三方依赖

| 库 | 来源 | 用途 |
|----|------|------|
| fastgltf | vcpkg | glTF 解析 |
| stb（stb_image, stb_image_resize2） | vcpkg | 图像解码、CPU mip 生成 |
| glm | vcpkg | 数学库（向量、矩阵、变换） |
| mikktspace | vcpkg | tangent 生成 |
| xxhash | vcpkg | 纹理缓存 content hash |
| bc7enc（bc7e.ispc + rgbcx） | third_party | BC7/BC5 纹理压缩 |

bc7enc 不在 vcpkg 中，从 Himalaya 的 `third_party/bc7enc/` 复制并适配（include 路径改为 `qualquer/bc7enc/`）。需要 CMake ISPC 语言支持（CMake 3.19+，项目已用 4.2）。

其余库通过 vcpkg manifest 管理。

### Vertex 格式

统一顶点格式，所有 mesh 共用。从 Himalaya 照搬，去掉 uv1（本项目不需要）和 Vulkan 特有方法。

```cpp
struct Vertex {
    glm::vec3 position;     //  0: 12 bytes
    glm::vec3 normal;       // 12: 12 bytes
    glm::vec2 uv0;          // 24:  8 bytes
    glm::vec4 tangent;      // 32: 16 bytes
};                          // total: 48 bytes
```

- tangent.w = handedness（MikkTSpace 约定）
- 缺失属性在加载时填默认值（normal → +Z，uv0 → 0，tangent → MikkTSpace 生成或 +X）
- host 和 device 共享同一定义（`.h` 被 `.cpp` 和 `.cu` 共同 include）

去掉 Himalaya 的 `binding_description()` / `attribute_descriptions()`（Vulkan vertex input 专用，Qualquer 不需要）。

### 场景数据结构

从 Himalaya 照搬，适配到 Qualquer 命名空间。均为纯数据头文件，放 renderer 层。

```cpp
// 轴对齐包围盒
struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

// GPU mesh 数据（一个 glTF primitive = 一个 Mesh）
struct Mesh {
    optix::CudaBuffer<Vertex> vertex_buffer;
    optix::CudaBuffer<uint32_t> index_buffer;
    uint32_t vertex_count = 0;
    uint32_t index_count = 0;
    uint32_t group_id = 0;      // glTF mesh index，BLAS 分组用
    uint32_t material_id = 0;   // 材质索引
};

// 场景中的一个 mesh 实例
struct MeshInstance {
    uint32_t mesh_id;           // Mesh 数组索引
    uint32_t material_id;       // 材质索引
    glm::mat4 transform{1.0f};  // 世界变换
    AABB world_bounds;          // 世界空间 AABB
};
```

**与 Himalaya 的差异**：
- `Mesh` 持有 `CudaBuffer`（CUDA 直接分配），而非 RHI `BufferHandle`（Vulkan buffer）
- `MeshInstance` 去掉 `prev_transform`（运动向量是 M2+ 的事）
- 由于 `Mesh` 持有 `CudaBuffer` 且 `CudaBuffer` 是 move-only RAII，`Mesh` 自身也是 move-only

### Camera

从 Himalaya 照搬 `Camera` struct（renderer 层）和 `CameraController` 类（app 层）。

**Camera**（`renderer/include/qualquer/renderer/camera.h`）：
- 输入状态：position、yaw、pitch、fov、near/far、aspect
- 派生状态：view、projection、inv_view、inv_projection（raygen primary ray 需要）
- 不含 `view_projection` / `inv_view_projection`（光栅化用，PT 不需要）
- `compute_focus_position(AABB)`：自动对焦场景包围盒
- 纯 glm 数学，无 Vulkan 依赖

**CameraController**（`app/include/qualquer/app/camera_controller.h`）：
- 右键拖拽旋转、WASD 移动、Space/Ctrl 升降、Shift 加速、F 对焦
- `init(GLFWwindow*, Camera*)` + `update(float delta_time)`
- 从 Himalaya 照搬，适配命名空间

### CUDA 纹理系统
MUSTREAD:8

CUDA 从 11.5 起原生支持 BC1–BC7 压缩纹理。纹理通过 `cudaMipmappedArray`（BC 格式）+ `cudaTextureObject_t` 创建，设备端用 `tex2D<float4>()` 采样，硬件自动解压。

**纹理处理流水线**（renderer 层，从 Himalaya 照搬压缩/缓存逻辑）：

```
stb_image 解码 → CPU mip 生成(stb_image_resize2) → BC 压缩(bc7enc/rgbcx) → KTX2 缓存
→ cudaMipmappedArray 创建 + 数据上传 → cudaTextureObject_t 创建
```

**TextureRole**（决定 BC 格式）：

| Role | BC 格式 | 用途 |
|------|---------|------|
| Color | BC7 SRGB | baseColor、emissive（gamma-correct） |
| Linear | BC7 UNORM | metallic-roughness、occlusion（线性数据） |
| Normal | BC5 UNORM | 法线贴图（RG 通道，Z shader 重建） |

**CudaTexture**（optix 层）：

管理单个 CUDA 纹理的 GPU 资源生命周期。

```cpp
struct CudaTexture {
    cudaMipmappedArray_t mipmap_array = nullptr;
    cudaTextureObject_t  texture_object = 0;

    void destroy();
};
```

创建流程：
1. `cudaMallocMipmappedArray(&mipmap_array, &channel_desc, extent, level_count)`
   - `channel_desc` 使用 BC 格式对应的 `cudaChannelFormatKind`（如 `cudaChannelFormatKindBC7_UNORM_SRGB`）
2. 逐 level `cudaGetMipmappedArrayLevel(&level_array, mipmap_array, i)` + `cudaMemcpy2DToArray` 上传压缩数据
3. `cudaCreateTextureObject(&texture_object, &res_desc, &tex_desc, nullptr)`
   - `res_desc.resType = cudaResourceTypeMipmappedArray`
   - `tex_desc.filterMode = cudaFilterModeLinear`（硬件双线性过滤）
   - `tex_desc.mipmapFilterMode = cudaFilterModeLinear`（trilinear）
   - `tex_desc.normalizedCoords = 1`
   - BC 格式时 `pResViewDesc` 传 `nullptr`

**纹理缓存**（renderer 层）：

从 Himalaya 照搬 cache 和 KTX2 模块。缓存路径改为 `%TEMP%\qualquer\textures\`。content hash 用 xxhash（XXH3_128），按源字节（JPEG/PNG 原始数据）计算，避免无意义的解码+重压缩。

**Default Textures**：

三个 1×1 纹理，R8G8B8A8_UNORM（不压缩，极值处 sRGB/linear 结果一致）：

| 名称 | RGBA 值 | 用途 |
|------|---------|------|
| white | (255,255,255,255) | 默认 baseColor / metallic-roughness / occlusion |
| flat_normal | (128,128,255,255) | 默认法线（Z-up，无扰动） |
| black | (0,0,0,255) | 默认 emissive（无发光） |

缺失纹理槽用对应 default 的 `cudaTextureObject_t` 填充，shader 无条件采样。

### 材质系统

**GPUMaterialData**（host + device 共享）：

从 Himalaya 直接照搬，80 字节，`alignas(16)`。CUDA 天然对齐兼容，结构体布局 host/device 一致。

```cpp
struct alignas(16) GPUMaterialData {
    glm::vec4 base_color_factor;        //  0
    glm::vec4 emissive_factor;          // 16
    float metallic_factor;              // 32
    float roughness_factor;             // 36
    float normal_scale;                 // 40
    float occlusion_strength;           // 44
    uint32_t base_color_tex;            // 48  — cudaTextureObject_t 数组索引
    uint32_t emissive_tex;              // 52
    uint32_t metallic_roughness_tex;    // 56
    uint32_t normal_tex;                // 60
    uint32_t occlusion_tex;             // 64
    float alpha_cutoff;                 // 68
    uint32_t alpha_mode;                // 72  — 0=Opaque, 1=Mask, 2=Blend
    uint32_t double_sided;              // 76
};                                      // 80 bytes
static_assert(sizeof(GPUMaterialData) == 80);
```

**纹理索引的含义**：Himalaya 中 `base_color_tex` 等字段是 Vulkan bindless descriptor array 的索引。Qualquer 中改为 `cudaTextureObject_t` 数组的索引——场景加载时收集所有 `cudaTextureObject_t` 到一个 `CudaBuffer<cudaTextureObject_t>`，上传到设备，shader 通过索引取得 texture object 再 `tex2D` 采样。

**MaterialInstance**（CPU 侧元数据）：

```cpp
struct MaterialInstance {
    uint32_t template_id;       // 着色模型（M1 固定 0 = standard PBR）
    uint32_t buffer_offset;     // GPUMaterialData 数组索引
    AlphaMode alpha_mode;       // pass 路由（Phase 3 全部视为 Opaque）
    bool double_sided;          // 双面渲染
};
```

### GPUGeometryInfo

per-geometry RT 查询数据，从 Himalaya 照搬。closest hit shader 通过 SBT record 中的 geometry info buffer 指针 + `optixGetInstanceId() + optixGetPrimitiveIndex()` 对应的几何索引来查找。

```cpp
struct GPUGeometryInfo {
    CUdeviceptr vertex_buffer_address;   //  0: 顶点 buffer 设备指针
    CUdeviceptr index_buffer_address;    //  8: 索引 buffer 设备指针
    uint32_t material_buffer_offset;     // 16: GPUMaterialData 数组索引
    uint32_t _padding;                   // 20
};                                       // 24 bytes
static_assert(sizeof(GPUGeometryInfo) == 24);
```

### 场景加载（SceneLoader）

app 层，从 Himalaya 照搬加载逻辑，适配 CUDA 资源。

**类职责**：解析 glTF → 创建 GPU 资源（CUDA buffer、纹理）→ 构建 MeshInstance 列表。持有所有加载资源的生命周期。

**加载流程**（与 Himalaya 一致的三阶段）：

1. **load_meshes**：遍历 glTF mesh/primitive，提取顶点属性（position、normal、uv0、tangent），生成缺失 tangent（MikkTSpace），创建 `CudaBuffer<Vertex>` 和 `CudaBuffer<uint32_t>`，计算 local AABB
2. **load_materials**：加载 sampler → 加载纹理（hash → cache check → decode → BC compress → CUDA upload）→ 填充 `GPUMaterialData`（含 default texture fallback）→ 上传材质数组到 `CudaBuffer<GPUMaterialData>`
3. **build_mesh_instances**：遍历场景图，收集 node transform × primitive → `MeshInstance`，计算 scene AABB

**与 Himalaya 的主要差异**：

| 方面 | Himalaya | Qualquer |
|------|----------|---------|
| 顶点/索引 buffer | Vulkan RHI buffer（VMA） | `CudaBuffer<T>`（`cudaMalloc`） |
| 纹理 | VkImage + bindless descriptor | `cudaMipmappedArray` + `cudaTextureObject_t` |
| 材质 SSBO | Vulkan SSBO（descriptor binding） | `CudaBuffer<GPUMaterialData>` |
| RT buffer flags | `ShaderDeviceAddress` + `AccelStructBuildInput` | 不需要（OptiX 直接用 CUDA 指针） |
| 纹理并行压缩 | OpenMP `#pragma omp parallel for` | 照搬（MSVC 支持 OpenMP） |

### OptiX 加速结构

optix 层新增 `AccelStructure` 类，封装 `optixAccelBuild`。

**BLAS 构建**：

```cpp
struct BLASGeometry {
    CUdeviceptr vertex_buffer;      // 顶点 buffer 设备指针
    CUdeviceptr index_buffer;       // 索引 buffer 设备指针
    uint32_t vertex_count;
    uint32_t index_count;
    uint32_t vertex_stride;         // sizeof(Vertex)
};

struct BLASHandle {
    OptixTraversableHandle handle = 0;
    CudaBuffer<uint8_t> buffer;     // AS backing storage
};
```

- 输入：`OptixBuildInput` type = `OPTIX_BUILD_INPUT_TYPE_TRIANGLES`
- vertex format = `OPTIX_VERTEX_FORMAT_FLOAT3`，stride = `sizeof(Vertex)`（position 在 offset 0）
- index format = `OPTIX_INDICES_FORMAT_UNSIGNED_INT3`
- flags = `OPTIX_GEOMETRY_FLAG_NONE`（Phase 3 全部视为 opaque，但不设 `DISABLE_ANYHIT`——保留未来 alpha 接入空间；也不设 `OPAQUE` flag 因为 Pipeline 级 `OPTIX_EXCEPTION_FLAG_NONE` + 不注册 anyhit 等效于 opaque 行为）
- build flags = `OPTIX_BUILD_FLAG_PREFER_FAST_TRACE | OPTIX_BUILD_FLAG_ALLOW_COMPACTION`
- 每个 glTF mesh（多个 primitive）构建一个 multi-geometry BLAS（按 `group_id` 分组，与 Himalaya 一致）

**TLAS 构建**：

```cpp
struct TLASHandle {
    OptixTraversableHandle handle = 0;
    CudaBuffer<uint8_t> buffer;
    CudaBuffer<OptixInstance> instance_buffer;
};
```

- `OptixInstance` 包含：transform（3×4 row-major）、instanceId（= group 在 GeometryInfo 数组中的起始偏移）、sbtOffset（= 0，单 hit group）、mask（0xFF）、flags（`OPTIX_INSTANCE_FLAG_NONE`）
- 与 Himalaya 的 `instanceCustomIndex` 概念一致，OptiX 用 `optixGetInstanceId()` 获取
- 按 (group_id, transform) 去重（与 Himalaya 逻辑一致）

**AS Compaction**：

BLAS 构建后执行 compaction（查询压缩后大小 → 分配新 buffer → `optixAccelCompact` → 释放原 buffer）。Compaction 通常节省 ~50% AS 内存，对大场景显著。TLAS 不做 compaction（instance 数量少，收益不值得额外 pass）。

### LaunchParams 扩展

```cpp
struct LaunchParams {
    // Phase 2 保留
    float4 *accumulation_buffer;
    uint32_t width;
    uint32_t height;
    uint32_t frame_index;

    // Phase 3 新增
    OptixTraversableHandle traversable;          // TLAS handle
    const GPUGeometryInfo *geometry_infos;        // geometry info 数组
    const GPUMaterialData *materials;             // 材质数组
    const cudaTextureObject_t *texture_objects;   // 纹理 object 数组

    // 相机（inv_view + inv_projection 用于 primary ray 生成）
    float4x4 inv_view;
    float4x4 inv_projection;
};
```

**float4x4 类型**：CUDA 无内置 4×4 矩阵类型。定义简单的 `struct float4x4 { float4 rows[4]; }` 在 launch_params.h 中（host 侧从 `glm::mat4` 转换填入，device 侧直接读取运算）。

### Pipeline 参数更新

| 参数 | Phase 2 | Phase 3 |
|------|---------|---------|
| `numPayloadValues` | 0 | 6（见下方 payload 定义） |
| `numAttributeValues` | 2 | 2（三角形重心坐标，不变） |
| `maxTraceDepth`（link options） | 0 | 1（primary ray only） |
| `pipelineLaunchParamsSizeInBytes` | `sizeof(LaunchParams)`（旧） | `sizeof(LaunchParams)`（新，更大） |

**Payload 寄存器**（32-bit × 6）：

Phase 3 primary ray 不需要 bounce，payload 用于 closest hit → raygen 回传着色结果：

| 寄存器 | 用途 |
|--------|------|
| p0-p2 | shading color（float3，RGBA 的 RGB） |
| p3 | hit distance（float，miss 时 -1） |
| p4-p5 | 预留（为 Phase 5 多 bounce 的 throughput、next direction 等做空间准备） |

### 设备程序更新

**`__raygen__rg`**：
- 从 `LaunchParams` 读取 `inv_view` / `inv_projection`
- 计算像素中心 NDC → clip → view（inv_projection）→ world ray（inv_view）
- 无 subpixel jitter（Phase 5 加采样时添加）
- `optixTrace(params.traversable, origin, direction, 0.0f, 1e16f, ...)`
- 从 payload 读取 shading color，写入累积 buffer
- primary ray 每帧覆盖写入（`frame_index == 0` 时清零累积；后续 Phase 加采样后改为增量平均）

**`__closesthit__ch`**：
- 通过 `optixGetInstanceId() + optixGetSbtGASIndex()` 查 `GPUGeometryInfo`（注：OptiX 9.1 中 `optixGetSbtGASIndex()` 返回 geometry index within GAS，等价于 Vulkan 的 `gl_GeometryIndexEXT`）
- 从 geometry info 获取 vertex/index buffer 指针，通过 `optixGetPrimitiveIndex()` 和重心坐标插值顶点属性
- 采样全部 PBR 纹理（baseColor、metallic-roughness、normal、emissive、occlusion）
- Normal mapping：TBN 矩阵构建 + normal map 采样 + 世界空间变换
- **Ambient 着色**：不做光照计算，输出 = `baseColor.rgb * occlusion`（纯 ambient，无光源、无 BRDF 计算）
- 结果写入 payload 寄存器

**`__miss__ms`**：
- 输出背景色（固定颜色，如深灰），hit_distance = -1 信号终止
- Phase 6 接入 IBL 后改为环境贴图采样

**`__anyhit__ah`**：
- Phase 3 保持空实现（全部视为 opaque）

### SBT 变化

Phase 2 的 SBT record 只有 header（32 字节），无用户数据。Phase 3 的 hitgroup record 需要携带数据指针：

```cpp
struct HitGroupData {
    const GPUGeometryInfo *geometry_infos;
    const GPUMaterialData *materials;
    const cudaTextureObject_t *texture_objects;
};

struct HitGroupSbtRecord {
    alignas(OPTIX_SBT_RECORD_ALIGNMENT) char header[OPTIX_SBT_RECORD_HEADER_SIZE];
    HitGroupData data;
};
```

raygen 和 miss record 仍保持 header-only（这些程序通过 `LaunchParams` 访问全局数据）。

**替代方案考量**：这些指针在 `LaunchParams` 中已有，hitgroup record 里再放一份看似冗余。但 SBT data 是 OptiX 中 per-geometry 数据传递的标准方式（未来多 hit group 时每组可以有不同数据），现在统一放入 SBT 符合 OptiX 惯例，且 closest hit shader 读 SBT data 比读 launch params 更直接（OptiX 内部可能有优化路径）。保持这个设计。

### Renderer 变化

- **init 扩展**：接收场景数据（Mesh 列表、材质、纹理、MeshInstance），构建 AS，上传 geometry info / 材质 / 纹理 object 数组，重建 SBT
- **submit_cuda 更新**：LaunchParams 填入相机矩阵 + TLAS handle + 数据指针，`optixLaunch` 的 traversable 从 0 变为 TLAS handle
- **resize 扩展**：累积 buffer 重建（不变），AS / 材质 / 纹理不受 resize 影响

### Application 变化

- **新增成员**：`Camera`、`CameraController`、`SceneLoader`
- **init 流程扩展**：Camera 初始化 → SceneLoader 加载场景 → Renderer init 接收场景数据
- **帧循环**：`CameraController::update(delta_time)` → Camera matrices 更新 → submit_cuda（传入相机数据）
- **delta time**：GLFW `glfwGetTime()` 计算帧间隔
- **场景路径**：暂时硬编码或通过命令行参数传入

### 文件结构变化

```
新增:
  # 第三方
  third_party/bc7enc/CMakeLists.txt
  third_party/bc7enc/include/qualquer/bc7enc/rgbcx.h
  third_party/bc7enc/include/qualquer/bc7enc/rgbcx_table4.h
  third_party/bc7enc/include/qualquer/bc7enc/rgbcx_table4_small.h
  third_party/bc7enc/src/rgbcx.cpp
  third_party/bc7enc/src/bc7e.ispc

  # optix 层
  optix/include/qualquer/optix/accel_structure.h
  optix/src/accel_structure.cpp
  optix/include/qualquer/optix/cuda_texture.h

  # renderer 层
  renderer/include/qualquer/renderer/vertex.h
  renderer/include/qualquer/renderer/scene_types.h
  renderer/include/qualquer/renderer/material.h
  renderer/include/qualquer/renderer/camera.h
  renderer/src/camera.cpp
  renderer/include/qualquer/renderer/texture.h
  renderer/src/texture.cpp
  renderer/include/qualquer/renderer/cache.h
  renderer/src/cache.cpp
  renderer/include/qualquer/renderer/ktx2.h
  renderer/src/ktx2.cpp
  renderer/src/mesh.cpp

  # app 层
  app/include/qualquer/app/scene_loader.h
  app/src/scene_loader.cpp
  app/include/qualquer/app/camera_controller.h
  app/src/camera_controller.cpp
  app/include/qualquer/app/gltf_utils.h
  app/src/gltf_utils.cpp

修改:
  vcpkg.json
  CMakeLists.txt
  optix/CMakeLists.txt
  renderer/CMakeLists.txt
  app/CMakeLists.txt
  renderer/include/qualquer/renderer/launch_params.h
  renderer/include/qualquer/renderer/renderer.h
  renderer/src/renderer.cpp
  renderer/src/device/programs.cu
  app/include/qualquer/app/application.h
  app/src/application.cpp
```

---

## Himalaya 参考对照

Phase 3 的多数模块直接从 Himalaya 照搬后适配。下表总结每个模块的参考源和适配程度。

| Qualquer 模块 | Himalaya 源文件 | 适配程度 |
|---------------|----------------|---------|
| Vertex | `framework/mesh.h` | 去 uv1、去 Vulkan 方法 |
| Mesh / MeshInstance / AABB | `framework/scene_data.h`, `framework/mesh.h` | buffer 类型改 CudaBuffer |
| Camera | `framework/camera.h/.cpp` | 去 view_projection，加 inv_view/inv_projection |
| CameraController | `app/camera_controller.h/.cpp` | 命名空间适配 |
| 纹理处理（mip/BC/cache） | `framework/texture.h/.cpp` | GPU 上传从 Vulkan 改 CUDA |
| KTX2 缓存 | `framework/ktx2.h/.cpp` | 格式枚举适配 |
| content hash | `framework/cache.h/.cpp` | 缓存路径改 qualquer |
| GPUMaterialData | `framework/material_system.h` | 直接照搬 |
| MaterialInstance | `framework/material_system.h` | 直接照搬 |
| Default textures | `framework/texture.h/.cpp` | CUDA 纹理创建替代 Vulkan |
| SceneLoader | `app/scene_loader.h/.cpp` | buffer/纹理创建改 CUDA |
| gltf_utils | `app/gltf_utils.h/.cpp` | 命名空间适配 |
| GPUGeometryInfo | `framework/scene_data.h` | address 类型改 CUdeviceptr |
| AS 构建逻辑 | `framework/scene_as_builder.cpp` | API 从 Vulkan RT 改 OptiX |
| Primary ray | `shaders/rt/reference_view.rgen` | GLSL → CUDA，去 jitter/accumulation |
| Closest hit | `shaders/rt/closesthit.rchit` | GLSL → CUDA，只保留材质采样，去光照 |
| MikkTSpace tangent | `framework/mesh.cpp` | 直接照搬 |

---

## 完成标准

- glTF 场景成功加载（mesh + 材质 + BC 压缩纹理 + mip + 缓存）
- BLAS/TLAS 正确构建（compaction 生效）
- Primary ray 正确命中几何体
- 全部 PBR 纹理正确采样（baseColor、metallic-roughness、normal map、emissive、occlusion）
- Normal mapping 正确（TBN 矩阵 + tangent space → world space）
- Ambient 着色输出正确（baseColor × occlusion）
- 交互式相机可浏览场景（旋转/移动/对焦）
- 窗口显示场景 + ImGui 面板
- Resize 正常（累积 buffer 重建，AS / 材质 / 纹理不受影响）
- 无 validation / OptiX / CUDA 报错
