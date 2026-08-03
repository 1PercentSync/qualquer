
## 5. 既有优化可能产生反作用的检查项

以下不是静态代码即可定性的错误。必须通过 release 编译资源报告、Nsight Systems/Compute、GPU 时间线和图像 A/B 验证。

### QRP-O14：所有 CUDA/OptiX device code 固定以 compute 8.9 为前端目标，新架构仅依赖 JIT forward compatibility

#### 代码事实

- 顶层 `CMakeLists.txt:9` 固定 `CMAKE_CUDA_ARCHITECTURES 89`。CMake 官方语义是无 `-real/-virtual` 后缀时同时生成 sm_89 real code 与 compute_89 virtual PTX，因此更新架构可通过 PTX forward JIT 运行，并非静态兼容性错误。
- `cmake/CompileOptiXIR.cmake:75-101` 也固定 `--gpu-architecture=compute_89` 生成 OptiX IR。
- 这保证 Ada 最低基线，却没有为已知更新目标生成其 native cubin/更高 virtual architecture 前端代码。

#### 可能反作用与验证

Blackwell/后续 GPU 会在首次使用普通 CUDA kernels 时承担 PTX JIT/cache 成本；更重要的是，前端按 8.9 feature set 编译，无法生成受更高 `__CUDA_ARCH__` 条件控制的代码或利用只在更新 virtual architecture 暴露的能力。driver/OptiX backend 仍可针对实际 GPU 重新优化，因此不能仅凭配置断言 steady-state 变慢。

保留 8.9 PTX 作为最低兼容 fallback，同时为明确支持的新架构生成 native code，并评估 OptiX IR 是否应按部署目标拆分。分别比较冷启动 JIT 时间、warm-cache kernel/OptiX frame time、register/local-memory 和 SASS；不要用 `native` 取代可分发的 Ada fallback，也不要无测量地假设更高 `compute_xx` 必然更快。

### QRP-O15：1:1 presentation blit 仍请求 linear filtering

#### 代码事实

- `display_buffer_` 始终按当前 swapchain extent 创建；CUDA tonemap 也直接输出 display resolution。
- `renderer/src/renderer.cpp:1050-1081` 的 blit source/destination rectangles 完全同尺寸，却指定 `VK_FILTER_LINEAR`。
- 1:1 pixel-center 映射没有缩放，linear 与 nearest 应读取同一 source texel；linear 不提供额外重建质量，却额外要求 source format 的 linear-filter blit capability（QRP-036）。

#### 优化机会与验证

改用 `VK_FILTER_NEAREST` 可简化 capability 前提，并避免实现无法折叠 linear filter 时的多余过滤工作；float→sRGB format conversion 仍由 blit 执行。先做逐像素图像 diff 确认 1:1 映射完全一致，再看 Vulkan transfer timestamp；预期性能收益很小，主要价值是更准确的语义和更宽能力覆盖。

### QRP-O16：除精确 `Release` 外都部署 DLSS development DLL，可能污染常用 profiling 配置

#### 代码事实

- `app/CMakeLists.txt:62-68` 仅当 `$<CONFIG:Release>` 为真时复制 `nvngx_dlssd.dll` 的 rel 版本；`RelWithDebInfo`、`MinSizeRel` 和自定义优化配置均得到 dev DLL。
- 其他代码以 `NDEBUG` 区分 release/debug，因此 `RelWithDebInfo` 通常已经走优化后的 renderer/OptiX 路径，却仍搭配 NGX development runtime。

#### 可能反作用与验证

CLion/Nsight 常用 `RelWithDebInfo` 保留符号做 profiling；此时 DLSS timing、VRAM、日志与画质行为可能不代表 release runtime，导致错误归因。部署规则应按“仅 Debug 用 dev，其余优化配置用 rel”或显式 profile option 选择，并在启动日志中记录实际 DLL/version。用同一 scene 对 dev/rel DLL 比较 evaluate 时间与输出，正式性能结论必须基于可发布 runtime。


