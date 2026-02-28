# 01-创建 QML 项目：从 Qt Creator 到跑起来

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 在 Qt Creator 里创建一个“能跑起来”的 QML 项目
- 看懂 QML 项目的最小骨架：CMakeLists / main.cpp / main.qml
- 按本教程约束：不使用 `QtQuick.Controls` 也能做出最小界面

## 先决知识（建议）

- 已安装 Qt 与 Qt Creator
- Qt Creator 里已有可用的 Desktop Kit（Qt 版本 + 编译器）

---

## 1. 在 Qt Creator 里创建项目（推荐：Qt 6 + CMake）

1. File → New File or Project…
2. 选择 Application → Qt → Qt Quick Application（或类似名字）
3. 选择构建系统：优先选 CMake
4. 选择 Kit：优先选一个能跑起来的（通常是 Desktop Qt 6.x.x MinGW/MSVC）
5. Finish

你创建完后，第一次点击 Run（运行）时，Qt Creator 会自动：
- 配置（configure）
- 编译（build）
- 运行（run）

---

## 2. 认识最小工程骨架：你只需要抓住 3 个文件

不同模板会略有差异，但你一定能找到这三块：

- `CMakeLists.txt`：告诉构建系统怎么编译、怎么把 QML 打包进程序
- `main.cpp`：程序入口，创建应用与 QML 引擎并加载 QML
- `main.qml`：你的界面从这里开始

---

## 3. 把模板改成“不用 QtQuick.Controls 也能跑”

Qt Creator 的默认 Qt Quick 模板经常会带 `QtQuick.Controls`。但你的教程明确禁止使用它，所以你可以直接把 `main.qml` 改成下面这个最小版本：

```qml
import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 360
    height: 240
    visible: true
    title: "QtCraft QML"

    Rectangle {
        anchors.fill: parent
        color: "#121212"

        Rectangle {
            anchors.centerIn: parent
            width: 240
            height: 80
            radius: 14
            color: "#6750A4"

            Text {
                anchors.centerIn: parent
                text: "Hello QML"
                color: "#ffffff"
                font.pixelSize: 20
            }

            MouseArea {
                anchors.fill: parent
                onClicked: parent.color = parent.color === "#6750A4" ? "#1E88E5" : "#6750A4"
            }
        }
    }
}
```

到这里你就已经能做到：
- 一个窗口
- 一块区域
- 点击后改变颜色（交互）

这已经足够支撑你继续学 QML 的“树结构 + 属性绑定”的直觉。

---

## 4. 如果你运行失败，优先排这三类问题

### 4.1 Kit 不可用 / 编译器找不到

表现：
- “No valid kits found”
- 或者 CMake/编译器路径报错

处理：
- 回到 Qt Creator 的 Kits 设置，确保 Qt 版本与编译器都装齐

### 4.2 QML 版本/模块导入失败

表现：
- QML 报 “module 'QtQuick.Window' is not installed”

处理：
- 你的 Qt 安装里可能没装 Qt Quick/QML 相关组件，回安装器补装
- 确认你用的是 Qt 6 的 Desktop Kit

### 4.3 QML 没加载到 / 运行后“什么都没有”

表现：
- 程序启动了但空白窗口或直接退出

处理：
- 确认 `main.qml` 确实在工程里，并且引擎加载路径正确
- 最快自检：先用一个明确的背景色（比如上面例子里的 `"#121212"`）确认 QML 生效

---

## 小练习

### 练习 1：改标题与尺寸

- 题目：把窗口标题改成你项目名，把宽高改成 800x600
- 约束：禁止 QtQuick.Controls
- 目标：熟悉“改 QML 立即看到效果”

### 练习 2：加一个计数器

- 题目：点击一次 count + 1，并把 count 显示出来
- 约束：禁止 QtQuick.Controls
- 提示：用 `property int count: 0`
- 目标：练“状态驱动 UI”

---

## 小结

- QML 项目最关键三块：CMakeLists / main.cpp / main.qml
- Qt Creator 模板可能自带 Controls，但你可以用 Window + Rectangle + Text + MouseArea 完成最小 UI
- 能跑起来以后再学“绑定、状态、动画、组件化”会轻松很多

下一篇：见 [02-QML入门-快速搭界面（不使用QtQuick.Controls）](02-QML入门-快速搭界面（不使用QtQuick.Controls）.md)
