# Agent 会话上下文

> 每次会话开始时阅读本文档，快速定位当前状态和所需上下文。
>
> **验证要求**：阅读完所有必读文档后，找到每篇文档中的 `MUSTREAD:<数字>` 标记，计算所有数字之和，并在开始工作前报告该数值。

---

## 当前位置

- **项目**：Qualquer — 基于 CUDA + OptiX 的 Path Tracer
- **分支**：main
- **Phase**：M1 Phase 1 — Vulkan 基础设施 + ImGui + 调试面板
- **进度**：Step 14 进行中（CUDA launch + signal 接入，待 Vulkan submit 加 external wait）

### 下一个任务

Step 14 第 6 项：Vulkan submit 添加 external semaphore wait（end_frame 的 submit_info）

> Step 14 第 5 项完成。submit_cuda 就位（kernel launch + signal external semaphore，同 stream 顺序提交）。但 Vulkan 侧还没 wait 该 semaphore，不能单独运行验证（binary semaphore 重复 signal）。接下来在 end_frame 的 submit_info 加 external semaphore wait，配对后才安全。详见 `tasks/phase1.md`。

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
