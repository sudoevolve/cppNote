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

## 7. MSAA：最“白给”的抗锯齿（以及离屏怎么接）

MSAA（多重采样）解决的是“几何边缘的锯齿”。它的工程特点是：

- 对边缘更明显（尤其是对比强烈的轮廓）
- 对 shader 内部细节（贴图高频、subpixel 线条）帮助有限
- 成本通常比更复杂的后处理 AA 更可控

### 7.1 默认 framebuffer 的 MSAA（最简单）

如果你直接渲染到默认 framebuffer，通常只需要：

1. 创建窗口时请求 samples
2. 开启 `GL_MULTISAMPLE`

GLFW 示例（创建窗口前）：

```cpp
glfwWindowHint(GLFW_SAMPLES, 4);
```

初始化（一次即可）：

```cpp
glEnable(GL_MULTISAMPLE);
```

这条路的限制是：你不容易把“MSAA 后的结果”当纹理采样来做后处理。

### 7.2 离屏 + MSAA：需要一次 resolve（blit）

如果你既要 MSAA，又要后处理，常见链路是两级 FBO：

- MSAA FBO：渲染场景（附件是多重采样 renderbuffer/texture）
- Resolve FBO：把 MSAA 结果 resolve 到普通 texture（用于 screen pass 采样）

关键点是：多重采样纹理不能直接用 `sampler2D` 当普通纹理采样（它是 `sampler2DMS`）。

工程上更常用的是 resolve：

```cpp
glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFbo);
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFbo);
glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
```

之后 screen pass 采样 `resolveFbo` 的 color texture 就和你当前章节的流程一致了。

### 7.3 MSAA FBO 的附件怎么选

最省心的组合通常是：

- color：多重采样 renderbuffer（只用于 resolve，不需要采样）
- depth/stencil：多重采样 renderbuffer

如果你确实想直接在 shader 里采 MSAA（比如自己做 resolve 或某些特殊效果），才会去用多重采样 texture + `sampler2DMS`。

---

## 8. 抗锯齿路线：MSAA / FXAA / TAA 怎么选

这里先给一个“工程口径”，避免初学阶段被名词拖走：

- MSAA：解决几何边缘锯齿；实现简单；离屏需要 resolve
- FXAA：后处理，成本低；对贴图/细线也有帮助，但会略糊
- TAA：画质潜力大但工程复杂（历史缓冲、抖动、重投影、拖影处理）

如果你的教程主线目标是“稳定做项目”：

- 先把 MSAA 跑通（默认 framebuffer 或离屏 resolve 版）
- 再做一个 FXAA（作为 screen pass 的可选后处理）

---

## 常见坑

- **FBO 不完整**：附件没配齐，必须检查 framebuffer status
- **viewport 没恢复**：离屏尺寸与窗口尺寸不同，容易导致最终画面缩放/裁剪异常
- **深度丢失**：scene pass 没有深度附件会导致 3D 遮挡错误
- **MSAA 开了但看不出区别**：确认是否真的创建了多重采样 buffer，以及是否开启了 `GL_MULTISAMPLE`
- **离屏 MSAA 后处理异常**：确认是否做了 resolve（blit），以及采样的是否是普通 texture

---

## 小练习

### 练习 1：做一个可切换的后处理开关

- 题目：按键切换：原图 vs 灰度 vs 反相
- 目标：把 screen pass 变成可扩展的效果入口

### 练习 2：加入曝光参数（概念级）

- 题目：在 screen pass 里加 `exposure`，做一次简单 tone mapping（例如 `1 - exp(-c * exposure)`）
- 目标：建立 HDR→LDR 的最小通路

### 练习 3：把离屏渲染升级为“MSAA + resolve + 后处理”

- 题目：scene pass 渲染到 MSAA FBO，再 blit 到普通 FBO 的纹理，最后做 screen pass 后处理
- 目标：把“抗锯齿”和“后处理”合并成一条稳定链路

---

## 小结

- FBO 让你把“一帧”变成一张可采样的纹理
- 后处理就是屏幕空间 shader：先让链路跑通再追求复杂效果
- HDR 的核心是先装下亮度范围，再做映射与曝光控制
- MSAA 是高性价比的几何抗锯齿；离屏时要记得 resolve 到普通纹理

下一篇：见 [13-性能与工程：状态切换、批处理、调试与 Profiling](13-性能与工程-状态切换-批处理-调试-Profiling.md)
