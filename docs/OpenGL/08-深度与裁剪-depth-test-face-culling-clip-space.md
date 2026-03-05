# 08-深度与裁剪：depth test、face culling、clip space

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 让 3D 场景“前后关系正确”：开启深度测试并正确清理深度缓冲
- 理解裁剪空间/视锥裁剪的基本规则，知道什么会被丢掉
- 用面剔除减少无效绘制，并理解绕序（winding order）

## 先决条件

- 已完成 [04-坐标与变换：NDC、矩阵、MVP、右手系](04-坐标与变换-NDC-矩阵-MVP-右手系.md)
- 建议已完成 [07-相机：LookAt、轨道相机、第一人称相机](07-相机-LookAt-轨道相机-第一人称相机.md)

---

## 1. 深度缓冲：为什么你会看到“穿模”

没有深度测试时，GPU 会按绘制顺序覆盖像素：

- 后画的覆盖先画的

这在 2D 还好，在 3D 会立刻变成“穿模”和“乱叠”。

深度缓冲（Depth Buffer）存的是每个像素的深度值。深度测试做的事很朴素：

- 只有更近（通常是更小深度）的片元，才允许写入颜色

---

## 2. 最小落地：开启深度测试 + 每帧清 depth

初始化（一次即可）：

```cpp
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);
```

每帧清屏要同时清颜色与深度：

```cpp
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

如果你忘了清深度，典型症状是：

- 画面出现“残影”“撕裂感”或对象突然消失（因为上一帧的深度还在挡着）

---

## 3. Clip Space 与裁剪：为什么物体会被“截掉”

你在顶点 shader 输出的是 `gl_Position`（裁剪空间坐标）。

GPU 会对它做两件关键事：

1. 视锥裁剪：超出一定范围的几何会被裁剪/丢弃
2. 齐次除法：得到 NDC（`xyz / w`）

你不需要背所有规则，但要有一个工程判断：

- “我的物体是否落在相机的视锥里？”

最容易导致“突然全没了”的三个原因：

- 相机方向不对（View 错）
- near/far 设错（Projection 错）
- Model 把物体挪走了（Model 错）

---

## 4. 面剔除（Face Culling）：不画背面

当你的模型是封闭的（比如立方体），背面通常永远不可见。

开启剔除：

```cpp
glEnable(GL_CULL_FACE);
glCullFace(GL_BACK);
glFrontFace(GL_CCW);
```

`glFrontFace(GL_CCW)` 的意思是：

- 默认把“顶点按逆时针绕序定义的三角形”当作正面

如果你发现开了剔除之后模型“看不到了”，先别怀疑人生，通常是：

- 模型绕序反了（CW/CCW）
- 你的变换做了负缩放（镜像）导致绕序翻转

---

## 5. 深度精度与 z-fighting：早晚会遇到

当两个面非常接近时，你可能看到闪烁（z-fighting）。

最常见的根因是深度精度不足，常见解决顺序：

- 先把 near 变大、far 变小（这是最有效、最简单的）
- 再考虑 polygon offset（后面做贴花/阴影会用到）

---

## 6. 透明与混合（Blending）：从“能画”到“看起来对”

透明不是“把 alpha 写成 0.5”，它是一整套规则：

- 颜色怎么混合（blend）
- 深度怎么处理（depth write / depth test）
- 绘制顺序怎么安排（排序）

### 6.1 开启混合：最常见的 alpha 混合

初始化（一次即可）：

```cpp
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

如果你的片元输出 `vec4(rgb, a)`，那么最终颜色约等于：

- out = src * a + dst * (1-a)

### 6.2 深度与透明：最容易踩坑的组合

不透明物体的习惯是：

- depth test 开
- depth write 开（默认）
- 顺序随意（通常前到后更省像素）

透明物体通常更稳的规则是：

- depth test 开（仍然需要遮挡）
- depth write 关（避免“透明物体把后面的透明物体挡死”）
- 按相机距离从远到近画（保证混合正确）

绘制透明物体前（每帧，开始画透明队列之前）：

