# 14-PBR 入门：BRDF 的直觉与实现骨架

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 建立 PBR 的工程直觉：你在模拟什么，哪些参数是“材质语言”
- 认识最常用的金属度/粗糙度工作流（Metallic-Roughness）
- 给出一个可迭代的实现骨架：从 Blinn-Phong 过渡到微表面模型

## 先决条件

- 已完成 [10-光照基础：Lambert/Phong/Blinn-Phong 与能量直觉](10-光照基础-Lambert-Phong-BlinnPhong-能量直觉.md)
- 建议已完成 [12-离屏渲染：FBO、后处理、HDR/曝光](12-离屏渲染-FBO-后处理-HDR-曝光.md)（PBR 常需要更合理的亮度范围）

---

## 1. PBR 不是“更复杂的公式”，而是一套一致的材质语义

传统 Blinn-Phong 的问题不是“不能用”，而是：

- 参数不直观：shininess/specStrength 很难跨材质复用
- 能量行为不稳定：你很容易把光越加越亮

PBR（这里指常见的微表面模型）提供的是一套更一致的材质表达方式：

- baseColor（基色）
- metallic（金属度）
- roughness（粗糙度）
- normal（法线贴图，细节）

---

## 2. 微表面直觉：表面由无数微小镜面组成

粗糙度决定了微小镜面的朝向分布：

- roughness 小：高光集中、锐利
- roughness 大：高光分散、柔和

金属度决定了“反射与漫反射”的分配：

- 非金属：主要是漫反射 + 少量镜面反射（F0 接近 0.04）
- 金属：几乎没有漫反射，镜面反射颜色接近 baseColor

这两句直觉几乎能解释你看到的 80% PBR 外观变化。

---

## 3. 最小 PBR 骨架：Cook-Torrance（概念级）

常见骨架会包含三块：

- NDF（法线分布函数）D：高光形状
- 几何遮蔽项 G：微表面之间互相遮挡
- 菲涅尔项 F：视角越斜越“亮”

组合成镜面项：

```
spec = (D * G * F) / (4 * (N·L) * (N·V))
```

入门阶段你不必一步到位。建议迭代顺序：

1. 先保留 Lambert 漫反射
2. 用 Schlick 近似实现 F
3. 加 GGX 的 D
4. 加 Smith 的 G
5. 再把能量守恒（kd/ks 分配）调正确

---

## 4. 材质参数组织：从“散装 uniform”到“可复用材质”

当你进入 PBR，材质参数会明显增多：

- baseColor（vec3）
- metallic（float）
- roughness（float）
- ao（float，可选）
- 各类贴图（baseColor/normal/metallicRoughness/ao）

工程建议：

- 先用统一的材质结构（CPU 侧）聚合参数
- shader 侧用一个固定命名约定（例如 `uMaterial.*` 或 UBO）
- 不要一上来就把所有贴图都做齐，先做参数版，再加贴图版

---

## 5. 与 HDR/色调映射的关系：为什么 PBR “需要”它

PBR 往往会产生更高的亮度范围（尤其是强光与高反射材质）。

如果你仍然在 LDR（0..1）里输出，很容易出现：

- 高光全部变白
- 材质差异被抹平

所以工程上通常是：

- 场景 HDR 渲染（float buffer）
- tone mapping + exposure 输出到屏幕

你不必一次做成完整渲染器，但要把它当作 PBR 的配套地基。

---

## 常见坑

- **把 roughness 当 shininess**：两者不是线性对应，外观会非常怪
- **金属材质还在加漫反射**：会显得“脏/粉”，缺少金属质感
- **颜色空间混乱**：baseColor 贴图按 sRGB，金属度/粗糙度贴图按线性

---

## 小练习

### 练习 1：做一个“材质球参数面板”（先用键盘调参）

- 题目：按键调整 roughness/metallic，并观察高光形态与漫反射变化
- 目标：把 PBR 变成可调试、可理解的系统

### 练习 2：对比 Blinn-Phong 与 PBR 的外观差异

- 题目：同一场景下切换两套 shader，记录哪些材质差异最明显
- 目标：明确 PBR 的价值与代价

---

## 小结

- PBR 的价值在于一致的材质语义与更稳定的能量行为
- roughness 控高光形态，metallic 控漫反射/镜面反射分配
- 实现建议按骨架分步迭代：F → D → G → 能量分配

下一篇：回到目录见 [OpenGL 教程目录](00-目录.md)

