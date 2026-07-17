# Phase 4.5 任务清单

> 目标：自适应帧率 + 采样质量收尾（Stochastic Alpha、Ray Cone LOD、Normal Map Specular AA）
>
> 设计与实现指南见 `docs/current-phase.md`（与下列小项同序）。
> Phase 4 已完成项见 `tasks/archive/phase4.md`。
>
> 每完成一个复选框暂停等待审查。一个 Step 结束时应请求用户在 CLion 中编译验证。

---

## Step 15：自适应 Sample 数

- [ ] 刷新率查询
- [ ] 帧时间测量
- [ ] Mode 选择逻辑
MUSTREAD:4
- [ ] Ping-pong / 串行切换
- [ ] UI：mode 显示与手动/自动切换
- [ ] 请求用户在 CLion 中编译验证（自适应切换正常，各 mode 帧率符合目标）

## Step 16：Stochastic Alpha

- [ ] anyhit：Blend 模式 stochastic alpha
- [ ] 请求用户在 CLion 中编译验证（blend 材质正确半透明）

## Step 17：Ray Cone LOD

- [ ] payload 接入 cone_width / cone_spread
- [ ] raygen 初始化 cone
- [ ] closesthit 传播与更新 cone
- [ ] 纹理采样改为 cone 驱动 LOD
- [ ] 请求用户在 CLion 中编译验证（高频纹理 aliasing 减少，远处纹理正确模糊）

## Step 18：Normal Map Specular AA

- [ ] 由 ray cone footprint 估算法线贴图方差
- [ ] 方差叠加到 roughness 并重算 alpha
- [ ] 请求用户在 CLion 中编译验证（法线贴图接缝处 specular 闪烁减少）
