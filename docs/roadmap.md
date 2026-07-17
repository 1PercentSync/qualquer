# 开发路线

> 整体 Milestone 和 Phase 规划。

---

## 项目长期目标

**实时 Path Tracing 渲染器**：游戏级实时性能 + 物理正确性。

- **M1** 移植 himalaya PT，建立物理正确的离线/准实时基础（OptiX RT + CUDA + Vulkan 呈现）。
- **M2** 引入 NRC、OMM、ReSTIR GI、Path Guiding、NEE-AT 等，达到游戏实时性能；ReSTIR PT 在前述完成后另议。

所有 Phase 划分和特性取舍服务于这一长期目标。单个 Phase 的收敛指标（如 Phase 4 的 "4000 spp 大致收敛"）是 **Phase 范围划分判据**，不是项目终点——特性归属判据见各 Phase 文档。

---

## Milestone 概览

| Milestone | 主题 | 核心技术 |
|-----------|------|----------|
| M1 | 移植 himalaya PT | OptiX RT Pipeline, DLSS-RR, VK-CUDA interop |
| M2 | 现代游戏 PT 管线 | NRC, OMM, ReSTIR GI, Path Guiding, NEE-AT（ReSTIR PT 另议） |

---

## M1：移植 himalaya PT

从 himalaya 的 Vulkan + OIDN 实现移植到 CUDA/OptiX + DLSS-RR，实时呈现使用 Vulkan swapchain。

### 参考项目

**Himalaya 路径**：`D:\CLionProjects\himalaya`

Himalaya 是 Vulkan + Vulkan RT + OIDN 的 PT 实现。Qualquer 不是其源码移植，而是在 M1 **移植阶段**以其为参考、用 CUDA/OptiX 重新实现。Step 实现前讨论见 `AGENTS.md`。

- **不得**把 Himalaya 的实现当作正确性证明（对错以物理/规范/本仓库测试与文档为准）
- **仅**在移植相关任务中对照；非移植任务**不**以 Himalaya 为参考

### 移植范围

- **场景加载**：glTF mesh、材质、纹理（BC 压缩、mip、bindless 思路复用）
- **RT 核心**：BLAS/TLAS 构建（OptiX 版）、GeometryInfo、材质访问
MUSTREAD:5
- **PT 着色**：Sobol + hash 去相关采样、PBR BRDF（EON diffuse + GGX specular + Turquin 补偿）、ray offset、normal mapping、ray cone LOD
- **光源**：IBL alias table、emissive 三角形 NEE、MIS
- **Non-opaque**：alpha mask、stochastic alpha
- **降噪与放大**：DLSS-RR（时域累积 + 去噪 + 放大，替代原计划的 OptiX Denoiser）
- **呈现**：Vulkan swapchain + CUDA-Vulkan interop

### Phase 划分

| Phase | 目标 |
|-------|------|
| 1 | 基础设施 + 窗口 + CUDA Interop（vulkan/optix，swapchain，ImGui，CUDA 测试图案） |
| 2 | OptiX 基础（Pipeline/SBT，raygen 输出纯色，累积 buffer ping-pong，双 Stream 流水线，替换测试 kernel） |
| 3 | 场景 + 材质（glTF，BC 纹理，PBR 材质，BLAS/TLAS，交互式相机，ambient 着色） |
| 4 | 核心 PT + 实时前置（Megakernel + SER，BRDF，NEE + MIS，IBL，Tonemap，Sobol，RR，分辨率解耦，DLSS-RR，4000 spp 收敛） |
| 4.5 | 自适应帧率 + 采样质量收尾（自适应 spp，Stochastic Alpha，Ray Cone LOD，Normal Map Specular AA） |

---

## M2：现代游戏 PT 管线

在 M1 物理正确 PT + DLSS-RR 基础上，引入实时向采样与几何/缓存能力。

### 范围

| 项 | 角色 |
|----|------|
| NRC | 神经辐射缓存，降低间接光照方差 / 加速收敛 |
| OMM | Opacity Micromap，加速 alpha-tested 几何遍历 |
| ReSTIR GI | 时空复用间接光照样本 |
| Path Guiding | 学习/引导次级路径方向，降低方差 |
| NEE-AT | 下一事件估计的自适应/增强采样（具体方案实现期再定） |

### 另议

**ReSTIR PT**：不在上述主清单内。NRC、OMM、ReSTIR GI、Path Guiding、NEE-AT **全部完成之后**，再评估是否做、如何与既有管线衔接。

### Phase 划分

待 M1 结束后再拆。

---
