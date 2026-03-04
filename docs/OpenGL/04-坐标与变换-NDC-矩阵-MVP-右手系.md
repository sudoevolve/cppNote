# 04-坐标与变换：NDC、矩阵、MVP、右手系

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 把“我画了但看不到”从 API 问题变成“坐标系问题”
- 搞清楚 NDC / Clip Space / Viewport 之间的关系
- 用 MVP（Model/View/Projection）让物体旋转、移动，并保持可控

## 先决条件

- 已完成 [03-第一个三角形：VAO/VBO/EBO + 最小 Shader](03-第一个三角形-VAO-VBO-EBO-最小Shader.md)

---

## 1. 你现在画的是 NDC：最方便，也最容易误导

在第 03 章，我们直接把顶点写成 `[-1, 1]` 的坐标，这叫 NDC。

它的好处是：不用管相机与投影，立刻能看到结果。

它的坏处是：你会误以为“世界就是 [-1,1]”，然后在进入 3D 时集体崩溃。

---

## 2. 现代管线里坐标是怎么走的

你需要记住的顺序只有一条链：

1. **模型空间（Model Space）**：物体自己的局部坐标
2. **世界空间（World Space）**：把物体放到世界里（平移/旋转/缩放）
3. **视图空间（View Space）**：把世界“挪到相机前面”
4. **裁剪空间（Clip Space）**：投影之后的齐次坐标（`gl_Position`）
5. **NDC**：GPU 做齐次除法（`clip.xyz / clip.w`）得到
6. **屏幕空间（Viewport）**：NDC 映射到窗口像素

你在 shader 里写 `gl_Position = vec4(...)`，写的是 **裁剪空间**。

---

## 3. MVP 的意义：把“位置变化”变成矩阵乘法

最常见的写法是：

```
clipPos = Projection * View * Model * localPos
```

对应到顶点 shader：

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 uMVP;
void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
}
```

---

## 4. 引入 GLM：只用到哪讲到哪

你可以用 GLM 来避免自己手写矩阵（主线推荐）。核心思路：

- Model：物体旋转/平移/缩放
- View：相机（先用 lookAt）
- Projection：透视投影（先用 perspective）

示例（仅展示关键片段）：

```cpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 model(1.0f);
model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 0, 1));

glm::mat4 view(1.0f);
view = glm::translate(view, glm::vec3(0, 0, -2.0f));

glm::mat4 proj = glm::perspective(glm::radians(60.0f),
                                  (float)fbw / (float)fbh,
                                  0.1f, 100.0f);

glm::mat4 mvp = proj * view * model;

int loc = glGetUniformLocation(program, "uMVP");
glUseProgram(program);
glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));
```

要点：

- `proj` 里 near/far 不要随便填（太极端会影响深度精度）
- `view` 的 `-2.0f` 是把相机“往后拉”，让物体落在视锥里

---

## 5. 右手系、相机朝向与“为什么是 -Z”

在很多现代 OpenGL 教程里，会默认：

- 相机看向 **-Z 方向**
- 物体默认在原点

所以经常看到 `view = translate(0,0,-2)` 这种写法：它把世界往 -Z 方向挪，相当于相机往 +Z 方向退。

不要死记：你只要能回答这句就够了：

- “我把物体放进了相机的视锥里了吗？”

---

## 6. 常见坑

- **投影矩阵忘了用宽高比**：图像会被拉伸
- **near 太小、far 太大**：深度精度变差，后面会出现 z-fighting
- **uniform 名字不对/没找到**：`glGetUniformLocation` 返回 -1 时，要先排查 shader 是否真的用到了这个 uniform（未使用可能被优化掉）

---

## 小练习

### 练习 1：让三角形绕自身旋转并上下浮动

- 题目：在 Model 上叠加一个 `translate(sin(t)*0.2)` 再旋转
- 目标：体会“变换顺序”对结果的影响

### 练习 2：改成透视投影的 3D 三角形

- 题目：把某个顶点的 z 改成 0.5，让它在 3D 空间里不共面
- 目标：确认你真的进入了 3D 的坐标思维

---

## 小结

- 你在 shader 里输出的是裁剪空间；NDC 是之后自动得到的
- MVP 是把“放置/相机/投影”系统化的最小方法
- 坐标问题优先分层排查：模型 → 视图 → 投影 → viewport

下一篇：见 [05-顶点属性与插值：颜色、varying、Gamma 的直觉](05-顶点属性与插值-颜色-varying-Gamma直觉.md)

