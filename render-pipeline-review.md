
## 5. 既有优化可能产生反作用的检查项

以下不是静态代码即可定性的错误。必须通过 release 编译资源报告、Nsight Systems/Compute、GPU 时间线和图像 A/B 验证。

### QRP-O16：除精确 `Release` 外都部署 DLSS development DLL，可能污染常用 profiling 配置

#### 代码事实

- `app/CMakeLists.txt:62-68` 仅当 `$<CONFIG:Release>` 为真时复制 `nvngx_dlssd.dll` 的 rel 版本；`RelWithDebInfo`、`MinSizeRel` 和自定义优化配置均得到 dev DLL。
- 其他代码以 `NDEBUG` 区分 release/debug，因此 `RelWithDebInfo` 通常已经走优化后的 renderer/OptiX 路径，却仍搭配 NGX development runtime。

#### 可能反作用与验证

CLion/Nsight 常用 `RelWithDebInfo` 保留符号做 profiling；此时 DLSS timing、VRAM、日志与画质行为可能不代表 release runtime，导致错误归因。部署规则应按“仅 Debug 用 dev，其余优化配置用 rel”或显式 profile option 选择，并在启动日志中记录实际 DLL/version。用同一 scene 对 dev/rel DLL 比较 evaluate 时间与输出，正式性能结论必须基于可发布 runtime。


