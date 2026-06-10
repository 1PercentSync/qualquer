# 当前阶段

> 目标：Vulkan 基础设施 + ImGui + 黑色背景
> 任务清单见 `tasks/phase1.md`。

---

## 实现步骤

### 依赖关系

```
Step 1 → Step 2 → Step 3 → Step 4 → Step 5 → Step 6
```

### 总览

| Step | 主题 | 验证标准 |
|------|------|----------|
| 1 | 窗口 | GLFW 窗口显示，可关闭 |
| 2 | Vulkan Instance | validation layer 正常输出 |
| 3 | Vulkan Device | 物理设备信息打印 |
| 4 | Swapchain | 无报错创建 |
| 5 | 帧循环 | 黑色背景显示 |
| 6 | ImGui | UI 面板显示 |

---

## 完成标准

- 窗口显示黑色背景
- ImGui 面板可交互
- 窗口可调整大小（swapchain 重建）
- 关闭窗口正常退出
