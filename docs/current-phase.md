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
| 3 | OptiX IR 编译基础设施 | 编译通过，最小设备程序编译为 embedded IR |
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

### OptiX IR 编译与嵌入

- **格式**：OptiX IR（nvcc `--optix-ir`），不用 PTX——OptiX 编译器可做更深层优化
- **嵌入**：构建时 `bin2c` 将 .optixir 转为 C 字节数组常量，编译链接进可执行文件。无运行时文件路径管理
- **CMake**：`cmake/CompileOptiXIR.cmake` 提供 `compile_optix_ir(target file1.cu ...)` 函数。内部流程：nvcc `--optix-ir` → .optixir → `bin2c --const` → .c → 加入 target 源文件
- 编译标志：`--optix-ir -I${OptiX_INCLUDE} --gpu-architecture=compute_89`

生成符号示例（由 `bin2c` 命名）：
```cpp
extern "C" const unsigned char programs_optixir[];
extern "C" const unsigned int  programs_optixir_size;
```

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

### Pipeline（optix 层）

`optix::Pipeline` 类：持有 OptixModule + OptixProgramGroup[] + OptixPipeline handle。init 接收 OptixDeviceContext + embedded IR（数据指针 + 大小）。

- **ProgramGroup** 作为公开成员暴露——renderer 需要它们来 pack SBT records
- **PipelineCompileOptions**：`numPayloadValues`、`numAttributeValues=2`（三角形重心坐标）、`traversableGraphFlags`
- **PipelineLinkOptions**：`maxTraceDepth=1`（Phase 2 raygen 不调 optixTrace，但 Pipeline 要求 ≥1）
- **Stack size**：`optixPipelineSetStackSize`，Phase 2 值保守，后续按实际 trace depth 调整

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

## 完成标准

- optixLaunch 输出纯色到累积 buffer
- Tone mapping 正确转换 HDR → LDR 到显示 buffer
- 累积 buffer ping-pong 机制就位（交替读写）
- 窗口显示纯色 + ImGui 面板
- Resize 正常（累积 buffer + 显示 buffer 重建）
- 无 validation / OptiX / CUDA 报错
- 测试 kernel 已移除
