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
- **只能** 创建和编辑 C++/CUDA 源码文件、文档及构建配置
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

**「一小项」的定义**：`tasks/` 清单中每一个 `- [ ]` 复选框条目就是一小项。每完成一个须暂停等待用户审查，严禁合并多个复选框为一次完成。

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

**语言**：description 用英文；type/scope 已是英文，保持一致

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
- **不写会变化的措辞**：所有代码注释/文档禁止使用开发阶段名（Phase/Step 等）和未来时态（will/later 等）等会随开发过时的表述
- **去冗余**：注释保留 why（为什么、非显然语义、设计约束），删除复述 what 的内容（代码已表达的无需再说）

### 控制流

- `if`、`else`、`for`、`while` 等语句体即使只有一行也**必须**使用 `{}`，不得省略

### 函数参数

- **不用默认参数**：函数声明不设默认值，调用方显式传值。避免调用点隐式依赖默认值、签名变更时遗漏实参。

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

## 工作区改动处理

### IDE 删 include

工作区出现的 include 删除是 IDE 基于传递性包含分析的结果。**编译/链接失败前不主动恢复**——“显式依赖更卫生”是揣测，不构成恢复依据。

### 格式化与 tidy 是用户意图

工作区中的格式化与 tidy 应用均为**用户主动操作**的产物，代表用户意图，不视为自动污染。默认一并接受；质疑须有实际理由，不得凭样式偏好逐个横挑。

### 局部回退须限定范围

`git checkout -- <file>` 会无差别丢弃该文件**全部**未提交内容（任何来源、是否有价值一律清空），用于“回退局部某行”时属越权。如需精准回退，用 patch 模式（`git checkout -p`）或手动编辑只移除真正要撤的几行。

---

## 设计原则

### 所有权

**原则**：句柄与资源的单一所有者，使用方不缓存副本。

每个资源（Vulkan 句柄、CUDA 句柄、buffer/image 引用、context 等）只有一个所有者（owner）。其他对象作为使用方，在 init/destroy/recreate 等操作时从调用方接收所有者的引用，**不**在自身成员中缓存任何由所有者拥有的句柄或引用。

**理由**：

- 单一来源（single source of truth）：句柄只存在一处，避免多份拷贝因句柄重建/替换而过期
- 句柄缓存开销可忽略（多为指针级句柄），按所有权清晰设计，非性能取舍

### 状态与输入分离

**原则**：成员表示对象的客观状态，不充当某次操作的隐式输入。

操作的输入走参数，输出可写回成员。区分标准是：该值属于「对象现在是什么」（状态：拥有的句柄、缓存的能力等）还是「这次调用想要什么」（请求）。前者读成员（getter、读取缓存的 capability 列表均合法），后者必须走参数。

**理由**：

- 成员兼作输入通道时，调用方需先改成员再调用，职责混同、隐藏了调用对成员状态的依赖
- 输入走参数后，函数签名本身声明了依赖；状态成员（如缓存的支持列表）也保持单一来源

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

### Vulkan 结构体

使用 C++20 designated initializers 初始化 Vulkan 结构体，不使用逐字段赋值：

```cpp
// 正确
const VkApplicationInfo app_info{
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .apiVersion = VK_API_VERSION_1_4,
};

// 错误
VkApplicationInfo app_info{};
app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
app_info.apiVersion = VK_API_VERSION_1_4;
```

---

## 第三方库

| 库 | 用途 |
|---|---|

---

