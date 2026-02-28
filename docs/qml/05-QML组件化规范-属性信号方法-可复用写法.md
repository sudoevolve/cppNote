# 05-QML 组件化规范：属性/信号/方法与可复用写法

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 把 “Rectangle + Text + MouseArea” 组合成稳定可复用组件
- 设计组件对外 API：`property`、`signal`、（必要时）`function`
- 让组件默认“按内容自然生长”，外部又能覆盖尺寸与样式

约束：示例不使用 `QtQuick.Controls`。

---

## 1. 文件名与类型名：最基本但最常踩

- 组件类型名来自文件名：`Badge.qml` 对应 `Badge { }`
- 使用时能被找到，取决于它是否在 QML import 路径里（同目录通常可直接用）

---

## 2. 组件的三层结构：外壳 / 内容 / 交互

一个实用的写法是把组件拆成三层思维：

- 外壳：尺寸、圆角、背景、阴影（如果有）
- 内容：文字/图形（由属性驱动）
- 交互：MouseArea（把事件转成对外 signal）

下面是一个 “Badge（标签/小胶囊）” 的完整示例组件：

```qml
import QtQuick 2.15

Rectangle {
    id: root

    property string text: "Badge"
    property color textColor: "#ffffff"
    property color backgroundColor: "#2a2a2e"
    property int paddingH: 10
    property int paddingV: 6
    property int pixelSize: 14
    property bool clickable: true

    signal clicked()

    radius: 999
    color: backgroundColor

    implicitWidth: label.implicitWidth + paddingH * 2
    implicitHeight: label.implicitHeight + paddingV * 2

    Text {
        id: label
        anchors.centerIn: parent
        text: root.text
        color: root.textColor
        font.pixelSize: root.pixelSize
    }

    MouseArea {
        anchors.fill: parent
        enabled: root.clickable
        onClicked: root.clicked()
    }
}
```

使用方式（同目录直接用）：

```qml
Badge {
    text: "QML"
    onClicked: console.log("badge clicked")
}
```

---

## 3. 尺寸与自适应：优先提供 implicit 尺寸

可复用组件里，推荐：

- 根元素写 `implicitWidth/implicitHeight`（由内容决定）
- 外部如果需要固定尺寸，再覆盖 `width/height`

例如上面的 Badge：

- 默认会根据 `text` 自动变宽
- 外部也可以写 `width: 200` 强行变成固定宽度

---

## 4. API 设计：哪些该暴露，哪些不该暴露

建议把对外 API 控制在这三类：

- `property`：外部可配置的外观/状态（文本、颜色、是否可点）
- `signal`：外部关心的事件（clicked/pressed/finished 等）
- `function`：外部需要命令式触发的能力（例如 start/stop/restart）

避免把内部实现细节暴露给外部（例如直接暴露内部 `Text`/`MouseArea` 的 id 让外部去改），否则组件可维护性会变差。

---

## 5. 状态驱动的交互反馈：按下态/悬停态

不用 Controls 也能有“像按钮一样”的反馈，常用做法：

- 用 MouseArea 的 `pressed` 驱动颜色/缩放
- 用 `hoverEnabled + containsMouse` 驱动 hover
- 用 `Behavior` 做过渡

```qml
import QtQuick 2.15

Rectangle {
    id: root

    property string text: "Button"
    signal clicked()

    width: 220
    height: 56
    radius: 14
    color: area.pressed ? "#3b6fd0" : (area.containsMouse ? "#5a96ff" : "#4c8bf5")
    scale: area.pressed ? 0.98 : 1.0

    Behavior on color { ColorAnimation { duration: 140 } }
    Behavior on scale { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }

    Text {
        anchors.centerIn: parent
        text: root.text
        color: "#ffffff"
        font.pixelSize: 18
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}
```

---

## 6. 组件组织建议：按“用途”而不是按“类型”

一个简单且好维护的组织方式：

- `components/` 放可复用组件
- 每个组件一个文件夹（复杂时），里面放 `*.qml` 与对应文档/示例
- 页面级 UI 放 `pages/`（或 `views/`），不要把页面当“组件库”

你已经有一个组件库示例：见 [SplitText](components/SplitText/SplitText.md)。

---

## 常见坑

- 忘了写 `implicitWidth/implicitHeight`：组件默认尺寸可能是 0，看起来像“没显示”
- 同时用 anchors 撑开宽度，又给了固定 width：容易出现约束冲突
- 外部通过 `id.xxx` 去改内部实现：短期方便，长期维护成本高

---

## 小练习

### 练习 1：把“卡片”封装成组件

- 题目：做一个 `Card.qml`，外部可配置 `title/subtitle` 与背景色
- 约束：不使用 QtQuick.Controls
- 目标：练 implicit size 与对外 API

### 练习 2：给组件加一个可选动画

- 题目：给 Card 增加 `property bool animated: true`，animated 时颜色变化有过渡
- 提示：根据 `animated` 动态决定 `Behavior` 的 duration（0 或正常值）

---

## 小结

- 组件对外 API 控制在 property/signal/function 三类
- implicit size 是“好用组件”的基础
- 交互反馈用 MouseArea + Behavior 就能做得很顺

下一篇：见 [06-QML与C++交互：注册类型/上下文属性/信号槽](06-QML与C++交互-注册类型-上下文属性-信号槽.md)
