# Phase 4.5 任务清单

> 目标：自适应帧率 + 采样质量收尾（Stochastic Alpha、Ray Cone LOD、Normal Map Specular AA）
>
> 设计见 `docs/current-phase.md`。选型理由见 `docs/technical-decisions.md`。
> Phase 4 已完成项（含原 4.5 Step 9–14.5）见 `tasks/archive/phase4.md`。
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

---

## Step 15：自适应 Sample 数

- [ ] 刷新率查询：GLFW `glfwGetVideoMode` 获取当前显示器刷新率
- [ ] 帧时间测量：CUDA events 测量 raygen kernel 执行时间
- [ ] Mode 选择逻辑：根据实测帧时间判断 Mode 1 / 2 / 3，计算目标帧率
MUSTREAD:4
- [ ] Ping-pong / 串行切换：Mode 1 保持双 stream 并行，Mode 2/3 切换到单 stream 串行
- [ ] UI：当前 mode 显示、手动/自动切换开关
- [ ] 请求用户在 CLion 中编译验证（自适应切换正常，各 mode 帧率符合目标）

## Step 16：Stochastic Alpha

- [ ] anyhit 扩展：alpha_mode==Blend 时 `hash(pixel_index, sample_index, primitive_id) > texel_alpha → optixIgnoreIntersection()`（hash 而非 Sobol）
- [ ] 请求用户在 CLion 中编译验证（blend 材质正确半透明）

## Step 17：Ray Cone LOD

- [ ] Payload 扩展至 18 registers：p16 → cone_width（float），p17 → cone_spread（float）
- [ ] Raygen 初始化：cone_width = 0，cone_spread = 2 × tan(0.5 × fov) / render_height（primary ray pixel footprint）
- [ ] Closesthit 更新：cone_width += cone_spread × hit_distance，bounce 时 cone_spread 根据 BRDF 散射特性更新
- [ ] 纹理采样改 `tex2DLod`：LOD = log2(cone_width × texture_resolution / triangle_footprint)
- [ ] 请求用户在 CLion 中编译验证（高频纹理 aliasing 减少，远处纹理正确模糊）

## Step 18：Normal Map Specular AA

- [ ] 基于 ray cone footprint 估算法线贴图方差（Kaplanyan 2016 方案）
- [ ] 方差叠加到 roughness²：`roughness² += variance`，clamp 后重算 alpha
- [ ] 请求用户在 CLion 中编译验证（法线贴图接缝处 specular 闪烁减少）
