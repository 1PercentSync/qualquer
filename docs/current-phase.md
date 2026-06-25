# 当前阶段

> 目标：OptiX Pipeline 框架（Pipeline/SBT，raygen 输出纯色，累积 buffer ping-pong，替换测试 kernel）
> 任务清单见 `tasks/phase2.md`。

---

## 实现步骤

### 依赖关系

```
Step 1 → Step 2 → Step 3 → Step 4 → Step 5 → Step 6 → Step 7 → Step 8
```

### 总览

| Step | 主题 | 验证标准 |
|------|------|----------|
| 1 | OptiX SDK 集成 + DeviceContext | 编译通过，日志输出 OptiX 版本 |
| 2 | CudaBuffer\<T\> | 编译通过 |
| 3 | OptiX IR 编译与加载基础设施 | 编译通过，最小设备程序编译为 .optixir 文件 |
| 4 | LaunchParams + 设备程序 | 编译通过，所有设备程序编译为 OptiX IR |
| 5 | Pipeline 类 | 编译通过，Pipeline 创建成功（日志输出） |
| 6 | 累积 buffer + Tone mapping | 编译通过 |
| 7 | Renderer 状态化重构 | 编译通过 |
| 8 | optixLaunch + 帧循环集成 + 清理 | 窗口显示纯色 + ImGui，resize 不崩溃 |

---

## 技术要点

### OptiX SDK

- OptiX 9.1.0，header-only SDK（无需链接库，运行时通过 `optixInit()` 从驱动加载符号）
- CMake 通过 `OptiX_INSTALL_DIR` 环境变量或 CMake 变量定位 include 目录
- `optix::Context` 新增 `OptixDeviceContext`（从 CUDA primary context 创建）和日志回调
- `OPTIX_CHECK` 宏（与 `CUDA_CHECK` / `VK_CHECK` 同构：失败 → spdlog::critical + abort）

### CudaBuffer\<T\>

optix 层 header-only 模板（`optix/include/qualquer/optix/cuda_buffer.h`）。RAII + move-only。

接口：`alloc(count)` / `free()` / `resize(count)` / `upload(src, count, stream)` / `clear(stream)` / `data()` → `T*` / `device_ptr()` → `CUdeviceptr` / `count()` / `size_bytes()`

用途：累积 buffer、SBT records、LaunchParams device copy、未来的顶点/索引/材质 buffer。

### OptiX IR 编译与加载

- **格式**：OptiX IR（nvcc `--optix-ir`），不用 PTX——OptiX 编译器可做更深层优化
- **加载方式**：构建期编译为 .optixir 文件并部署到运行目录，运行时读文件传入 `optixModuleCreate`。不嵌入可执行文件（理由见 `technical-decisions.md`）
- **CMake**：`cmake/CompileOptiXIR.cmake` 提供 `compile_optix_ir(target file1.cu ...)` 函数。内部流程：nvcc `--optix-ir` → .optixir → POST_BUILD 复制到 `$<TARGET_FILE_DIR:target>/shaders/`
- 编译标志：`--optix-ir --gpu-architecture=compute_89 -std=c++20 -I${OptiX_INCLUDE}`。依据（CUDA 13.2 nvcc 文档）：`--gpu-architecture` 正式情况下必须是 virtual 架构（compute_89），OptiX IR 需 virtual 供运行时 JIT；`-std=c++20` 显式指定，因 nvcc 默认 C++ dialect 跟随 host compiler 不确定；`--machine=64`/`--ccbin` 经查官方默认已满足，不加
- 部署参考 himalaya 的 POST_BUILD copy 模式（himalaya 复制 shader 源文件，Qualquer 复制编译产物 .optixir）

### 设备程序

单文件 `renderer/src/device/programs.cu`，包含所有 OptiX 入口点。编译为 OptiX IR（非常规 CUDA 对象），运行时由 `optixModuleCreate` 加载。

| 入口 | Phase 2 行为 |
|------|-------------|
| `__raygen__rg` | 计算像素坐标，写纯色到累积 buffer |
| `__miss__ms` | 空实现（payload 设背景色，Phase 3 后实际使用） |
| `__closesthit__ch` | 空实现（Phase 3 接入几何体后填充） |
| `__anyhit__ah` | 空实现（Phase 4 alpha mask 使用） |

