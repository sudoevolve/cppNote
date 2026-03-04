# 05-顶点属性与插值：颜色、varying、Gamma 的直觉

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 从“一个固定颜色的三角形”升级到“每个顶点带属性，片元里自动插值”
- 搞清楚 vertex shader 输出、fragment shader 输入之间发生了什么
- 建立 Gamma/sRGB 的第一直觉：为什么颜色会看起来“发灰/发黑”

## 先决条件

- 已完成 [03-第一个三角形：VAO/VBO/EBO + 最小 Shader](03-第一个三角形-VAO-VBO-EBO-最小Shader.md)

---

## 1. 顶点属性不止位置：你要开始“给顶点贴信息”

你之前的 VBO 只有位置 `(x,y,z)`。

现在我们给每个顶点再加一组颜色 `(r,g,b)`，让 GPU 在三角形内部自动插值。

新的顶点数据：

```cpp
static float vertices[] = {
    // pos.x pos.y pos.z    col.r col.g col.b
     0.0f,  0.6f, 0.0f,     1.0f, 0.2f, 0.2f,
    -0.6f, -0.6f, 0.0f,     0.2f, 1.0f, 0.2f,
     0.6f, -0.6f, 0.0f,     0.2f, 0.2f, 1.0f,
};
```

---

## 2. 重新定义 attribute layout（两个输入槽位）

你需要告诉 GPU：

- location 0：读 3 个 float，当作位置
- location 1：读 3 个 float，当作颜色

```cpp
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);

glBindVertexArray(0);
```

这两行的本质是：

- stride：一整条顶点记录占多少字节
- offset：这个属性从这条记录的第几个字节开始

---

## 3. Shader：用 varying（vertex 输出 → fragment 输入）

顶点 shader：

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main()
{
    vColor = aColor;
    gl_Position = vec4(aPos, 1.0);
}
```

片元 shader：

```glsl
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}
```

你会看到三角形内部出现渐变，这就是插值。

---

## 4. 插值到底发生在哪

你可以用一句话记住：

- 顶点 shader 只在顶点上运行，片元 shader 在像素上运行；两者之间，GPU 会在三角形内部对 `out` 变量做插值，再作为 `in` 提供给片元 shader。

这也是为什么你很少需要“自己在 CPU 上插值”。

---

## 5. Gamma 与 sRGB：先要一个直觉

你很快会遇到两个现象：

- 你把颜色从 0.0 到 1.0 线性变化，但肉眼感觉不是线性的
- 你做混合/插值，结果看起来“发灰”

直觉版结论（先用来避免踩坑）：

- 屏幕显示并不是“线性亮度”，而是接近 Gamma 曲线
- 如果你在“非线性空间”做插值，就会看起来不对

后续涉及纹理时，我们会把 sRGB 作为可落地的工程开关讲清楚（如何让采样与输出在正确空间里发生）。

---

## 常见坑

- **stride/offset 算错**：症状往往是颜色闪烁、随机、或者三角形“爆炸”
- **location 对不上**：shader 里 `layout(location=1)`，但你只配了 0
- **vColor 没连通**：vertex 没写 `out`，fragment 还在读 `in`，要么编译失败要么被优化

---

## 小练习

### 练习 1：加一个“颜色随时间变化”的 uniform

- 题目：在 fragment shader 里加 `uniform float uT;`，用它调制颜色
- 目标：练一次 uniform 的 set/get 与 shader 逻辑联动

### 练习 2：用插值做“圆形渐变”

- 题目：把 `vColor` 换成 `vPos`（把位置传给 fragment），在 fragment 用长度做渐变
- 目标：为后面写 shader 效果打基础（从数据到图像的直觉）

---

## 小结

- 顶点可以携带任意属性：位置只是最基本的一种
- varying 是 vertex→fragment 的数据通道，插值由 GPU 自动完成
- Gamma/sRGB 是“看起来对不对”的底层因素，先建立直觉，后面再落到工程开关

下一篇：见 [06-纹理：采样、wrap/filter、mipmap、sRGB](06-纹理-采样-wrap-filter-mipmap-sRGB.md)

