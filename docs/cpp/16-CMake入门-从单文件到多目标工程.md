# 16-CMake 入门：从单文件到多目标工程

目录页：见 [C++ 小白教程目录](00-目录.md)

## 目标

- 理解 CMake 在工程里到底负责什么、不负责什么
- 会写最小 `CMakeLists.txt`，把单文件程序编译出来
- 会把工程拆成“库 + 可执行文件”两个目标，并设置 C++ 标准版本

## 先决知识（建议）

- 会写一个能编译运行的 C++ 程序（见 01 章）

---

## 1. CMake 是什么：它不是编译器

- 专业名称：CMake（构建系统生成器）
- 类比：翻译的“组织者”：决定用哪些源文件、怎么分组、用什么编译参数
- 作用：生成你平台对应的构建文件（例如 Ninja 文件、Visual Studio 工程、Makefile）
- 规则/坑点：真正把代码编译成可执行文件的是编译器（GCC/Clang/MSVC），CMake 只是把“怎么编译”描述清楚

一句话记忆：

- 编译器：把 `.cpp` 变成 `.exe/.o`
- CMake：把“工程怎么组织、怎么编译”写成一份跨平台配置

---

## 2. 你需要认识的 5 个词

- 源码目录（source dir）：放 `CMakeLists.txt` 和源码的目录
- 构建目录（build dir）：CMake 生成的中间文件与产物目录
- 配置（configure）：CMake 读取 `CMakeLists.txt` 生成构建系统文件
- 构建（build）：调用具体构建工具去编译/链接
- 目标（target）：可执行文件或库（这是 CMake 里最重要的抽象）

推荐习惯：永远“源目录 + 构建目录”分离（out-of-source build）。

---

## 3. 最小工程：单文件 + 一个可执行目标

假设你的目录结构是：

```
project/
  CMakeLists.txt
  main.cpp
```

### 3.1 最小 `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20)

project(demo LANGUAGES CXX)

add_executable(demo main.cpp)
target_compile_features(demo PRIVATE cxx_std_20)
```

直觉解释：

- `project`：给工程起名，并声明用 C++
- `add_executable`：生成一个叫 `demo` 的可执行目标
- `target_compile_features`：对这个目标声明“至少需要 C++20”

### 3.2 最常用的构建命令（跨平台）

在工程根目录执行：

```bash
cmake -S . -B build
cmake --build build
```

这两句分别对应：

- 生成构建系统文件到 `build/`
- 编译并链接

---

## 4. 拆成“库 + 可执行文件”：工程化第一步

当代码不止一个文件时，推荐的拆法是：

- 把可复用的逻辑放进库（`add_library`）
- `main.cpp` 只做“调度”

一个常见结构：

```
project/
  CMakeLists.txt
  app/
    main.cpp
  lib/
    add.h
    add.cpp
```

### 4.1 对应的 `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.20)

project(demo LANGUAGES CXX)

add_library(mylib lib/add.cpp)
target_include_directories(mylib PUBLIC lib)
target_compile_features(mylib PUBLIC cxx_std_20)

add_executable(app app/main.cpp)
target_link_libraries(app PRIVATE mylib)
```

直觉解释：

- `mylib` 是库目标；`PUBLIC` 的 include 目录与编译特性会“传给”依赖它的目标
- `app` 链接 `mylib`，所以 `app` 自动能包含 `lib/add.h`

---

## 5. 多目录工程：用 `add_subdirectory` 管理模块

当工程变大时，把所有 target 都写在一个 `CMakeLists.txt` 里会越来越难维护。更常见的组织方式是“每个模块一个子目录”：

```
project/
  CMakeLists.txt
  app/
    CMakeLists.txt
    main.cpp
  lib/
    CMakeLists.txt
    add.h
    add.cpp
```

根目录 `CMakeLists.txt` 只负责“拼装工程”：

```cmake
cmake_minimum_required(VERSION 3.20)
project(demo LANGUAGES CXX)

add_subdirectory(lib)
add_subdirectory(app)
```

`lib/CMakeLists.txt` 只负责库自身：

```cmake
add_library(mylib add.cpp)
target_include_directories(mylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(mylib PUBLIC cxx_std_20)
```

`app/CMakeLists.txt` 只负责可执行文件：

```cmake
add_executable(app main.cpp)
target_link_libraries(app PRIVATE mylib)
```

这套写法的直觉是：根目录不关心细节，每个模块自己把“我需要什么”描述清楚。

---

## 6. Debug / Release：配置类型与构建目录

你会经常听到“Debug 版本”和“Release 版本”，它们最常见的差异是：

- Debug：便于调试，通常不开最高级优化
- Release：开启更多优化，运行更快，但调试信息可能少

在 CMake 里你要分清两种情况：

- 单配置生成器（常见于 Ninja/Make）：用 `-DCMAKE_BUILD_TYPE=Debug` 选择配置
- 多配置生成器（常见于 Visual Studio）：构建时用 `--config Debug` 选择配置

示例（单配置）：

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

示例（多配置）：

```bash
cmake -S . -B build
cmake --build build --config Debug
```

推荐习惯：

- Debug/Release 用不同 build 目录，互不污染

---

## 7. 编译选项：把警告开起来（更早发现问题）

在学习阶段，把警告打开会让你更快发现“潜在 bug”。更现代的做法是把编译选项写到 target 上：

```cmake
add_executable(app main.cpp)

if (MSVC)
    target_compile_options(app PRIVATE /W4)
else()
    target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
endif()
```

规则/坑点：

- 选项是“编译器相关”的，不同编译器写法不同
- 不要全局乱 `add_compile_options(...)`，优先写在具体 target 上

---

## 8. 常见坑（新手最容易踩）

- 错误：在源目录里直接生成构建文件 → 结果：目录变得很乱 → 正确：用 `-B build` 建独立构建目录
- 错误：全局 `set(CMAKE_CXX_STANDARD 20)` 然后以为都生效 → 结果：多目标时混乱 → 正确：优先用 `target_compile_features`
- 错误：把 include 目录到处 `include_directories(...)` 全局加 → 结果：依赖关系不清楚 → 正确：用 `target_include_directories` 写在目标上
- 错误：新增 `.cpp` 文件但没加进目标 → 结果：链接报“未定义引用” → 正确：把源文件加到 `add_library/add_executable` 里
- 错误：把 Debug/Release 产物混在一个 build 目录里 → 结果：难定位问题、缓存互相影响 → 正确：分开 build 目录或用正确的配置方式
- 错误：在 CMake 里硬编码绝对路径 → 结果：换电脑就炸 → 正确：用相对路径、target include、find_package 等机制

---

## 小练习

### 练习 1：单文件编译

- 输入/输出：写一个 `main.cpp`，输出 `sum=...`
- 约束：用 CMake 配置并构建出可执行文件
- 提示：照抄 3.1 的 `CMakeLists.txt`
- 目标：熟悉 configure 与 build 两步

### 练习 2：库与可执行拆分

- 输入/输出：把 `add(a, b)` 放进库，main 调用并输出
- 约束：必须用 `add_library` + `target_link_libraries`
- 提示：头文件放 `lib/`，并用 `target_include_directories`
- 目标：建立 target 思维

---

## 小结

- CMake 的核心是 target：把“源文件、头文件路径、编译特性、依赖库”组织清楚
- 新手阶段先掌握：`add_executable`、`add_library`、`target_link_libraries`、`target_include_directories`
- 保持 out-of-source build，会让工程干净很多

上一篇：见 [15-C++11 到 C++23：标准演进路线图（你该学什么）](15-C++11到23-标准演进路线图.md)