### LaunchParams

renderer 层共享头文件（`renderer/include/qualquer/renderer/launch_params.h`），host `.cpp` 和 device `.cu` 都 include。

Phase 2 内容：累积 buffer 写入指针（`float4*`）、画面宽高、帧号。后续 Phase 逐步扩展（相机、AS handle、材质等）。

每帧通过 `cudaMemcpyAsync` 上传到 `CudaBuffer<LaunchParams>`（小结构体，开销可忽略）。

MUSTREAD:8
### Pipeline（optix 层）

`optix::Pipeline` 类：持有 OptixModule + OptixProgramGroup[] + OptixPipeline handle。init 接收 OptixDeviceContext + .optixir 文件路径，内部读文件创建 Module。

- **ProgramGroup** 作为公开成员暴露——renderer 需要它们来 pack SBT records
- **PipelineCompileOptions**：`numPayloadValues`、`numAttributeValues=2`（三角形重心坐标）、`traversableGraphFlags`、`pipelineLaunchParamsSizeInBytes=sizeof(LaunchParams)`（9.1 可选，下个大版本强制，现在就设好）
- **PipelineLinkOptions**：`maxTraceDepth=1`（Phase 2 raygen 不调 optixTrace，但 Pipeline 要求 ≥1）
- **Stack size**：使用 `optixPipelineSetStackSizeFromCallDepths`（传入 trace depth / callable depth 等语义参数，内部自动计算 stack size，比手动 `optixPipelineSetStackSize` 更不易出错）

### SBT（renderer 层）

renderer 用 Pipeline 暴露的 ProgramGroup handle 构建 SBT records。OptiX API `optixSbtRecordPackHeader` 出现在 renderer 层——与 Vulkan/CUDA API 出现在任意层同理（项目原则：不为隔离原始类型做无收益 wrapper）。

- Phase 2 record 只含 header（32 字节 opaque data），无用户数据
- 三块 `CudaBuffer`：raygen（1 record）、miss（1 record）、hit（1 record）
- 上传后填充 `OptixShaderBindingTable` 结构传入 `optixLaunch`

### 累积 Buffer

- 2 × `CudaBuffer<float4>`，尺寸 = width × height
- Ping-pong：帧 N 读 A 写 B，帧 N+1 读 B 写 A（与 `technical-decisions.md` 一致）
- Phase 2 raygen 只写不读（无真正累积），但 ping-pong 机制就位
- 跟随 swapchain resize 重建并 clear

### Tone Mapping

独立 CUDA kernel（`renderer/src/tonemap.cu`），非 OptiX 设备程序。

读当前帧累积 buffer（`float4`）→ 写 display surface（`cudaSurfaceObject_t`，R8G8B8A8_UNORM）。Phase 2 采用简单 clamp（值域截断到 [0,1]），后续替换。

执行时序：optixLaunch 之后、semaphore signal 之前（同一 stream）。

### Renderer 变化

- **新增 init / destroy / resize**：创建/销毁 Pipeline、SBT、累积 buffer、LaunchParams buffer；resize 重建累积 buffer
- **内部状态**：Pipeline、SBT buffers、累积 buffers、params buffer、ping-pong 索引、frame_counter_
- **submit_cuda 流程**：LaunchParams 上传 → `optixLaunch`（traversable=0，无 AS）→ tone map → signal
- **record_vulkan**：不变（blit + ImGui）
- **optixLaunch 的 traversable=0**：Phase 2 无加速结构，raygen 不调 `optixTrace`，合法

### 文件结构变化

```
新增:
  cmake/CompileOptiXIR.cmake
  optix/include/qualquer/optix/cuda_buffer.h
  optix/include/qualquer/optix/optix_check.h
  optix/include/qualquer/optix/pipeline.h
  optix/src/pipeline.cpp
  renderer/include/qualquer/renderer/launch_params.h
  renderer/include/qualquer/renderer/tonemap.h
  renderer/src/tonemap.cu
  renderer/src/device/programs.cu

修改:
  optix/CMakeLists.txt
  optix/include/qualquer/optix/context.h
  optix/src/context.cpp
  renderer/CMakeLists.txt
  renderer/include/qualquer/renderer/renderer.h
  renderer/src/renderer.cpp

删除:
  renderer/include/qualquer/renderer/test_kernel.h
  renderer/src/test_kernel.cu
```

