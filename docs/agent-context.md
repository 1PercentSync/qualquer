# Agent 会话上下文

> 每次会话开始时阅读本文档，快速定位当前状态和所需上下文。
>
> **验证要求**：阅读完所有必读文档后，找到每篇文档中的 `MUSTREAD:<数字>` 标记，计算所有数字之和，并在开始工作前报告该数值。

---

## 当前位置

- **项目**：Qualquer — 基于 CUDA + OptiX 的 Path Tracer
- **分支**：main
- **Phase**：M1 Phase 3 — 场景加载 + 完整材质系统 + 加速结构 + Primary Ray
- **进度**：Phase 3 Step 3 进行中——CudaTexture 完成，下一步 cache.h + cache.cpp

### 下一个任务

Step 3 第二项：`renderer/include/qualquer/renderer/cache.h` + `renderer/src/cache.cpp`（cache_root、content_hash、cache_path、atomic_write_file），链接 xxhash。

> Phase 1、Phase 2 全部完成并归档。

---

## 必读文档

> **严禁**在读完以下所有文档前执行任何任务或回复用户。

| 文档 | 说明 |
|------|------|
| `docs/roadmap.md` | Phase 概览与顺序 |
| `docs/current-phase.md` | 当前阶段目标、范围、实现指南（Phase 3） |
| `tasks/phase3.md` | Phase 3 可执行任务清单 |
| `docs/architecture.md` | 渲染器架构与设计理念 |
| `docs/technical-decisions.md` | 技术选型与决策 |

---

## 归档文档

| 文档 | 说明 |
|------|------|
| `docs/archive/phase1.md` | Phase 1 目标、范围、实现指南（已归档） |
| `tasks/archive/phase1.md` | Phase 1 可执行任务清单（已归档） |
| `docs/archive/phase2.md` | Phase 2 目标、范围、实现指南（已归档） |
| `tasks/archive/phase2.md` | Phase 2 可执行任务清单（已归档） |
---

## 维护规则

- **每完成一小项后**：更新"当前位置"的进度描述和"下一个任务"
- **Phase 切换时**：更新 Phase 行、进度、下一个任务、必读文档列表
- **文档归档时**：移入"归档"，更新路径
