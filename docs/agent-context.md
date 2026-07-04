# Agent 会话上下文

> 每次会话开始时阅读本文档，快速定位当前状态和所需上下文。
>
> **验证要求**：阅读完所有必读文档后，找到每篇文档中的 `MUSTREAD:<数字>` 标记，计算所有数字之和，并在开始工作前报告该数值。

---

## 当前位置

- **项目**：Qualquer — 基于 CUDA + OptiX 的 Path Tracer
- **分支**：main
- **Phase**：M1 Phase 3 — 场景加载 + 完整材质系统 + 加速结构 + Primary Ray
- **进度**：Phase 3 Step 8 第1小项完成：DebugUI 新增 Scene 段（当前文件名 + tooltip + "Load..." 按钮触发 Win32 文件对话框 → DebugUIActions.scene_load_requested）；Application switch_scene 实现（drain 两条 CUDA stream → scene_loader_.destroy → load → renderer_.load_scene 无条件重建 AS → set_focus_target + auto_position → save_config），帧循环在 draw 后接入 scene_load_requested；与 Himalaya 差异：不用 vkQueueWaitIdle（场景资源全 CUDA-owned，无 Vulkan 队列绑定），改用 CUDA stream drain

- **验证 gap**：场景 PBR ambient 着色 + 交互式相机浏览 + 场景切换待端到端验证（Step 8 后续小项）
- **实现偏差（已同步文档）**：delta time 实际用 ImGui::GetIO().DeltaTime（非 glfwGetTime）——方案 A 将 begin_frame 提前到 submit_cuda 前，ImGui DeltaTime 在 controller.update 时已当帧有效；current-phase.md 帧循环/delta time 节已更新

### 下一个任务

Phase 3 Step 8 第2小项：请求用户在 CLion 中编译验证（场景切换正常）

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

### 进度与下一个任务的编写模板

**小项完成**：

```
- **进度**：Phase X Step Y 第N小项完成：<文件/模块名> 已创建/实现（<具体内容描述>）

### 下一个任务

Phase X Step Y 第N+1小项：<下一小项的简述>
```

**Step 完成**（编译验证通过后）：

```
- **进度**：Phase X Step Y 完成（<整体概括>）

### 下一个任务

Phase X Step Y+1 第一小项：<下一 Step 首项的简述>
```