---

## OptiX 9.1 API 参考

> Phase 2 涉及的 API 函数和结构体速查。来源：[OptiX 9.1 API Reference](https://raytracing-docs.nvidia.com/optix9/api/OptiX_API_Reference.pdf)、[Programming Guide](https://raytracing-docs.nvidia.com/optix9/guide/optix_guide.251118.A4.pdf)。

### 初始化

```cpp
// 加载 OptiX 库并初始化函数表（inline，定义在 optix_stubs.h）
OptixResult optixInit(void);

// 创建设备上下文。fromContext=0 使用当前 CUDA context
OptixResult optixDeviceContextCreate(
    CUcontext fromContext,
    const OptixDeviceContextOptions* options,
    OptixDeviceContext* context);

OptixResult optixDeviceContextDestroy(OptixDeviceContext context);
```

```cpp
struct OptixDeviceContextOptions {
    OptixLogCallback logCallbackFunction;  // 日志回调函数指针
    void*            logCallbackData;      // 传给回调的用户数据
    int              logCallbackLevel;     // 最大日志级别（1-4）
    OptixDeviceContextValidationMode validationMode;  // 验证模式
};
// logCallbackLevel: 1=fatal, 2=error, 3=warning, 4=print
```

### Module

```cpp
// 从 OptiX IR 或 PTX 创建 Module（注意：不是旧版 optixModuleCreateFromPTX）
OptixResult optixModuleCreate(
    OptixDeviceContext              context,
    const OptixModuleCompileOptions*   moduleCompileOptions,
    const OptixPipelineCompileOptions* pipelineCompileOptions,
    const char*                     input,       // OptiX IR 或 PTX 数据
    size_t                          inputSize,
    char*                           logString,
    size_t*                         logStringSize,
    OptixModule*                    module);
```

```cpp
struct OptixModuleCompileOptions {
    int                          maxRegisterCount;  // 0 = 无限制
    OptixCompileOptimizationLevel optLevel;         // DEFAULT / LEVEL_0..3
    OptixCompileDebugLevel       debugLevel;        // DEFAULT / NONE / MINIMAL / MODERATE / FULL
    const OptixModuleCompileBoundValueEntry* boundValues;
    unsigned int                 numBoundValues;
    unsigned int                 numPayloadTypes;
    const OptixPayloadType*      payloadTypes;
    OptixModule                  baseModule;        // 特化用，通常 nullptr
};
```

### ProgramGroup

```cpp
OptixResult optixProgramGroupCreate(
    OptixDeviceContext           context,
    const OptixProgramGroupDesc* programDescriptions,
    unsigned int                 numProgramGroups,
    const OptixProgramGroupOptions* options,
    char*                        logString,
    size_t*                      logStringSize,
    OptixProgramGroup*           programGroups);

OptixResult optixProgramGroupDestroy(OptixProgramGroup programGroup);
```

```cpp
struct OptixProgramGroupDesc {
    OptixProgramGroupKind kind;  // RAYGEN / MISS / HITGROUP / ...
    unsigned int          flags; // 通常 0
    union {
        OptixProgramGroupSingleModule raygen;    // { module, entryFunctionName }
        OptixProgramGroupSingleModule miss;      // { module, entryFunctionName }
        OptixProgramGroupHitgroup     hitgroup;  // 见下
        // callables 省略
    };
};

struct OptixProgramGroupSingleModule {
    OptixModule module;
    const char* entryFunctionName;  // e.g. "__raygen__rg"
};

struct OptixProgramGroupHitgroup {
    OptixModule moduleCH;              // closest-hit module（可 nullptr）
    const char* entryFunctionNameCH;
    OptixModule moduleAH;              // any-hit module（可 nullptr）
    const char* entryFunctionNameAH;
    OptixModule moduleIS;              // intersection module（三角形时 nullptr）
    const char* entryFunctionNameIS;
};
```

### Pipeline

```cpp
OptixResult optixPipelineCreate(
    OptixDeviceContext                 context,
    const OptixPipelineCompileOptions* pipelineCompileOptions,
    const OptixPipelineLinkOptions*    pipelineLinkOptions,
    const OptixProgramGroup*           programGroups,
    unsigned int                       numProgramGroups,
    char*                              logString,
    size_t*                            logStringSize,
    OptixPipeline*                     pipeline);

// 手动版：需自行计算各 stack size
OptixResult optixPipelineSetStackSize(
    OptixPipeline pipeline,
    unsigned int  directCallableStackSizeFromTraversal,
    unsigned int  directCallableStackSizeFromState,
    unsigned int  continuationStackSize,
    unsigned int  maxTraversableGraphDepth);

// 简化版（推荐）：传入语义参数，内部自动计算 stack size
OptixResult optixPipelineSetStackSizeFromCallDepths(
    OptixPipeline pipeline,
    unsigned int  maxTraceDepth,
    unsigned int  maxContinuationCallableDepth,
    unsigned int  maxDirectCallableDepthFromState,
    unsigned int  maxDirectCallableDepthFromTraversal,
    unsigned int  maxTraversableGraphDepth);
```

```cpp
struct OptixPipelineCompileOptions {
    int          usesMotionBlur;
    unsigned int traversableGraphFlags;        // ALLOW_SINGLE_GAS / ALLOW_SINGLE_LEVEL_INSTANCING / ALLOW_ANY
    int          numPayloadValues;             // 32-bit payload 寄存器数
    int          numAttributeValues;           // 通常 2（三角形重心坐标）
    unsigned int exceptionFlags;               // NONE / STACK_OVERFLOW / TRACE_DEPTH / DEBUG
    const char*  pipelineLaunchParamsVariableName;  // 设备侧 extern __constant__ 变量名
    size_t       pipelineLaunchParamsSizeInBytes;
    unsigned int usesPrimitiveTypeFlags;       // 0 = 三角形 + custom
    int          allowOpacityMicromaps;
    int          allowClusteredGeometry;
};

struct OptixPipelineLinkOptions {
    unsigned int maxTraceDepth;
    unsigned int maxContinuationCallableDepth;
    unsigned int maxDirectCallableDepthFromState;
    unsigned int maxDirectCallableDepthFromTraversal;
    unsigned int maxTraversableGraphDepth;
};
```

### SBT

```cpp
OptixResult optixSbtRecordPackHeader(
    OptixProgramGroup programGroup,
    void*             sbtRecordHeaderHostPointer);  // 写 32 字节 header

// 常量
#define OPTIX_SBT_RECORD_HEADER_SIZE  32   // header 大小（字节）
#define OPTIX_SBT_RECORD_ALIGNMENT    16   // record 最小对齐

// SBT record 模板（用户定义）
template<typename T>
struct SbtRecord {
    __align__(OPTIX_SBT_RECORD_ALIGNMENT)
    char header[OPTIX_SBT_RECORD_HEADER_SIZE];
    T data;  // 用户数据（可省略）
};

struct OptixShaderBindingTable {
    CUdeviceptr  raygenRecord;
    CUdeviceptr  exceptionRecord;
    CUdeviceptr  missRecordBase;
    unsigned int missRecordStrideInBytes;
    unsigned int missRecordCount;
    CUdeviceptr  hitgroupRecordBase;
    unsigned int hitgroupRecordStrideInBytes;
    unsigned int hitgroupRecordCount;
    CUdeviceptr  callablesRecordBase;
    unsigned int callablesRecordStrideInBytes;
    unsigned int callablesRecordCount;
};
```

### Launch

```cpp
OptixResult optixLaunch(
    OptixPipeline                    pipeline,
    CUstream                         stream,
    CUdeviceptr                      pipelineParams,      // 设备侧 LaunchParams 地址
    size_t                           pipelineParamsSize,
    const OptixShaderBindingTable*   sbt,
    unsigned int                     width,
    unsigned int                     height,
    unsigned int                     depth);
```

设备侧声明（每个需要访问的 module 中）：

```cpp
extern "C" __constant__ LaunchParams params;
// 变量名必须与 pipelineLaunchParamsVariableName 一致
```

---

## 完成标准

- optixLaunch 输出纯色到累积 buffer
- Tone mapping 正确转换 HDR → LDR 到显示 buffer
- 累积 buffer ping-pong 机制就位（交替读写）
- 窗口显示纯色 + ImGui 面板
- Resize 正常（累积 buffer + 显示 buffer 重建）
- 无 validation / OptiX / CUDA 报错
- 测试 kernel 已移除
