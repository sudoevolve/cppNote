# 11-阴影入门：Shadow Map 的最小实现与常见伪影

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 做出第一个可用阴影：Shadow Map 最小闭环（两遍渲染）
- 搞清楚“阴影贴图”存的到底是什么（深度），以及怎么比较
- 认识并能解释三类经典伪影：Shadow Acne、Peter Panning、边缘锯齿

## 先决条件

- 已完成 [10-光照基础：Lambert/Phong/Blinn-Phong 与能量直觉](10-光照基础-Lambert-Phong-BlinnPhong-能量直觉.md)
- 会使用 FBO 的基本概念（本章会给最小落地）

---

## 1. Shadow Map 的核心直觉

阴影不是“画一张黑色纹理贴上去”。它更像一次“从光源视角出发的可见性测试”：

- 从光源看过去，每个方向上离光源最近的表面会记录一个深度值
- 当你从相机视角渲染时，对每个片元做同样的“从光源看过去”的深度比较
- 如果当前片元比记录的深度更远，说明被挡住 → 在阴影里

你只需要记住一句话：

- 阴影贴图存的是“从光源看过去的深度”，不是颜色。

---

## 2. 最小闭环：两遍渲染

### Pass A：从光源视角渲染深度到一张 depth texture

- 目标：得到 `shadowMap`（一张深度纹理）
- 只需要写入深度，不输出颜色

### Pass B：从相机视角正常渲染

- 正常做光照
- 在 fragment 里采样 `shadowMap`，做深度比较，得到 shadow factor

---

## 3. 光源空间：lightView/lightProj/lightSpaceMatrix

对于方向光，常用正交投影；点光需要立方体贴图阴影，本章先做方向光。

lightSpaceMatrix：

```
lightSpace = lightProj * lightView
```

你会把顶点位置变换到光源裁剪空间，然后在 fragment 做比较。

---

## 4. Depth FBO（概念级最小落地）

你需要：

- 一个 FBO
- 一个 depth texture 作为附件
- 关闭颜色输出（只渲深度）

关键 OpenGL 状态（示意）：

- `glBindFramebuffer(GL_FRAMEBUFFER, depthFBO)`
- `glViewport(0,0,shadowW,shadowH)`
- `glClear(GL_DEPTH_BUFFER_BIT)`
- 绘制场景（使用 depth-only shader）

然后回到默认 framebuffer 再正常渲染。

---

## 5. 阴影比较（概念示意）

在 fragment shader，你需要：

1. 把世界位置变换到 light clip space
2. 做齐次除法得到 NDC
3. 映射到 0..1 得到 shadow map 的采样坐标
4. 取出 shadow depth，与当前 depth 比较

你很快会发现阴影“不是二值的”，否则边缘会很锯齿，所以通常会做 PCF（多次采样平均）。

---

## 6. 三大伪影与最小修复策略

### 6.1 Shadow Acne（阴影痤疮）

现象：本来应该亮的面上出现密集的自阴影条纹。

本质：深度比较的数值误差 + 法线方向导致的偏差不够。

最小修复：

- 在比较时加 bias（偏移）
- bias 随法线与光方向夹角变化（斜面需要更大 bias）

### 6.2 Peter Panning（漂浮）

现象：物体阴影与物体分离，像悬空。

本质：bias 太大。

最小修复：

- 减小 bias
- 结合 PCF，尽量用更小 bias

### 6.3 边缘锯齿/抖动

现象：阴影边缘很粗糙或随着镜头移动抖。

常见原因：

- shadow map 分辨率不够
- light 投影范围过大（把精度摊薄了）
- 未做 PCF

最小修复：

- 增加 shadow map 分辨率（先粗暴有效）
- 缩小方向光的投影范围（更重要、更根本）
- 加 PCF

---

## 常见坑

- **忘了恢复 viewport**：从 shadow pass 回来后，画面比例/清屏会异常
- **光源投影范围过大**：阴影像素被摊薄，边缘像马赛克
- **只在某些角度有阴影**：lightSpace 变换或 NDC→UV 映射错

---

## 小练习

### 练习 1：把阴影强度做成可调参数

- 题目：加一个 `uShadowStrength`，在阴影处做 `mix(lit, shadowed, strength)`
- 目标：把阴影从“硬开关”变成可调效果

### 练习 2：做 PCF 并对比

- 题目：实现 3x3 PCF，观察边缘改善与性能代价
- 目标：理解“软阴影”在工程上如何取舍

---

## 小结

- Shadow Map = 从光源视角记录深度，再在相机视角做深度比较
- 两遍渲染是最小闭环：depth pass + lighting pass
- 三大伪影的应对顺序：先调投影范围，再调 bias，再做 PCF

下一篇：见 [12-离屏渲染：FBO、后处理、HDR/曝光](12-离屏渲染-FBO-后处理-HDR-曝光.md)

