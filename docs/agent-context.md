# Agent 会话上下文

> 每次会话开始时阅读本文档，快速定位当前状态和所需上下文。
>
> **验证要求**：阅读完所有必读文档后，找到每篇文档中的 `MUSTREAD:<数字>` 标记，计算所有数字之和，并在开始工作前报告该数值。

---

## 当前位置

- **项目**：Qualquer — 基于 CUDA + OptiX 的 Path Tracer
- **分支**：main
- **Phase**：M1 Phase 1 — Vulkan 基础设施 + ImGui + 调试面板
- **进度**：Step 12 进行中（InteropImage、InteropSemaphore 类声明已创建，待实现）

### 下一个任务

Step 12 第 2 项：实现 InteropImage（R8G8B8A8_UNORM VkImage + OPTIMAL tiling + 手动 vkAllocateMemory with VkExportMemoryAllocateInfo + Win32 HANDLE 导出 + destroy）

> Step 12 第 1 项完成。InteropImage/InteropSemaphore 类声明就位，接下来实现 InteropImage 的创建、dedicated exportable memory 绑定与 Win32 handle 导出。详见 `tasks/phase1.md`。

---

## 必读文档

> **严禁**在读完以下所有文档前执行任何任务或回复用户。

| 文档 | 说明 |
|------|------|
| `docs/roadmap.md` | Phase 概览与顺序 |
| `docs/current-phase.md` | 当前阶段目标、范围、实现指南 |
| `tasks/phase1.md` | Phase 1 可执行任务清单 |
| `docs/architecture.md` | 渲染器架构与设计理念 |
| `docs/technical-decisions.md` | 技术选型与决策 |

---

## 归档文档

| 文档 | 说明 |
|------|------|

---

## 维护规则

- **每完成一小项后**：更新"当前位置"的进度描述和"下一个任务"
- **Phase 切换时**：更新 Phase 行、进度、下一个任务、必读文档列表
- **文档归档时**：移入"归档"，更新路径
