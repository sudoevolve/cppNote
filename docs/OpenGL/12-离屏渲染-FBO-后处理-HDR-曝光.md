# 12-离屏渲染：FBO、后处理、HDR/曝光

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 建立离屏渲染（FBO）的工程骨架：先渲染到纹理，再渲染到屏幕
- 做一个最小后处理：灰度/反相/简单色调映射（二选一即可）
- 建立 HDR 的直觉：为什么需要更高动态范围与曝光控制

## 先决条件

- 已完成 [06-纹理：采样、wrap/filter、mipmap、sRGB](06-纹理-采样-wrap-filter-mipmap-sRGB.md)
- 已理解 [11-阴影入门：Shadow Map 的最小实现与常见伪影](11-阴影入门-ShadowMap最小实现-常见伪影.md) 的“先渲染到纹理再使用”的思路

---

## 1. 为什么要离屏：你想做的效果不在“直接画到屏幕”里

一旦你想做这些事，离屏几乎是必经之路：

- 后处理（模糊、Bloom、色调映射、FXAA）
- 屏幕空间效果（SSAO、SSR）
- 多 pass 渲染（先几何再光照，延迟渲染）

离屏渲染的核心思想极其简单：

- 把一帧先画进一张纹理（color buffer），再用一张全屏四边形把它画到屏幕上

---

## 2. FBO 最小组成

一个可用的 framebuffer 至少要有：

- 一个颜色附件（color texture / renderbuffer）
- 一个深度附件（depth renderbuffer 或 depth texture）

主线建议：

- color 用 texture（后处理需要采样）
- depth 用 renderbuffer（如果你不需要采样深度，renderbuffer 更简单）

---

## 3. 两段渲染：Scene Pass + Screen Pass

### 3.1 Scene Pass（渲染到 FBO）

关键步骤（概念）：

- bind FBO
- viewport 设为离屏大小
- 清 color/depth
- 正常渲染你的场景

### 3.2 Screen Pass（渲染到默认 framebuffer）

- bind 默认 framebuffer（0）
- viewport 设为窗口大小
- 画一个全屏三角形/四边形
- fragment shader 采样 scene color texture，并做后处理

---

## 4. 全屏 Pass：建议用“全屏三角形”

全屏四边形需要两个三角形 + 顶点数据；全屏三角形更省事，也更常见。

直觉：

- 让一个大三角形覆盖整个屏幕
- 在 fragment shader 里用 `gl_FragCoord` 或插值的 UV 来采样

你不必现在就完美实现，先把“屏幕 Pass 能采到上一遍的颜色纹理”跑通。

---

## 5. 最小后处理：灰度/反相

灰度（直觉版）：

```glsl
vec3 c = texture(uScene, vUV).rgb;
float g = dot(c, vec3(0.2126, 0.7152, 0.0722));
FragColor = vec4(vec3(g), 1.0);
```

反相：

```glsl
vec3 c = texture(uScene, vUV).rgb;
FragColor = vec4(vec3(1.0) - c, 1.0);
```

这一步的意义在于：

- 你确认了离屏纹理采样链路没问题
- 后面的 Bloom、色调映射只是“更复杂的屏幕 shader”

---

## 6. HDR 与曝光：先要直觉，再落地

当你进入真实光照/多光源/高亮效果时，LDR（0..1）颜色范围会迅速饱和：

- 高亮一律变成白
- 亮度层次丢失

HDR 的工程路线通常是：

1. 场景渲染到 float color buffer（例如 RGBA16F）
2. 在 screen pass 做 tone mapping（把 HDR 映射回 LDR）
3. 加曝光参数 `exposure`，让亮度可控

你可以先把 HDR 当作一句工程口径：

- 不要在 LDR 里硬堆亮度；先把亮度范围装下，再映射到屏幕。

---

## 常见坑

- **FBO 不完整**：附件没配齐，必须检查 framebuffer status
- **viewport 没恢复**：离屏尺寸与窗口尺寸不同，容易导致最终画面缩放/裁剪异常
- **深度丢失**：scene pass 没有深度附件会导致 3D 遮挡错误

---

## 小练习

### 练习 1：做一个可切换的后处理开关

- 题目：按键切换：原图 vs 灰度 vs 反相
- 目标：把 screen pass 变成可扩展的效果入口

### 练习 2：加入曝光参数（概念级）

- 题目：在 screen pass 里加 `exposure`，做一次简单 tone mapping（例如 `1 - exp(-c * exposure)`）
- 目标：建立 HDR→LDR 的最小通路

---

## 小结

- FBO 让你把“一帧”变成一张可采样的纹理
- 后处理就是屏幕空间 shader：先让链路跑通再追求复杂效果
- HDR 的核心是先装下亮度范围，再做映射与曝光控制

下一篇：见 [13-性能与工程：状态切换、批处理、调试与 Profiling](13-性能与工程-状态切换-批处理-调试-Profiling.md)

