# 08-资源与工程组织：qrc、qmldir、QML module、CMake

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 认识 QML 工程里“文件在哪、怎么被找到、怎么被打包”的基本规则
- 理解 `qrc`、`qmldir`、QML module 的区别与组合方式
- 知道 Qt 6 + CMake 下常见的 QML 集成方式（概念模板）

---

## 1. QML 文件是怎么被找到的

QML 引擎能加载 QML，通常来自三类来源：

1. **资源系统（qrc）**：编译进可执行文件（推荐用于发布）
2. **文件系统路径**：开发阶段便捷，但发布时要管理部署
3. **QML module**：通过 `import Xxx 1.0` 导入（工程化最强）

当你写：

```qml
import MyApp 1.0
MyType { }
```

背后就依赖“模块定义”和“类型可见性”。

---

## 2. qrc：把 QML/图片资源编译进程序

优点：

- 发布简单：资源都在程序里
- 路径稳定：`qrc:/...`

典型加载方式：

```cpp
engine.load(QUrl(u"qrc:/main.qml"_qs));
```

QML 里引用图片：

```qml
Image { source: "qrc:/assets/logo.png" }
```

---

## 3. qmldir：给“文件夹”定义一个可 import 的模块（传统方式）

`qmldir` 文件通常放在一个目录下，用来描述：

- 模块名
- 哪些 QML 类型由哪些文件提供

概念例子（仅展示格式）：

```
module MyWidgets
MyBadge 1.0 MyBadge.qml
SplitText 1.0 SplitText.qml
```

然后在 QML 里：

```qml
import MyWidgets 1.0
MyBadge { }
```

---

## 4. Qt 6 + CMake 的主流方式：QML module（qt_add_qml_module）

在 Qt 6 里更推荐用 QML module 的方式组织（能同时管 qrc、导入、类型注册等）。

概念模板（示意，不保证与你工程的文件名一致）：

```cmake
qt_add_qml_module(app
    URI MyApp
    VERSION 1.0
    QML_FILES
        main.qml
        components/Badge.qml
        components/SplitText.qml
    RESOURCES
        assets/logo.png
)
```

这样你可以：

- 用 `import MyApp 1.0` 导入模块内类型
- 同时把 QML/资源打包到 qrc（通常由工具链处理）

---

## 5. import 路径与“同目录可用”的误解

很多初学者会以为：

- “QML 文件放在项目里就能用”

实际上可用性取决于：

- 是否在同目录（同目录可直接按类型名引用）
- 是否在引擎 import path 下
- 是否属于某个 QML module（通过 import 引入）

建议项目一旦开始做组件库，就尽量走 module 化，而不是靠“目录碰巧在一起”。

---

## 6. 推荐的目录结构（不强制）

思路是把“页面”和“组件库”分开：

- `qml/pages/`：页面级 UI
- `qml/components/`：可复用组件
- `qml/theme/`：Theme、token、样式工具
- `assets/`：图片、字体等资源

---

## 常见坑

- QML 类型找不到：多数是 import 路径/模块没配置好，而不是语法问题
- 发布后找不到图片：开发时用相对路径，发布时没把文件带上；优先用 qrc
- 模块版本混乱：建议统一一个模块 URI 和版本，对外暴露稳定 API

---

## 小结

- qrc 管“打包与稳定路径”，module 管“工程化导入与类型组织”
- 小项目可先用同目录引用；一旦组件多，就尽快 module 化

下一篇：见 [09-性能与调试：Binding 循环、过度创建、Profiler/Scene Graph](09-性能与调试-Binding循环-Profiler-SceneGraph.md)
