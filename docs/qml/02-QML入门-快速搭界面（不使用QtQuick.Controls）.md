# 02-QML 入门：快速搭界面（不使用 QtQuick.Controls）

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 用 QML 写出一个最小界面：矩形、文字、点击交互
- 理解 QML 的核心直觉：属性绑定（binding）+ 声明式布局
- 在不使用 `QtQuick.Controls` 的前提下，做一个可复用的小组件

## 先决知识（可选）

- 你已经能跑起来 QML 工程（见 QML 01 章）

---

## 1. QML 是什么：像“搭积木写 UI 的描述语言”

- 专业名称：QML（Qt Modeling Language）
- 类比：你不是一行行告诉电脑“先画这个再画那个”，而是直接描述“界面长什么样”
- 作用：开发 UI 更快，动画更顺，适合快速迭代
- 规则/坑点：本教程明确禁止使用 `QtQuick.Controls`；我们用 `Rectangle/Text/MouseArea` 等基础元素自己搭

---

## 2. 最小 QML：一个窗口 + 一块可点击区域

```qml
import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 360
    height: 240
    visible: true
    title: "QML Demo"

    Rectangle {
        anchors.centerIn: parent
        width: 220
        height: 72
        radius: 12
        color: "#4c8bf5"

        Text {
            anchors.centerIn: parent
            text: "Click me"
            color: "#ffffff"
            font.pixelSize: 20
        }

        MouseArea {
            anchors.fill: parent
            onClicked: parent.color = parent.color === "#4c8bf5" ? "#7c4dff" : "#4c8bf5"
        }
    }
}
```

你要抓住两个直觉：
- 你在写“树结构”：Window 里面有 Rectangle，Rectangle 里面有 Text、MouseArea
- 属性就是 UI 的状态：`color` 变了，界面自然变

---

## 3. 属性绑定（binding）：让值“自动跟着变”

- 专业名称：Property Binding（属性绑定）
- 类比：像“公式”。你写的是 `width: parent.width * 0.6`，以后 parent 变了，width 自动跟着算
- 作用：少写手动更新代码，界面更稳定
- 规则/坑点：绑定是表达式，不要在绑定属性上又手动赋值造成“断开绑定”

一个最小例子：

```qml
Rectangle {
    width: parent.width * 0.6
    height: width * 0.3
}
```

---

## 4. 状态与动画：让 UI “动起来”

你可以用最基础的 `Behavior` 给某个属性加过渡动画：

```qml
Rectangle {
    id: box
    width: 220
    height: 72
    radius: 12
    color: pressed ? "#3b6fd0" : "#4c8bf5"

    property bool pressed: false

    Behavior on color {
        ColorAnimation { duration: 180 }
    }

    MouseArea {
        anchors.fill: parent
        onPressed: box.pressed = true
        onReleased: box.pressed = false
    }
}
```

你可以把 Behavior 理解成：
- 当 color 改变时，用动画去过渡，而不是瞬间跳变

---

## 5. 写一个不依赖 Controls 的“按钮组件”（可复用）

你可以把一个 Rectangle + Text + MouseArea 封装成一个组件文件，例如 `SimpleButton.qml`：

```qml
import QtQuick 2.15

Rectangle {
    id: root
    width: 220
    height: 72
    radius: 12

    property string text: "Button"
    property color textColor: "#ffffff"
    property color normalColor: "#4c8bf5"
    property color pressedColor: "#3b6fd0"

    signal clicked()

    color: area.pressed ? pressedColor : normalColor

    Behavior on color {
        ColorAnimation { duration: 160 }
    }

    Text {
        anchors.centerIn: parent
        text: root.text
        color: root.textColor
        font.pixelSize: 20
    }

    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
```

使用时：

```qml
SimpleButton {
    text: "OK"
    onClicked: console.log("clicked")
}
```

---

## 常见坑

- 错误：以为 `MyButton {}` 会自动可用，但文件名不匹配/不在 import 路径  
  结果：类型找不到或被当成属性组导致报错  
  正确：组件文件名与类型名一致（`SimpleButton.qml` 对应 `SimpleButton`），并保证在 QML 可 import 的路径

- 错误：在绑定属性上手动赋值  
  结果：绑定被断开，后续不自动更新  
  正确：要么一直用绑定表达式，要么改成由状态变量驱动（例如 `pressed`）

---

## 小练习

### 练习 1：做一个计数器

- 题目：一个 Text 显示 count，一个按钮点击 +1
- 输入/输出：点击按钮 count 增加
- 约束：禁止 QtQuick.Controls
- 提示：用 `property int count: 0`
- 目标：练属性、绑定、点击事件

### 练习 2：做一个列表（Repeater）

- 题目：用 Repeater 把一组字符串显示成竖向列表
- 输入/输出：显示 N 行文本
- 约束：禁止 QtQuick.Controls
- 提示：用 Column + Repeater + Text
- 目标：练“声明式生成多个元素”

---

## 小结

- QML 是声明式 UI：用树结构描述界面，用属性表达状态
- 绑定让界面自动跟随状态变化，动画让变化更丝滑
- 不用 QtQuick.Controls 也能组合出可复用组件（Rectangle + Text + MouseArea）

下一篇：见 [03-QML布局与定位：anchors / positioners / 尺寸系统](03-QML布局与定位-anchors-positioners-尺寸系统.md)
