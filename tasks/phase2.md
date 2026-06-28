# Phase 2 任务清单

> 目标：OptiX Pipeline 框架（Pipeline/SBT，raygen 输出纯色，累积 buffer ping-pong，替换测试 kernel）
> 详细设计见 `docs/current-phase.md`，技术决策见 `docs/technical-decisions.md`。
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

---

## Step 1：OptiX SDK 集成 + DeviceContext

- [x] 创建 `cmake/FindOptiX.cmake`（通过默认安装路径查找 include 目录，设置 `OptiX_INCLUDE_DIR`）
- [x] 更新 `optix/CMakeLists.txt`（引入 FindOptiX，添加 OptiX include 路径）+ 顶层 CMakeLists.txt 注册 cmake/ 模块路径
- [x] 创建 `optix/include/qualquer/optix/optix_check.h`（OPTIX_CHECK 宏）+ `flush_on(critical)` 保证 abort 前日志输出
- [x] `optix::Context` 新增 `OptixDeviceContext` 成员，init 中 `optixInit()` + `optixDeviceContextCreate()`（含日志回调），destroy 中销毁
- [x] 请求用户在 CLion 中编译验证

## Step 2：CudaBuffer\<T\>

- [x] 创建 `optix/include/qualquer/optix/cuda_buffer.h`（RAII 模板：alloc / free / resize / upload / clear / data / device_ptr / count / size_bytes）
- [x] 请求用户在 CLion 中编译验证

## Step 3：OptiX IR 编译与加载基础设施

- [x] 创建 `cmake/CompileOptiXIR.cmake`（`compile_optix_ir(target file1.cu ...)` 函数：nvcc --optix-ir → .optixir，POST_BUILD 部署到运行目录）
- [x] 创建 `renderer/src/device/programs.cu`（最小 `__raygen__rg` 空实现，验证编译链路）
- [x] 更新 `renderer/CMakeLists.txt`（引入 compile_optix_ir，编译 programs.cu 并部署 .optixir）
- [x] 请求用户在 CLion 中编译验证

MUSTREAD:4
## Step 4：LaunchParams + 设备程序

- [x] 创建 `renderer/include/qualquer/renderer/launch_params.h`（LaunchParams 结构体：累积 buffer 指针、宽高、帧号）
- [x] 完善 `renderer/src/device/programs.cu`（`__raygen__rg` 写纯色到累积 buffer、`__miss__ms` / `__closesthit__ch` / `__anyhit__ah` 空实现）
- [x] 请求用户在 CLion 中编译验证

## Step 5：Pipeline 类

- [x] 创建 `optix/include/qualquer/optix/pipeline.h`（Pipeline 类：init / destroy，持有 Module + ProgramGroup[] + Pipeline handle，ProgramGroup 公开暴露）
- [x] 创建 `optix/src/pipeline.cpp`（读 .optixir 文件创建 Module、ProgramGroup 创建、Pipeline 链接、stack size 配置）
- [x] 更新 `optix/CMakeLists.txt`（新增 pipeline.cpp）
- [x] 请求用户在 CLion 中编译验证

## Step 6：累积 buffer + Tone mapping

- [x] 创建 `renderer/include/qualquer/renderer/tonemap.h`（tone map kernel launch 入口声明）
- [x] 创建 `renderer/src/tonemap.cu`（读 float4 累积 buffer → clamp → 写 uchar4 到 display surface）并更新 `renderer/CMakeLists.txt`（新增 tonemap.cu）
- [x] 请求用户在 CLion 中编译验证

## Step 7：Renderer 状态化重构

- [x] `Renderer` 新增 init / destroy / resize 声明（init 接收 OptixDeviceContext + 宽高；destroy 释放所有资源；resize 重建累积 buffer）
- [x] `Renderer::init` 实现：创建 Pipeline（读 .optixir 文件）、构建 SBT records（pack header + upload 三块 CudaBuffer）、分配累积 buffers（2 × CudaBuffer\<float4\>）和 LaunchParams buffer
- [x] `Renderer::resize` 实现：累积 buffer resize + clear
- [x] `Renderer::destroy` 实现：释放 Pipeline、SBT buffers、累积 buffers、LaunchParams buffer
- [x] 请求用户在 CLion 中编译验证

## Step 8：optixLaunch + 帧循环集成 + 清理

- [x] `submit_cuda` 实现更新：LaunchParams 填充 + 上传 → `optixLaunch`（pipeline, stream, params, sbt, w, h, 1）→ tone map → signal semaphore；ping-pong 索引翻转
- [x] Application 接线：init 中调 `Renderer::init`，destroy 中调 `Renderer::destroy`，resize 路径调 `Renderer::resize`
- [x] 移除 `renderer/src/test_kernel.cu` 和 `renderer/include/qualquer/renderer/test_kernel.h`
- [ ] 更新 `renderer/CMakeLists.txt`（移除 test_kernel.cu）
- [ ] 请求用户在 CLion 中编译验证（窗口显示纯色 + ImGui，resize 不崩溃，无 validation / OptiX / CUDA 报错）
