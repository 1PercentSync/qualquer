# 技术选型与决策

> 渲染器当前使用的技术方案、选型理由和关键实现决策。
> 架构层面的设计见 `architecture.md`。

---

## CUDA-Vulkan 互操作

### 内存所有权

**决策**：Vulkan 分配，CUDA 导入。

Vulkan 创建 VkImage（启用 external memory 标志），导出 Win32 HANDLE，CUDA 通过 `cudaImportExternalMemory` 映射后写入。

**理由**：
- 主流方案，NVIDIA 官方示例和文档均采用此方式
- Vulkan 原生拥有 VkImage，布局转换和验证层检查完整
- 格式兼容性由 Vulkan 明确定义

备选方案（CUDA 分配，Vulkan 导入）文档稀缺、验证层对外部导入内存检查有限，排除。

### 中间 Buffer

**决策**：使用独立的渲染 buffer，不直接写入 swapchain image。

渲染流程：OptiX 写入渲染 buffer → Vulkan blit 到 swapchain image。

**理由**：
- Swapchain image 由 WSI 创建，无法添加 external memory 标志，不能导出给 CUDA
- Swapchain 格式通常是 8-bit sRGB，不适合 HDR 累积（需要浮点格式避免量化误差）
- Blit 是后处理的天然边界（tone mapping、gamma 校正）

### 双缓冲（Ping-pong）

**决策**：两个 RGBA32F 渲染 buffer，交替读写。

| 帧 | CUDA | Vulkan |
|----|------|--------|
| N | 读 A，写 B | 显示 A |
| N+1 | 读 B，写 A | 显示 B |

**理由**：
- 单缓冲下呈现会阻塞渲染（CUDA 必须等 blit 完成才能写入）
- Ping-pong 使渲染和呈现并行，提高 GPU 利用率
- 相比"累积 buffer + 显示 buffer 分离"方案，无额外 copy，buffer 数量更少

### 同步

使用 Vulkan external semaphore（`VK_KHR_external_semaphore_win32`）协调 CUDA 和 Vulkan 的访问时序。

---
