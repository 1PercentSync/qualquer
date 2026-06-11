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

### Buffer 架构

**决策**：累积 buffer（2 个，ping-pong）+ 显示 buffer（1 个）

| Buffer | 数量 | 格式 | 所有权 | 用途 |
|--------|------|------|--------|------|
| 累积 buffer | 2 | RGBA32F | CUDA 独占 | HDR 累积，ping-pong 交替读写 |
| 显示 buffer | 1 | RGBA8 | Vulkan 分配，CUDA 导入 | LDR 输出，Vulkan blit 到 swapchain |

**帧流程**：

| 帧 | 累积 | Tone mapping | Vulkan |
|----|------|--------------|--------|
| N | 读 A，写 B | 读 A → 显示 buffer | blit 显示 buffer |
| N+1 | 读 B，写 A | 读 B → 显示 buffer | blit 显示 buffer |

- Tone mapping 读取上一帧的累积结果（显示延迟一帧）
- 累积写入和 tone mapping 读取可并行（操作不同 buffer）
- Tone mapping 在 CUDA 侧完成（HDR→LDR），Vulkan 只做 blit

**性能特征**：

```
每帧时间 = max(T_累积, T_tonemap + T_呈现)
```

- 若 T_累积 > T_tonemap + T_呈现：瓶颈在累积，tone map + 呈现被完全隐藏
- 若 T_累积 < T_tonemap + T_呈现：瓶颈在呈现侧，无法完全隐藏

**理由**：
- 累积 buffer 不需要 interop，简化同步
- 显示 buffer 是 LDR（RGBA8），带宽最小
- 单显示 buffer 在 T_累积 > T_呈现 时足够（典型 PT 场景）

### 同步

使用 Vulkan external semaphore（`VK_KHR_external_semaphore_win32`）协调 CUDA 和 Vulkan 对显示 buffer 的访问时序。

### 后续改进方向

1. **单帧多 sample 累积**：每帧累积多个 sample 以提高收敛速度
2. **自适应采样**：根据实际帧率动态调整每帧 sample 数

---
