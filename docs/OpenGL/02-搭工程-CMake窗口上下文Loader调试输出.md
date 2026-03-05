# 02-搭工程：CMake + 窗口 + OpenGL Loader + 调试输出

目录页：见 [OpenGL 教程目录](00-目录.md)

## 目标

- 搭出一个“稳定可复用”的最小工程骨架：创建窗口 → 创建上下文 → 加载 OpenGL 函数 → 帧循环
- 把黑屏变成“可观测问题”：打印版本信息、打开调试输出、确保 resize 不出错
- 为下一章画三角形准备好：清屏、viewport、输入事件的最小支持
- 把工程落地到 VS：用 vcpkg 管依赖，用 VS/CMake 进行配置与调试

## 先决条件

- 已阅读 [01-概念：GPU、驱动、上下文、渲染管线](01-概念-GPU驱动上下文渲染管线.md)
- 能用 CMake 构建一个 C++ 可执行程序

---

## 1. 选型：窗口库 + Loader（主线）

本教程主线使用：

- 窗口/上下文：GLFW（跨平台、资料多）
- OpenGL Loader：GLAD（按版本生成，常见于教学项目）
- 数学库：GLM（先不引入，本章只把工程跑通）

你完全可以替换为 SDL / Qt，但建议先跟主线跑通，再换。

---

## 2. 最小工程结构（建议）

先把目录定下来，后面每一章都在同一骨架上迭代：

```
OpenGLPlayground/
  CMakeLists.txt
  src/
    main.cpp
  shaders/
    (后续再加)
```

如果你的终点是“小型渲染器”，建议从一开始就把骨架稍微工程化一点（后续章节可以逐步填充，不要求现在就写满）：

```
OpenGLPlayground/
  CMakeLists.txt
  src/
    app/
      main.cpp
    renderer/
      (后续逐步拆：Shader/Mesh/Texture/Camera/Material/Pass)
  shaders/
  assets/
```

---

## 3. 获取依赖：两种常用方式

### 3.1 方式 A：vcpkg（推荐给 Windows 初学者）

- 优点：省心、版本可控、CMake 集成成熟
- 缺点：需要先装 vcpkg

安装 GLFW、GLM（后面会用）：

```
vcpkg install glfw3 glm
```

然后在 CMake 里通过 toolchain 使用 vcpkg（示意）：

```
-DCMAKE_TOOLCHAIN_FILE=.../vcpkg/scripts/buildsystems/vcpkg.cmake
```

如果你的目标是“Visual Studio 里一键配置/构建/调试”，更推荐的工程口径是：

- vcpkg 走 manifest（`vcpkg.json`）或至少固定 triplet（例如 `x64-windows`）
- CMake 用 Presets 或 VS 的 CMake 集成来选择工具链与 triplet

概念示例（CMake configure 时传入）：

```
-DCMAKE_TOOLCHAIN_FILE=.../vcpkg/scripts/buildsystems/vcpkg.cmake
-DVCPKG_TARGET_TRIPLET=x64-windows
```

这样你在 VS 里切 Debug/Release、切 x64 架构时，依赖也能跟着稳定复用。

GLAD 通常不是 vcpkg 主线方案（也可以装），更常见是“直接把生成的 glad 源码放进工程”。本教程在“讲清楚原理”的前提下，建议你用自己生成的 glad。

### 3.2 方式 B：CMake FetchContent（不依赖包管理器）

- 优点：工程自给自足
- 缺点：首次配置会下载依赖，网络不稳定时会卡

GLFW 很适合 FetchContent；GLAD 仍建议用预生成源码直接加入项目。

---

## 4. CMakeLists.txt（最小可用骨架）

下面给一份“骨架级”配置：能编译 main.cpp，并链接 GLFW。GLAD 部分以“你把 glad.c/glad.h 放进工程”为前提。

```cmake
cmake_minimum_required(VERSION 3.20)
project(OpenGLPlayground LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(OpenGLPlayground
    src/main.cpp
    external/glad/src/glad.c
)

target_include_directories(OpenGLPlayground PRIVATE
    external/glad/include
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(OpenGLPlayground PRIVATE glfw)
```

如果你用的是非 vcpkg 的 GLFW（比如自己编译/FetchContent），`find_package` 的写法会不同，但这不影响本章理解：目标是让“窗口 + 上下文”能稳定创建。

---

## 5. main.cpp：创建窗口、创建上下文、加载函数、进入帧循环

先把“能清屏并刷新”作为交付标准。下面代码有几个关键点：

- 必须先创建窗口/上下文，再加载 GLAD
- 必须设置 viewport，并在 resize 时更新
- 一定要有帧循环：poll events + swap buffers

```cpp
#include <cstdio>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

static void APIENTRY glDebugCallback(
    GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam)
{
    (void)source; (void)type; (void)id; (void)severity; (void)length; (void)userParam;
    std::fprintf(stderr, "[GL] %s\n", message);
}

int main()
{
    if (!glfwInit()) {
        std::fprintf(stderr, "glfwInit failed\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(960, 540, "OpenGL Playground", nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "glfwCreateWindow failed\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::fprintf(stderr, "gladLoadGLLoader failed\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::printf("OpenGL:  %s\n", glGetString(GL_VERSION));
    std::printf("GLSL:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    std::printf("Vendor:  %s\n", glGetString(GL_VENDOR));
    std::printf("Renderer:%s\n", glGetString(GL_RENDERER));

    int fbw = 0, fbh = 0;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);

    if (GLAD_GL_KHR_debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugCallback, nullptr);
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.10f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

跑起来的期望结果：

- 窗口出现
- 背景是深灰色
- 控制台打印出 OpenGL/GLSL/vendor/renderer

---

## 6. 常见坑（本章最容易踩）

- **先加载 GLAD 再创建上下文**：高概率崩溃或所有函数指针为空
- **没处理 viewport / resize**：窗口拉伸后你画的东西会“看不见/被裁掉”
- **Debug 输出没任何信息**：不一定是错；有些驱动默认不报很多，先把它当“辅助”而不是“真理”

---

## 小练习

### 练习 1：让清屏颜色随时间变化

- 题目：用 `glfwGetTime()` 改变 `glClearColor`，让背景缓慢变色
- 目标：验证帧循环与时间源没问题

### 练习 2：加一个按键关闭窗口

- 题目：按 ESC 时调用 `glfwSetWindowShouldClose(window, GLFW_TRUE)`
- 目标：把输入事件引入工程骨架（下一章会用）

---

## 小结

- 你的“第一性目标”不是画东西，而是把上下文与 loader 顺序固定下来
- 版本信息 + viewport + debug 输出是“可观测性”的最小三件套
- 到这里，你已经拥有画三角形所需的运行时环境

下一篇：见 [03-第一个三角形：VAO/VBO/EBO + 最小 Shader](03-第一个三角形-VAO-VBO-EBO-最小Shader.md)

