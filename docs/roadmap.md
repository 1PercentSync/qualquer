# 开发路线

> 整体 Milestone 和 Phase 规划。

---

## 项目长期目标

**实时 Path Tracing 渲染器**：游戏级实时性能 + 物理正确性。

- **M1** 移植 himalaya PT，建立物理正确的离线/准实时基础（OptiX RT + CUDA + Vulkan 呈现）。
- **M2** 引入 ReSTIR DI/GI、SHaRC/NRC、NRD 等现代管线，达到游戏实时性能。

所有 Phase 划分和特性取舍服务于这一长期目标。单个 Phase 的收敛指标（如 Phase 4 的 "4000 spp 大致收敛"）是 **Phase 范围划分判据**，不是项目终点——特性归属判据见各 Phase 文档。

---

## Milestone 概览

| Milestone | 主题 | 核心技术 |
|-----------|------|----------|
| M1 | 移植 himalaya PT | OptiX RT Pipeline, OptiX Denoiser, VK-CUDA interop |
| M2 | 现代游戏 PT 管线 | ReSTIR DI/GI, SHaRC/NRC, NRD, temporal accumulation |

---

## M1：移植 himalaya PT

从 himalaya 的 Vulkan + OIDN 实现移植到 CUDA/OptiX + OptiX Denoiser，实时呈现使用 Vulkan swapchain。

### 参考项目

**Himalaya 路径**：`D:\CLionProjects\himalaya`

Himalaya 是 Vulkan + Vulkan RT + OIDN 的 PT 实现，作为 M1 的参考和学习对象。Qualquer 不是 Himalaya 的移植，而是以其为参考，用 CUDA/OptiX 技术栈重新实现。

### M1 工作流程

每个 Step 执行前，按以下顺序调研：

1. **先读 Qualquer**：从任务清单条目提取本 Step 涉及的 Qualquer 模块（头文件、实现、相关基础设施），建立「已有什么、缺什么」的认知
2. **再读 Himalaya**：在 Qualquer 认知基础上阅读 Himalaya 对应代码，差异对比才有锚点

据读到的内容在回复中陈述读了哪些区域、得到什么判定及依据（供用户校验判定是否站得住），并按结果分流：

- **有对应实现**：与用户讨论三点——
  1. **Himalaya 的实现逻辑**：它做了什么，为什么这么做
  2. **Qualquer 的实现计划**：我们怎么做，基于我们的架构和技术栈
  3. **差异说明**：精简了什么、做了什么适应、适应的理由

- **无对应实现**：向用户陈述 Qualquer 的实现计划与取舍依据——无可参照的 Himalaya 实现，讨论聚焦 Qualquer 自身方案。

两种情形下讨论都是交互节点（须用户认可计划），不是实现前的单方面背景陈述，不得讨论完直接实现。

**粒度**：三点讨论的粒度是 **Step**，不是 checkbox 小项。同一 Step 内，首项的三点讨论经用户认可后，该 Step 的后续 checkbox 直接执行，不重复三点讨论；只有进入**下一个 Step** 时才重新触发“读 Himalaya + 三点讨论”。

**预期**：基础设施搭建过程中，从 Himalaya 那边无法获得有价值的信息是正常的，按“无对应”分支处理即可。

### 移植范围

- **场景加载**：glTF mesh、材质、纹理（BC 压缩、mip、bindless 思路复用）
- **RT 核心**：BLAS/TLAS 构建（OptiX 版）、GeometryInfo、材质访问
MUSTREAD:5
- **PT 着色**：Sobol + blue noise 采样、PBR BRDF、ray offset、normal mapping、ray cone LOD
- **光源**：IBL alias table、emissive 三角形 NEE、MIS
- **Non-opaque**：alpha mask、stochastic alpha
- **降噪**：OptiX Denoiser（替代 OIDN）
- **呈现**：Vulkan swapchain + CUDA-Vulkan interop

### Phase 划分

| Phase | 目标 |
|-------|------|
| 1 | 基础设施 + 窗口 + CUDA Interop（vulkan/optix，swapchain，ImGui，CUDA 测试图案） |
| 2 | OptiX 基础（Pipeline/SBT，raygen 输出纯色，累积 buffer ping-pong，双 Stream 流水线，替换测试 kernel） |
| 3 | 场景 + 材质（glTF，BC 纹理，PBR 材质，BLAS/TLAS，交互式相机，ambient 着色） |
| 4 | 核心 PT（Megakernel + SER，BRDF，多 bounce，IBL cubemap，NEE + MIS，Tonemap，4000 spp 收敛） |
| 4.5 | 收敛质量 + DLSS-RR + 自适应（Sobol + hash 去相关，Ray Cone LOD，Russian Roulette，DLSS-RR，自适应帧率） |

---

## M2：现代游戏 PT 管线

待定。

---
