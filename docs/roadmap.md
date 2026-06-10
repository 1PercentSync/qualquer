# 开发路线

> 整体 Milestone 和 Phase 规划。

---

## Milestone 概览

| Milestone | 主题 | 核心技术 |
|-----------|------|----------|
| M1 | 移植 himalaya PT | OptiX RT Pipeline, OptiX Denoiser, VK-CUDA interop |
| M2 | 现代游戏 PT 管线 | ReSTIR DI/GI, SHaRC/NRC, NRD, temporal accumulation |

---

## M1：移植 himalaya PT

从 himalaya 的 Vulkan + OIDN 实现移植到 CUDA/OptiX + OptiX Denoiser，实时呈现使用 Vulkan swapchain。

### 移植范围

- **场景加载**：glTF mesh、材质、纹理（BC 压缩、mip、bindless 思路复用）
- **RT 核心**：BLAS/TLAS 构建（OptiX 版）、GeometryInfo、材质访问
- **PT 着色**：Sobol + blue noise 采样、PBR BRDF、ray offset、normal mapping、ray cone LOD
- **光源**：IBL alias table、emissive 三角形 NEE、MIS
- **Non-opaque**：alpha mask、stochastic alpha
- **降噪**：OptiX Denoiser（替代 OIDN）
- **呈现**：Vulkan swapchain + CUDA-Vulkan interop

### Phase 划分

| Phase | 目标 |
|-------|------|
| 1 | 基础设施 + 窗口（core/vulkan，swapchain，ImGui，空白画面） |
| 2 | OptiX 基础（Context/Pipeline/SBT，raygen 输出纯色） |
| 3 | 场景加载 + AS（glTF，BLAS/TLAS，primary ray） |
| 4 | 材质 + 纹理（PBR，纹理加载，bindless） |
| 5 | PT 着色（BRDF，多 bounce，采样） |
| 6 | 光源（IBL，emissive NEE，MIS） |
| 7 | 降噪 + 累积（OptiX Denoiser，accumulation）|

---

## M2：现代游戏 PT 管线

待定。

---
