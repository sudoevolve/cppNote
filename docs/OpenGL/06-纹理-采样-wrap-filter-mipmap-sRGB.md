# 06-纹理：采样、wrap/filter、mipmap、sRGB

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 从“纯色三角形”升级到“贴图三角形”
- 搞清楚纹理坐标、采样、wrap/filter 的作用与典型现象
- 用 mipmap 解决远处闪烁/噪点，用 sRGB 建立颜色空间的正确直觉

## 先决条件

- 已完成 [05-顶点属性与插值：颜色、varying、Gamma 的直觉](05-顶点属性与插值-颜色-varying-Gamma直觉.md)

---

## 1. 纹理本质：一个可被 shader 采样的二维数组

纹理不是“图片文件”。图片文件需要你在 CPU 侧解码成像素数组，然后上传到 GPU 的纹理对象。

本章先不纠结解码库选型（stb_image 等），我们先把 OpenGL 侧链路讲清楚：

- 创建 texture 对象
- 上传像素数据
- 设置采样参数
- 在 fragment shader 里采样 `sampler2D`

---

## 2. 顶点数据：增加纹理坐标（UV）

给每个顶点加一个 `(u,v)`：

```cpp
static float vertices[] = {
    // pos.x pos.y pos.z     uv.u  uv.v
     0.0f,  0.6f, 0.0f,      0.5f, 1.0f,
    -0.6f, -0.6f, 0.0f,      0.0f, 0.0f,
     0.6f, -0.6f, 0.0f,      1.0f, 0.0f,
};
```

attribute layout：

```cpp
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
```

---

## 3. Shader：把 UV 传到 fragment，再采样

顶点 shader：

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
out vec2 vUV;
void main()
{
    vUV = aUV;
    gl_Position = vec4(aPos, 1.0);
}
```

片元 shader：

```glsl
#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D uTex;
void main()
{
    FragColor = texture(uTex, vUV);
}
```

---

## 4. 纹理对象：创建、上传、绑定到纹理单元

OpenGL 的纹理采样由两部分组成：

- texture 对象本身（数据与参数）
- texture unit（shader 里的 sampler 指向哪个 unit）

示例（像素数据用占位变量表示）：

```cpp
GLuint tex = 0;
glGenTextures(1, &tex);
glBindTexture(GL_TEXTURE_2D, tex);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, pixels);
glGenerateMipmap(GL_TEXTURE_2D);

glBindTexture(GL_TEXTURE_2D, 0);
```

绘制前绑定：

```cpp
glUseProgram(program);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, tex);
glUniform1i(glGetUniformLocation(program, "uTex"), 0);
```

---

## 5. wrap/filter：你会“立刻看到”的差别

### 5.1 wrap（超出 0..1 的坐标怎么处理）

- `GL_REPEAT`：平铺
- `GL_CLAMP_TO_EDGE`：夹到边缘（最常用来避免边缘采样出线）

### 5.2 filter（纹理采样如何从像素到连续值）

- `GL_NEAREST`：最近邻（像素风，放大很块状）
- `GL_LINEAR`：双线性（放大更平滑）

当物体远离相机时，你会出现“闪烁/噪点”：这不是你的 UV 错了，而是采样频率不匹配，这就是 mipmap 的用武之地。

---

## 6. mipmap：远处不再闪

核心直觉：

- 远处一个屏幕像素对应很多纹理像素时，直接采样会混叠（aliasing）
- mipmap 给你提供了“更低分辨率的版本”，采样时会选更合适的层级

最小落地：

- 上传完纹理后 `glGenerateMipmap(GL_TEXTURE_2D)`
- min filter 用 `GL_LINEAR_MIPMAP_LINEAR`（三线性）

---

## 7. sRGB：把“发灰/发黑”变成可控开关

直觉结论：

- “颜色贴图”（albedo/baseColor）通常应被当作 sRGB
- “数据贴图”（法线、粗糙度、金属度）必须是线性的

OpenGL 侧的常见做法是：对“颜色贴图”用 sRGB 内部格式，让 GPU 在采样时自动把 sRGB 转成线性，再在输出到屏幕时做正确的转换（具体启用方式会在后处理/HDR 章节统一讲清）。

本章你先记住一句工程口径：

- 不要在线性空间和 sRGB 空间里“混着插值/混色”，否则肉眼会觉得不对。

---

## 常见坑

- **UV 上下颠倒**：图片坐标系与 OpenGL UV 习惯不同，现象是贴图倒过来
- **没绑定到正确 unit**：`glActiveTexture(GL_TEXTURE0 + n)` 与 `sampler = n` 对不上
- **min filter 用了 mipmap，但没生成 mipmap**：采样结果可能是黑的或未定义
- **边缘出现“黑线/色线”**：常见原因是 wrap/filter 与图片边缘像素，优先试 `GL_CLAMP_TO_EDGE`

---

## 小练习

### 练习 1：做一个“UV 超出 0..1 的平铺实验”

- 题目：把 UV 乘以 4，再分别试 `REPEAT` 与 `CLAMP_TO_EDGE`
- 目标：用肉眼理解 wrap 行为

### 练习 2：对比 mipmap 的效果

- 题目：把 min filter 分别设为 `GL_LINEAR` 与 `GL_LINEAR_MIPMAP_LINEAR`
- 目标：理解“远处闪烁”不是你写错，而是采样理论问题

---

## 小结

- 纹理 = 数据 + 参数；采样 = texture 对象 + texture unit
- wrap/filter 决定“超出范围”与“放大/缩小”的观感
- mipmap 是解决远处混叠的基础工程方案
- sRGB 是“颜色正确”的地基，先建立分类直觉：颜色贴图 vs 数据贴图

下一篇：见 [07-相机：LookAt、轨道相机、第一人称相机](07-相机-LookAt-轨道相机-第一人称相机.md)