```cpp
glDepthMask(GL_FALSE);
```

画完透明队列后恢复：

```cpp
glDepthMask(GL_TRUE);
```

### 6.3 预乘 Alpha（Premultiplied Alpha）：UI/贴图常见语义

有些贴图数据是“预乘 alpha”的：RGB 已经乘过 A。

这时对应的混合因子通常是：

```cpp
glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
```

你可以用一个工程口径记住区别：

- 非预乘：src.rgb 还没乘 a，用 `SRC_ALPHA`
- 预乘：src.rgb 已经乘 a，用 `ONE`

---

## 7. 模板缓冲（Stencil）：用一个 8-bit mask 做“分区域渲染”

模板缓冲的直觉是：每个像素除了 color/depth，还有一个小小的整数标签（常见 8-bit）。

你可以用它做很多“效果技巧”：

- 描边/高亮（先写模板，再只在模板外画一圈）
- 镜子/洞口/门户（只在模板区域画某个 pass）
- HUD/遮罩（限制绘制范围）

### 7.1 开启模板测试与基本用法

初始化（一次即可）：

```cpp
glEnable(GL_STENCIL_TEST);
```

最小配置（概念上）：

- `glStencilFunc`：通过条件（参考值 vs 当前 stencil 值）
- `glStencilOp`：通过/失败时怎么改 stencil 值

示例：先把某个物体所在像素的 stencil 写成 1：

```cpp
glStencilMask(0xFF);
glStencilFunc(GL_ALWAYS, 1, 0xFF);
glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
```

然后再画第二遍，只允许在 stencil != 1 的地方画（用于描边外扩）：

```cpp
glStencilMask(0x00);
glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
```

画完恢复写入：

```cpp
glStencilMask(0xFF);
glStencilFunc(GL_ALWAYS, 0, 0xFF);
```

### 7.2 记住“清理”与 FBO 的关系

如果你启用了 stencil buffer，每帧清理要把 stencil 也清掉：

```cpp
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
```

离屏渲染时，如果你的 FBO 没有 stencil 附件，你再怎么 `glEnable(GL_STENCIL_TEST)` 也不会按你预期工作。

---

## 常见坑

- **只清了颜色没清深度**：画面会越来越怪
- **near/far 乱填**：深度问题会在你以为“还没学到那一步”时就出现
- **剔除开启后全消失**：先检查绕序，再检查是否有负缩放
- **透明物体“越画越黑/越画越怪”**：检查混合因子、深度写入、以及是否按远到近排序
- **Stencil 没效果**：检查是否有 stencil buffer，是否清了 stencil，是否禁用了 stencil 写入（mask）

---

## 小练习

### 练习 1：做一个会旋转的立方体并验证深度测试

- 题目：画一个立方体（12 个三角形），旋转后观察遮挡是否正确
- 目标：让深度测试的价值变成肉眼可见

### 练习 2：故意制造 z-fighting

- 题目：画两个几乎重叠的平面，调整 near/far 观察闪烁变化
- 目标：把“深度精度”从概念变成现象

### 练习 3：做一个透明 Billboard（树叶/玻璃）并排序

- 题目：画多张带 alpha 的四边形，按相机距离从远到近绘制
- 目标：把“透明 = 混合 + 深度策略 + 顺序”跑通

### 练习 4：做一个描边效果（Stencil）

- 题目：第一遍写 stencil=1，第二遍略微放大模型，只在 stencil!=1 的区域画纯色边
- 目标：掌握 stencil 的最常用工程套路

---

## 小结

- 3D 必须开启深度测试，并且每帧清 depth
- 裁剪/视锥问题优先从 Model/View/Projection 去排查
- 面剔除是简单有效的性能手段，但要理解绕序与负缩放
- 透明渲染要同时管住：混合、深度写入、绘制顺序
- Stencil 是“分区域渲染”的强力工具，描边/遮罩类效果都离不开它

下一篇：见 [09-模型与网格：OBJ/GLTF（简化版）加载、法线、切线](09-模型与网格-OBJ-GLTF加载-法线-切线.md)
