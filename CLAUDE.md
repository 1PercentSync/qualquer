# Qualquer 渲染器

基于 CUDA + OptiX 的 Path Tracer。

- 设计文档索引：`docs/agent-context.md`
- 任务规划：`tasks/`

## 文档规范

- 文档内容：中文
- 文档文件名：英文

---

## 开发环境

| 项 | 值 |
|---|---|
| 操作系统 | Windows 11 |
| IDE | CLion |
| 编译器 | MSVC |
| CMake | 4.2 |
| 包管理 | vcpkg (manifest mode) |
| CUDA | 13.2 |
| OptiX | 9.1.0 |
| Vulkan | 1.4 |
| C++ | C++20 |

代码通过 WSL 中的 Agent 编辑，所有构建、编译、运行操作由用户在 CLion 中手动完成。

### Agent 约束

- **禁止** 使用 Subagent（Agent 工具）或 Plan 工具，除非用户主动要求
- **禁止** 执行 cmake、build、run 命令
- 修改构建配置和工具链文件（CMakeLists.txt、vcpkg.json、.clangd 等）时**必须向用户请求**，经用户同意后方可编辑
- **只能** 创建和编辑 C++/CUDA 源码文件及文档（构建配置除外，见上条）
- 需要验证时告知用户在 CLion 中操作
- **禁止**在没有经过确认的情况下推断项目规划（如某功能属于哪个 Milestone/Phase），必须先查 `docs/` 下的文档确认后再陈述
- **禁止**例行 Git 检查；只有出现明确风险信号（空白异常、补丁应用失败、用户要求）时，才针对具体文件运行最小必要检查

### 工作流程

**首次实现前复述**：每次会话中，用户第一次要求开始实现任务时，必须先**完整复述**以下工作流程，然后立即开始实现；不得将复述理解为需要等待用户额外确认。

1. 完成 `tasks/` 中的**一小项**任务后**暂停**，等待用户审查
2. 用户允许提交代码即代表该小项**验收通过**（用户回复"继续"等同于：验收通过 → 提交 → 更新进度 → 推送 → 开始下一项）
3. **提交 commit**（代码和文档分开提交）
4. 提交后**更新 `tasks/`** 和 **`docs/agent-context.md`**，作为单独的 `docs` commit
5. **push** 到远程仓库（每次提交都必须推送，不得积压）
6. 不要求每一小项都能编译，只需一个 Step 结束时编译通过即可

**「一小项」的定义**：`tasks/` 清单中每一个 `- [ ]` 复选框条目就是一小项。例如 `- [ ] Swapchain 创建（format、present mode、extent 选择）` 是一小项，`- [ ] Image View 获取` 是另一小项。**严禁**将多个复选框条目合并为一次完成，每完成一个复选框就必须停下来等待用户审查。

### 上下文管理

- **会话开始**：阅读 `docs/agent-context.md`，根据其中的必读/按需列表加载所需文档

### Commit 规范

格式：`<type>(<scope>): <description>`

| type | 用途 |
|------|------|
| `feat` | 新功能 / 新代码 |
| `docs` | 文档 |
| `chore` | 构建配置、工具链、杂项维护 |
| `fix` | 修复 bug |
| `refactor` | 重构（不改变行为） |

scope 可选，使用架构层名

**分离原则**：代码变更和文档变更必须分开提交，不得混在同一个 commit 中

### 文档归档规范

Phase 切换或文档被替换时，旧文档必须先归档再创建新文档：

1. 使用 `git mv` 将旧文档移入 `docs/archive/` 或 `tasks/archive/`（保留 git 跟踪历史）
2. 归档作为**独立 commit**，不与新文档创建混在一起
3. `docs/` 和 `tasks/` 下的文件**成对归档**（如 `current-phase.md` 和对应的 `tasks/phaseN.md`）
4. 归档后更新 `docs/agent-context.md` 的归档文档列表

---

## 编码规范

### 头文件

使用 `#pragma once`，路径带项目前缀：`#include <qualquer/<layer>/xxx.h>`

### 代码文档

- 格式：Javadoc 风格 Doxygen（`/** */`）
- 语言：英文
- `.h` + `.cpp` 配对：`.h` 写接口文档（**what**：做什么、语义、约束），`.cpp` 写实现注释（**why/how**：为什么这么做、算法解释）
- 仅 `.h`（纯头文件库 / 数据结构定义）：文档全部写在 `.h` 中，同时包含 what 和 why
- 仅 `.cpp`（如 `main.cpp`）：文档全部写在 `.cpp` 中，同时包含 what 和 why
- 所有公开和私有接口、字段、枚举值均需写文档
- 文件头文档（`@file`）只写不会随开发变化的内容（文件名、所属模块），不写会过时的功能描述

### 控制流

- `if`、`else`、`for`、`while` 等语句体即使只有一行也**必须**使用 `{}`，不得省略

### 日志

- 日志级别按语义选择（info 就是 info，warn 就是 warn），不为迁就当前 `kLogLevel` 而提升级别
- `kLogLevel` 控制运行时过滤，需要看到更多信息时调整它即可

### 命名

| 元素 | 规范 |
|------|------|
| 类 / 结构体 | `PascalCase` |
| 方法 / 自由函数 | `snake_case` |
| 私有成员 | `snake_case` 后缀 `_` |
| 公有成员 / 局部变量 | `snake_case` |
| 命名空间 | `snake_case` |
| 枚举值 (scoped) | `PascalCase` |
| 常量 | `kPascalCase` |
| 宏 | `SCREAMING_CASE` |

### 命名空间

与架构层次对应：

| 命名空间 | 层次 | 职责 |
|----------|------|------|
| `qualquer::optix` | OptiX 封装层 | Context、Pipeline、AS、Denoiser |
| `qualquer::vulkan` | Vulkan 呈现层 | Swapchain、interop |
| `qualquer::renderer` | 渲染逻辑层 | PT 管线、材质、场景 |
| `qualquer::app` | 应用层 | 窗口、输入、加载、UI |

---

## 项目结构

每层独立 CMake static library，编译期强制单向依赖（上层依赖下层，下层不知道上层的存在）。

```
        app
         ↓
      renderer
       ↓   ↓
   optix   vulkan
```

### 第三方库集成规范

vcpkg 有端口的库通过 `vcpkg.json` 管理。vcpkg 无端口的库放入 `third_party/<name>/`，每个库自带 `CMakeLists.txt`，顶层 `CMakeLists.txt` 通过 `add_subdirectory()` 引入，消费方只需 `target_link_libraries()` 链接目标名。

| 类型 | 目录结构 |
|------|----------|
| 源码编译 | `include/qualquer/<name>/` + `src/` |
| 预编译二进制 | `include/` + `lib/` + `bin/`（保持上游布局） |

源码编译库的头文件放入 `include/qualquer/<name>/` 以统一 include 路径风格（`#include <qualquer/<name>/xxx.h>`）。PRIVATE include 指向头文件目录，解决源文件中的相对路径引用。第三方代码统一禁用编译器警告（MSVC `/W0`，GCC/Clang `-w`）。

---

## 关键技术约定

随开发逐步填充。

---

## 第三方库

| 库 | 用途 |
|---|---|

---

