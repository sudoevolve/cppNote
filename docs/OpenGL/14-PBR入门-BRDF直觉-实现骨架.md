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

## 6. 环境光：为什么“只打一盏直射光”的 PBR 会显得假

微表面模型把高光/反射描述得更真实了，但如果场景里只有一盏“直射光”，你会很容易看到：

- 阴影里完全死黑（缺少环境光的能量）
- 金属材质在非直射区域几乎没信息（因为金属主要靠镜面反射）

更接近真实的做法是：让材质从环境中“拿到”漫反射与镜面反射能量。

这就是 IBL（Image Based Lighting）的工程动机：

- 环境贴图提供全方向的入射光
- 通过预计算把积分变成一次贴图采样（或少量采样）

---

## 7. Cubemap：把环境表示成“六张面”

在实时渲染里，环境贴图最常见的运行时表示是 cubemap：

- 采样接口天然是方向向量（反射向量、法线方向等）
- 不需要再把方向映射到球面 UV

### 7.1 Skybox（直觉版）

最小 skybox 其实就是：

- 一个立方体（或全屏三角形技巧）
- vertex shader 只负责提供方向
- fragment shader 采样 cubemap

你不需要一次写完所有细节，但要记住两条工程纪律：

- skybox 通常最后画（或者深度写入关）
- view 矩阵去掉平移（让天空“无限远”）

---

## 8. IBL 的最小链路（Split-Sum）：Diffuse + Specular

工程上最常用的一套 IBL 近似是 split-sum：

- 漫反射环境：irradiance cubemap
- 镜面环境：prefiltered cubemap（按 roughness 有不同 mip）
- 菲涅尔/几何项的积分：BRDF LUT（2D 查表）

最终在 shader 里把“直射光”与“环境光”相加。

### 8.1 漫反射 IBL：Irradiance Map

直觉：对环境做一次低频卷积，把“这个法线方向上看到的平均入射光”变成一张 cubemap。

使用时：

```glsl
vec3 irradiance = texture(uIrradianceMap, N).rgb;
vec3 diffuseIBL = irradiance * baseColor;
```

### 8.2 镜面 IBL：Prefilter Map + BRDF LUT

镜面反射依赖 roughness：越粗糙，高光越模糊，来自更宽的方向范围。

工程近似通常是：

- 预计算：对环境做 GGX 相关的预滤波，得到一张带 mip 的 cubemap
- 运行时：用 roughness 选 mip，用 BRDF LUT 做能量分配修正

概念用法：

```glsl
vec3 R = reflect(-V, N);
float mip = roughness * float(uPrefilterMaxMip);
vec3 prefiltered = textureLod(uPrefilterMap, R, mip).rgb;

vec2 brdf = texture(uBrdfLut, vec2(max(dot(N, V), 0.0), roughness)).rg;
vec3 specularIBL = prefiltered * (F0 * brdf.x + brdf.y);
```

### 8.3 最小工程步骤（不纠结库选型）

1. 准备一张 HDR 环境图（常见是 equirectangular）
2. 转换到 environment cubemap
3. 从 environment 生成 irradiance cubemap（低频）
4. 从 environment 生成 prefilter cubemap（带 mip）
5. 离线/启动时生成 BRDF LUT（2D 纹理）
6. 渲染时：直射光 + IBL（diffuse + specular）

你不必一次把 1~6 都做成“完善工具链”，但建议至少把 2/3/4/5 跑成可复用的离屏 pass。

---

## 9. 小型渲染器落地：模块与 Pass 的最小交付

到这一章为止，你已经拥有“渲染器”而不是“示例代码”的大部分拼图。为了让它能持续迭代，建议把工程目标收敛成三个层次：模块、渲染队列、渲染 pass。

### 9.1 模块最小拆分（不追求完美，但要可扩展）

你不需要一上来就做引擎，但建议至少把职责拆开：

- App：窗口、输入、时间、主循环
- Renderer：渲染入口与渲染队列（不透明/透明/后处理）
- Resources：Shader、Texture、Mesh（含生命周期）
- Scene：Transform、Camera、Light、Material（CPU 侧数据模型）

如果你希望目录结构一眼能看出“这是渲染器”，可以参考这个骨架：

```
src/
  app/
  renderer/
    passes/
  resources/
  scene/
shaders/
assets/
```

### 9.2 Pass 最小集合（对应你前面章节学到的能力）

一个“小而完整”的 forward 渲染器，最小 pass 集合通常是：

- Shadow Pass：渲染到 depth texture（11 章）
- Forward Pass：主场景渲染（含不透明/透明队列）（08/10/13 章）
- Skybox Pass：环境贴图可视化（本章）
- Post Process Pass：tone mapping/曝光/可选 FXAA（12 章）

如果你要把 IBL 做到可用，再补一个“启动时预计算”的离屏链路：

- IBL Precompute：equirect → env cubemap → irradiance → prefilter → BRDF LUT（本章 8.3）

### 9.3 里程碑：用“可验收输出”驱动迭代

把渲染器迭代拆成可验收的里程碑，调试会轻松很多：

1. 画三角形（03）
2. 画立方体 + 相机（04/07/08）
3. 贴图 + sRGB（06）
4. 网格加载（09）
5. 光照（10）
6. 阴影（11）
7. 离屏 + HDR + tone mapping（12）
8. 统计/抓帧/渲染队列（13）
9. PBR（参数版）+ Skybox（14）
10. IBL（可选进阶）：irradiance/prefilter/BRDF LUT（14）

这套里程碑的意义是：每一步都能在 VS 里断点调试、在 RenderDoc 里抓一帧验证，不靠猜。

---

## 常见坑

- **把 roughness 当 shininess**：两者不是线性对应，外观会非常怪
- **金属材质还在加漫反射**：会显得“脏/粉”，缺少金属质感
- **颜色空间混乱**：baseColor 贴图按 sRGB，金属度/粗糙度贴图按线性
- **没有环境光就做 PBR**：阴影死黑、金属没信息；至少加一个环境项或 IBL
- **prefilter 没有 mip**：roughness 变化不明显或全糊；预滤波链路要生成 mip 并按 roughness 选 mip

---

## 小练习

### 练习 1：做一个“材质球参数面板”（先用键盘调参）

- 题目：按键调整 roughness/metallic，并观察高光形态与漫反射变化
- 目标：把 PBR 变成可调试、可理解的系统

### 练习 2：对比 Blinn-Phong 与 PBR 的外观差异

- 题目：同一场景下切换两套 shader，记录哪些材质差异最明显
- 目标：明确 PBR 的价值与代价

### 练习 3：加一个最小 Skybox

- 题目：加载/生成一个 cubemap 并画 skybox
- 目标：建立“环境贴图”的运行时通路

---

## 小结

- PBR 的价值在于一致的材质语义与更稳定的能量行为
- roughness 控高光形态，metallic 控漫反射/镜面反射分配
- 实现建议按骨架分步迭代：F → D → G → 能量分配
- 真实感的关键增量来自环境光：skybox + IBL 会让材质立刻成立

下一篇：回到目录见 [OpenGL 教程目录](00-目录.md)
