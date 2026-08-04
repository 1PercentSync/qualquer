# CUDA Driver API 性能空间评估

## 结论

Qualquer 有选择性引入 CUDA Driver API 以获取额外性能控制的空间，但全面将 Runtime API 改写为 Driver API 本身基本不会提升每帧性能。合适的方向是保持 Runtime/Driver 混合，只在 Driver API 提供独占能力的位置使用它。

## 当前基础

项目已经处于混合 API 架构：CUDA Runtime 管理常规资源与 kernel，OptiX、DLSS-RR 和部分上下文操作使用 `CUcontext`、`CUstream`、`CUdeviceptr` 等 Driver API 体系。Path Tracing 主体通过 `optixLaunch` 执行，因此普通 CUDA 调用改名为对应 `cu*` 接口不会增加对 OptiX 内部调度的控制。

## 可利用的空间

### VMM 与可压缩显存

Driver API 的虚拟内存管理允许通过 `cuMemCreate` 请求 `CU_MEM_ALLOCATION_COMP_GENERIC`。当设备和驱动实际授予压缩属性时，可压缩数据可能减少 DRAM 带宽、L2 读带宽和 L2 占用。

适合优先验证的资源：

- BLAS/TLAS backing buffer
- 顶点与索引 buffer
- GeometryInfo、材质和光源数据
- Env/emissive alias table

必须先查询 `CU_DEVICE_ATTRIBUTE_GENERIC_COMPRESSION_SUPPORTED`，并在分配后确认实际属性。CUDA arrays、Vulkan 导入的显示图像及 DLSS 内部分配不能由普通线性 VMM allocator 直接覆盖。

### 稳定虚拟地址与稀疏驻留

VMM 可以分离虚拟地址和物理显存，支持稳定设备地址、按页提交及自定义分配布局。这可能改善场景切换、显存碎片和超大场景资源管理。稀疏 CUDA arrays 还可用于纹理按 mip/tile 驻留，但需要独立的纹理流送设计，并非简单 API 替换。

### 模块生命周期

Driver API 可以显式加载和卸载 cubin/PTX，例如在环境贴图转换结束后卸载对应模块。但项目只有少量普通 CUDA kernel，主渲染仍由 OptiX 承担，因此该方向主要影响初始化时间和少量模块驻留资源。

## 不形成性能优势的替换

以下 Runtime→Driver 一对一替换通常进入相同驱动后端，不会改变 GPU 工作：

- `cudaMalloc` → `cuMemAlloc`
- `cudaMemcpyAsync` → `cuMemcpy*Async`
- Runtime event/stream → Driver event/stream
- Runtime external semaphore → Driver external semaphore
- `<<<...>>>` → `cuLaunchKernel`

显式创建独立 CUDA context 也不会天然加速当前单 GPU 管线；多个 context 还可能增加显存、驱动资源和切换成本。Stream priority、CUDA Graph、memory pool、L2 access policy 和大部分 launch attributes 已有 Runtime API 等价能力。

## 验证建议

1. 查询目标 GPU 的 VMM 与 generic compression 支持。
2. 新增独立 VMM buffer 类型，不整体替换现有 `CudaBuffer`。
3. 先仅对 BLAS/TLAS backing buffer 做 A/B 测试。
4. 确认分配实际获得压缩属性。
5. 比较 `optixLaunch` 时间、DRAM/L2 流量和显存占用。
6. 再分别测试其他长期只读线性 buffer，避免混合后无法归因。

## 判断

项目确实存在通过 Driver API 获得性能改善的空间，其中当前最直接的候选是 VMM generic compressible memory；面向大型场景的长期候选是稀疏数组和按页驻留。全面迁移 Driver API 或机械替换等价调用没有明确性能依据。

## 参考资料

- [CUDA Driver API 与 Runtime 互操作](https://docs.nvidia.com/cuda/cuda-programming-guide/03-advanced/driver-api.html)
- [Driver API 与 Runtime API 的区别](https://docs.nvidia.com/cuda/cuda-runtime-api/driver-vs-runtime-api.html)
- [CUDA Virtual Memory Management](https://docs.nvidia.com/cuda/cuda-programming-guide/04-special-topics/virtual-memory-management.html)
- [OptiX Device Context](https://raytracing-docs.nvidia.com/optix9/api/group__optix__host__api__device__context.html)
