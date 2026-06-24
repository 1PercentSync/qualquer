# 渲染器架构与设计理念

> 渲染器在长远视角下的架构特征、层次结构、边界约束和贯穿技术选型的设计理念。
> 技术选型结果见 `technical-decisions.md`。

---

## 项目定位

Qualquer 是基于 CUDA + OptiX 的 Path Tracer，Vulkan 仅用于 swapchain 呈现。

- **性质**：个人长期学习和练习项目
- **渲染方式**：Path Tracing（glTF mesh）
- **开发方式**：AI 辅助开发

---

## 硬件目标

- **目标平台**：支持 OptiX 的 NVIDIA GPU（RTX 40 系 / Ada Lovelace 及以上）
- **性能理念**：追求技术和画面的最佳性价比

---

## 设计原则

### 排除过于复杂而收益不高的技术

复杂度和收益必须成正比。实现成本远高于视觉或性能收益的技术，排除。

### 渐进式实现

先能用，再好用，再优秀。每个模块可分阶段实现，阶段间的演进应尽量自然。

### 业界已验证的技术

采用有成熟实现和资料的技术，不做实验性方案。

### 性能性价比

同等画面质量选性能更优的方案；同等性能选画面更好的方案。

### 决策可更新性

`technical-decisions.md` 中的决策是某时刻的最优解快照，不是不可更改的约束。决策的正确性取决于其理由在当前场景下是否仍成立，而非是否已被记录。区分两类依据：

- **硬约束**：保护架构属性（如编译期单向依赖、所有权单一性）。违反会破坏项目结构，不可妥协。
- **软理由**：性能偏好、便利性、初始化顺序等（如"CUDA→Vulkan 以便 CUDA 失败时省去 Vulkan 初始化"）。仅在被其声称的收益所支撑时成立。

当用户提议与某条已记录决策冲突，且新场景暴露了该决策无法覆盖的问题时，冲突本身是更新信号：用新场景检验旧理由，软理由失效就更新决策与文档，而非用"决策已存在"作为反对依据。已记录的决策不应被当作权威服从对象。

---

## 架构层次

```
        app
         ↓
      renderer
       ↓   ↓
   optix   vulkan
```

严格单向依赖（上层依赖下层，下层不知道上层的存在）。

基础设施（数学、日志、工具函数）不单独成层，放到实际使用的层中。

### optix（OptiX 封装层）

封装 OptiX 和 CUDA API，向上层提供简洁的 RT 接口。

包含：Context 管理、Module/Pipeline/SBT 创建、加速结构（BLAS/TLAS）、Denoiser、CUDA 内存管理。

**设计原则**：不包含任何渲染逻辑。

### vulkan（Vulkan 呈现层）

仅负责 swapchain 呈现和 CUDA-Vulkan interop。

包含：Instance/Device/Queue 管理、Swapchain、CUDA external memory/semaphore interop。

**设计原则**：最小化，只做呈现。
MUSTREAD:4

### renderer（渲染逻辑层）

实现 PT 渲染管线和场景管理，负责**单帧渲染内容的录制**：拿到命令缓冲和帧输入，录制一帧要画什么（PT kernel 调度、tone map、blit、ImGui 录制）。

包含：PT 着色逻辑、材质系统、场景数据结构、累积 buffer、降噪调度。

**不持有帧循环**：acquire/submit/present、fence/semaphore 同步、swapchain 重建都在 app 层。renderer 只暴露 “渲染内容” 接口（CUDA 提交 + Vulkan 命令录制），由 app 编排好时序后调用。理由：一帧的渲染内容横跨 optix（CUDA 着色）和 vulkan（blit/呈现），有内部数据依赖和时序编排，只有同时依赖两者的 renderer 能承载；但帧循环骨架（编排+同步）属于“驱动时序”，与“画什么”正交，归 app。这条切法来自 Himalaya 的实证——其 Renderer 是 Application 帧循环膨胀后抽出的“渲染内容”，编排+同步始终留在 Application。

**与 Himalaya 的区别**：Himalaya 的 Layer 1（Framework）和 Layer 2（Pass）在这里合并。原因：
- PT 是单个 OptiX launch，不需要 Render Graph 编排多 Pass
- CUDA stream 天然顺序执行，不需要复杂的依赖声明
- "Pass" 就是 PT kernel + denoiser，无需独立抽象

### app（应用层）

场景加载、资产管理、相机控制、用户输入、UI，以及**帧循环的编排与同步**。

帧循环（wait fence → acquire image → renderer 录制 → submit → present → resize 重建）在 app 层。app 同时驱动 optix 和 vulkan 做时序编排。

---

## 核心架构特征

### 无 Render Graph

与 Himalaya 不同，Qualquer 不需要 Render Graph：
- Himalaya 有多个 Pass 需要声明式编排和 barrier 管理
- Qualquer 的渲染是 OptiX launch → Denoiser → Vulkan blit 的线性流程
- CUDA stream 保证顺序执行

### 材质系统

复用 Himalaya 的材质数据结构思路：
- GPU 材质数据（PBR 参数 + bindless 纹理索引）
- 统一顶点格式
- 无条件纹理采样（bindless + default 纹理）

### 场景表示

渲染器接收 mesh 实例列表和相机参数。环境照明来自 IBL，面光源来自 emissive 材质三角形。

---

## 架构约束

| 约束 | 保护的架构属性 |
|------|---------------|
| 编译期单向依赖：上层依赖下层，下层不依赖上层 | 层级清晰、可独立演进 |
| 不做无脑 Vulkan wrapper | wrapper 参数仍是 Vulkan 类型，不隐藏细节，徒增间接 |

编译期单向依赖是真正的硬约束（CMake 层级、include 关系强制）。Vulkan 原生类型和 API 调用允许出现在任意层——上层直接使用 Vulkan 句柄、直接调用 `vkCmd*` / `vkQueue*` 是正常的，因为渲染逻辑本就需要这些。只为隔离原始类型而做的 wrapper（参数照搬 `VkXxx` 的薄封装）没有收益，不引入。

---
