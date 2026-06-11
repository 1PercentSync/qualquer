# 当前阶段

> 目标：Vulkan 基础设施 + ImGui + 黑色背景
> 任务清单见 `tasks/phase1.md`。

---

## 实现步骤

### 依赖关系

```
Step 1 → Step 2 → Step 3 → Step 4 → Step 5 → Step 6 → Step 7 → Step 8 → Step 9
```

### 总览

| Step | 主题 | 验证标准 |
|------|------|----------|
| 1 | 项目骨架 | CMake 构建通过 |
| 2 | 窗口 | GLFW 窗口显示，可关闭 |
| 3 | Vulkan Instance | validation layer 正常输出 |
| 4 | Vulkan Device | 物理设备信息打印 |
| 5 | Swapchain | 无报错创建 |
| 6 | 帧同步与命令录制 | 编译通过 |
| 7 | 帧循环与呈现 | 黑色背景显示，resize 不崩溃 |
| 8 | ImGui 集成 | demo window 显示 |
| 9 | 调试面板 | 自定义面板显示 FPS |

---

## 技术要点

### Vulkan 版本与扩展

- Vulkan 1.4（synchronization2、dynamic_rendering 为核心特性）
- VK_KHR_swapchain

### Swapchain

- Format：B8G8R8A8_SRGB + VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
- Present mode：MAILBOX → IMMEDIATE → FIFO（按优先级回退）

### 帧同步

- Frames in flight：2
- 每帧资源：command pool、command buffer、fence、2 semaphore（acquire + present）

### ImGui

- 使用 Dynamic Rendering（无 render pass）
- 直接渲染到 swapchain image

---

## 完成标准

- 窗口显示黑色背景
- ImGui 面板可交互
- 窗口可调整大小（swapchain 重建）
- 关闭窗口正常退出
