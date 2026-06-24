# Agent 会话上下文

> 每次会话开始时阅读本文档，快速定位当前状态和所需上下文。
>
> **验证要求**：阅读完所有必读文档后，找到每篇文档中的 `MUSTREAD:<数字>` 标记，计算所有数字之和，并在开始工作前报告该数值。

---

## 当前位置

- **项目**：Qualquer — 基于 CUDA + OptiX 的 Path Tracer
- **分支**：main
- **Phase**：M1 Phase 2 — OptiX 基础（Pipeline/SBT，raygen 输出纯色，替换测试 kernel）
- **进度**：Phase 1 完成（CUDA-Vulkan interop + 渐变色测试图案打通，编译运行验证通过）；Phase 文档已归档，待创建 Phase 2 文档集

### 下一个任务

创建 `docs/current-phase.md`（Phase 2 目标/范围/技术要点）与 `tasks/phase2.md`（Phase 2 可执行任务清单）

> Phase 1 全部完成并归档。窗口显示 CUDA 渐变色 + ImGui 面板，resize 不崩溃，无 validation 报错。进入 Phase 2 规划。详见 `docs/roadmap.md`。

---

## 必读文档

> **严禁**在读完以下所有文档前执行任何任务或回复用户。

| 文档 | 说明 |
|------|------|
| `docs/roadmap.md` | Phase 概览与顺序 |
| `docs/current-phase.md` | 当前阶段目标、范围、实现指南（Phase 2，待创建） |
| `tasks/phase2.md` | Phase 2 可执行任务清单（待创建） |
| `docs/architecture.md` | 渲染器架构与设计理念 |
| `docs/technical-decisions.md` | 技术选型与决策 |

---

## 归档文档

| 文档 | 说明 |
|------|------|
| `docs/archive/phase1.md` | Phase 1 目标、范围、实现指南（已归档） |
| `tasks/archive/phase1.md` | Phase 1 可执行任务清单（已归档） |
---

## 维护规则

- **每完成一小项后**：更新"当前位置"的进度描述和"下一个任务"
- **Phase 切换时**：更新 Phase 行、进度、下一个任务、必读文档列表
- **文档归档时**：移入"归档"，更新路径
