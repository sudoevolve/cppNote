# 03-第一个三角形：VAO/VBO/EBO + 最小 Shader

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 用现代 OpenGL 画出第一个三角形（不是固定管线）
- 搞清楚 VAO/VBO/EBO 各自解决什么问题
- 建立最小 shader 编译/链接/报错输出流程，告别“黑屏没日志”

## 先决条件

- 已完成 [02-搭工程：CMake + 窗口 + OpenGL Loader + 调试输出](02-搭工程-CMake窗口上下文Loader调试输出.md)

---

## 1. 三角形最小闭环需要什么

你需要的就四样东西：

- 一段顶点数据（3 个点）
- 一个顶点着色器（把顶点位置输出到裁剪空间）
- 一个片元着色器（输出一个颜色）
- 一次 draw call（告诉 GPU 开始画）

资源与状态的角色分工：

- VBO：存顶点数据（GPU 端）
- VAO：存“如何读顶点数据”的配置（attribute layout）
- EBO（可选）：索引缓冲，复用顶点（本章顺手带上，后面会常用）

---

## 2. 顶点数据：先用 NDC（最省事）

NDC（Normalized Device Coordinates）范围是 `[-1, 1]`，直接写进去就能看到三角形：

```cpp
static float vertices[] = {
    // pos.x, pos.y, pos.z
     0.0f,  0.6f, 0.0f,
    -0.6f, -0.6f, 0.0f,
     0.6f, -0.6f, 0.0f,
};

static unsigned int indices[] = {
    0, 1, 2
};
```

---

## 3. 最小 Shader：先写成字符串（后面再工程化）

```cpp
static const char* kVertexShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos, 1.0);
}
)";

static const char* kFragmentShaderSrc = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.30, 0.72, 0.98, 1.0);
}
)";
```

如果你后面遇到“shader 编译失败但你没看到错误”，基本就是日志没打印或没检查编译状态。

---

## 4. 编译与链接：最小可用的错误输出

```cpp
static GLuint compileShader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048] = {};
        glGetShaderInfoLog(shader, (GLsizei)sizeof(log), nullptr, log);
        std::fprintf(stderr, "Shader compile failed: %s\n", log);
    }

    return shader;
}

static GLuint linkProgram(GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048] = {};
        glGetProgramInfoLog(program, (GLsizei)sizeof(log), nullptr, log);
        std::fprintf(stderr, "Program link failed: %s\n", log);
    }

    return program;
}
```

---

## 5. VAO/VBO/EBO：把“数据”和“怎么读”绑在一起

```cpp
GLuint vao = 0, vbo = 0, ebo = 0;
glGenVertexArrays(1, &vao);
glGenBuffers(1, &vbo);
glGenBuffers(1, &ebo);

glBindVertexArray(vao);

glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glBindVertexArray(0);
```

你可以先用一句话记住 VAO 的意义：

- VAO 让你后续绘制时只需要 `glBindVertexArray(vao)`，就能恢复“顶点输入的所有配置”。

---

## 6. 在帧循环里绘制

把下面内容插入到 [02](02-搭工程-CMake窗口上下文Loader调试输出.md) 的帧循环里（清屏后，swap 前）：

```cpp
static GLuint program = 0;
if (program == 0) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, kVertexShaderSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragmentShaderSrc);
    program = linkProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

glUseProgram(program);
glBindVertexArray(vao);
glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
glBindVertexArray(0);
```

期望结果：一个青蓝色三角形出现在窗口中央附近。

---

## 7. 常见坑（这章的“黑屏大户”）

- **shader `#version` 不匹配**：你创建的是 3.3 Core，但 shader 不是 `#version 330 core`
- **忘了 `glUseProgram`**：你以为“绑定了 VAO 就会画”，但没有 program 输出就没有像素
- **attribute layout 写错**：`size/stride/offset` 一错，顶点位置就会读乱
- **EBO 绑定时机错**：EBO 绑定是 VAO 状态的一部分，必须在 `glBindVertexArray(vao)` 之后绑定

---

## 小练习

### 练习 1：按键切换线框/填充

- 题目：按下 `W` 切换 `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE/GL_FILL)`
- 目标：建立“OpenGL 是状态机”的直觉

### 练习 2：把颜色改成由顶点输入决定

- 题目：给每个顶点加一个 RGB 属性，shader 里插值到片元
- 目标：为下一章的“顶点属性与插值”预热

---

## 小结

- 三角形最小闭环：顶点数据 + 最小 shader + VAO/VBO/EBO + draw call
- VAO 负责“记住顶点输入配置”，VBO/EBO 负责“存数据”
- shader 错误日志一定要能看到，否则你调试成本会指数级上升

下一篇：见 [04-坐标与变换：NDC、矩阵、MVP、右手系](04-坐标与变换-NDC-矩阵-MVP-右手系.md)

