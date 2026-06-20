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
- Per-frame 资源（FrameData）：command pool、command buffer、render fence、image_available semaphore
- Per-swapchain-image 资源（Swapchain）：render_finished semaphore

### 帧循环（归属 app 层）

- 流程：`wait fence → acquire → reset fence → begin cmd → [录制] → end cmd → submit2 → present`
- Clear 用 dynamic rendering：`vkCmdBeginRendering` + attachment `loadOp=CLEAR`（黑色）→ `vkCmdEndRendering`。不选 `vkCmdClearColorImage`，因为 ImGui 必须画在 rendering pass 内，用 `begin_rendering`/`end_rendering` 形态在 begin/end 间插入 ImGui 录制即可。`loadOp` 在 Step 14 blit 介入前保持 `CLEAR`（此前 oldLayout=UNDEFINED，无有意义内容可 LOAD）；blit 写入 swapchain image 后改为 `LOAD` 以保留 blit 结果
- swapchain image 的 layout 流转：`UNDEFINED → COLOR_ATTACHMENT_OPTIMAL → PRESENT_SRC_KHR`，两个手写 `vkCmdPipelineBarrier2`（dynamic rendering 不自动管 layout）

### ImGui

- 使用 Dynamic Rendering（无 render pass）
- 直接渲染到 swapchain image
- 封装类归 renderer 层（架构上"ImGui 录制"属渲染内容录制，归 renderer；当前 Application 临时调用，Step 14 renderer 接管 render_frame 内容时已就位）
- Descriptor Pool 由 ImGui backend 自建自销（`DescriptorPoolSize > 0`），不手动管理
- 版本 1.92.6 的 Vulkan backend API 与 Himalaya 参考的 1.91.9 不同：渲染管线信息在 `PipelineInfoMain.PipelineRenderingCreateInfo` 下、`Init` 返回 bool 需检查、需设 `ApiVersion`

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
