# 当前阶段

> 目标：Vulkan 基础设施 + CUDA-Vulkan Interop + ImGui + 测试图案
> 任务清单见 `tasks/phase1.md`。

---

## 实现步骤

### 依赖关系

```
Step 1 → Step 2 → Step 3 → Step 4 → Step 5 → Step 6 → Step 7 → Step 8 → Step 9 → Step 10 → Step 11 → Step 12 → Step 13 → Step 14
```

### 总览

| Step | 主题 | 验证标准 |
|------|------|----------|
| 1 | 项目骨架 | CMake 构建通过 |
| 2 | Application 框架 + 窗口 | GLFW 窗口显示，可关闭 |
| 3 | Vulkan Instance | validation layer 正常输出 |
| 4 | Vulkan Device | 物理设备信息打印 |
| 5 | Swapchain | 无报错创建 |
| 6 | 帧同步与命令录制 | 编译通过 |
| 7 | 帧循环与呈现 | 黑色背景显示，resize 不崩溃 |
| 8 | ImGui 集成 | demo window 显示 |
| 9 | 调试面板 | 自定义面板显示 FPS |
| 10 | optix 层 + CUDA Context | 编译通过，CUDA 设备信息打印 |
| 11 | 初始化顺序重构 | CUDA 和 Vulkan 选择同一 GPU |
| 12 | Vulkan Interop 资源 | 编译通过 |
| 13 | CUDA 导入 + 测试 Kernel | 编译通过 |
| 14 | 帧循环集成（CUDA blit） | 窗口显示渐变色 + ImGui，resize 不崩溃 |

---

## 技术要点

### Vulkan 版本与扩展

- Vulkan 1.4（synchronization2、dynamic_rendering 为核心特性）
- VK_KHR_swapchain

### Swapchain

- Format：B8G8R8A8_SRGB + VK_COLOR_SPACE_SRGB_NONLINEAR_KHR（硬要求，不支持则报错退出，无回退）
- Present mode：单一回退 FIFO（请求的 mode 不支持则退 FIFO，不级联到 Immediate）

### 帧同步

- Frames in flight：2
- 每帧资源：command pool、command buffer、fence、2 semaphore（acquire + present）

### ImGui

- 使用 Dynamic Rendering（无 render pass）
- 直接渲染到 swapchain image

### CUDA-Vulkan Interop

- CUDA 初始化在 Vulkan 之前，Vulkan 通过 UUID 匹配 CUDA 设备
- 设备扩展：`VK_KHR_external_memory_win32`、`VK_KHR_external_semaphore_win32`
- 显示 buffer：`R8G8B8A8_UNORM`、OPTIMAL tiling、手动 `vkAllocateMemory`（不走 VMA）
- CUDA 侧通过 `cudaSurfaceObject_t` 写入显示 buffer
- 同步：per-frame binary external semaphore × 2，CUDA signal → Vulkan wait
- Blit：`vkCmdBlitImage`（显示 buffer → swapchain image，线性→sRGB 自动编码）
- 测试 kernel：UV 渐变 + 帧号驱动动画

---

## 完成标准

- 窗口显示 CUDA 渲染的渐变测试图案
- ImGui 面板可交互
- 窗口可调整大小（swapchain + 显示 buffer 重建）
- 关闭窗口正常退出
- CUDA-Vulkan 同步正确（无 validation 报错）
