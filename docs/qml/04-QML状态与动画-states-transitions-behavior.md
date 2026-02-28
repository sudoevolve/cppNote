# 04-QML 状态与动画：states / transitions / Behavior

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 用“状态驱动 UI”的方式组织交互（少写手动更新）
- 掌握 3 类最常用动画：`Behavior`、`Transition`、`Sequential/ParallelAnimation`
- 写出常见动效：按下反馈、展开/收起、淡入淡出、位移/缩放

约束：示例不使用 `QtQuick.Controls`。

---

## 1. 先建立直觉：状态是“真相”，动画只是“过渡方式”

推荐的写法是：

- 先定义状态变量（例如 `property bool expanded`）
- 界面属性通过表达式绑定到状态（例如 `height: expanded ? 240 : 64`）
- 再用 `Behavior` 或 `Transition` 给属性变化加过渡

这样做的好处：

- UI 不容易“跑偏”（不会忘记某个属性没更新）
- 动画可以随时删/改，不影响状态逻辑

---

## 2. Behavior：给某个属性加“自动过渡”

当某个属性值发生变化时，`Behavior` 会自动插入动画。

```qml
import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 420
    height: 280
    visible: true

    Rectangle {
        anchors.centerIn: parent
        width: 260
        height: 120
        radius: 14
        color: box.pressed ? "#3b6fd0" : "#4c8bf5"
        scale: box.pressed ? 0.98 : 1.0

        property bool pressed: false

        Behavior on color { ColorAnimation { duration: 140 } }
        Behavior on scale { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }

        Text {
            anchors.centerIn: parent
            text: box.pressed ? "Pressed" : "Press me"
            color: "#ffffff"
            font.pixelSize: 18
        }

        MouseArea {
            anchors.fill: parent
            onPressed: box.pressed = true
            onReleased: box.pressed = false
        }
    }
}
```

你会频繁用到的点：

- `Behavior` 适合“某一个属性总要过渡”的场景（例如颜色、透明度、缩放）
- 如果你需要“只有某些状态切换时才动画”，更适合用 `Transition`

---

## 3. States + Transition：把界面切成几个稳定形态

下面例子做一个“可展开面板”，包含：

- 折叠态：高度小、内容不可见
- 展开态：高度大、内容淡入

```qml
import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 480
    height: 360
    visible: true

    Rectangle {
        anchors.fill: parent
        color: "#0f0f10"

        Rectangle {
            id: panel
            anchors.horizontalCenter: parent.horizontalCenter
            y: 32
            width: 360
            radius: 16
            color: "#202124"

            property bool expanded: false
            state: expanded ? "expanded" : "collapsed"

            states: [
                State {
                    name: "collapsed"
                    PropertyChanges { target: panel; height: 64 }
                    PropertyChanges { target: content; opacity: 0; y: 16 }
                },
                State {
                    name: "expanded"
                    PropertyChanges { target: panel; height: 220 }
                    PropertyChanges { target: content; opacity: 1; y: 0 }
                }
            ]

            transitions: [
                Transition {
                    NumberAnimation { properties: "height,y,opacity"; duration: 220; easing.type: Easing.OutCubic }
                }
            ]

            Rectangle {
                id: header
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 64
                radius: 16
                color: "transparent"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                    text: panel.expanded ? "Panel (expanded)" : "Panel (collapsed)"
                    color: "#e8eaed"
                    font.pixelSize: 16
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: panel.expanded = !panel.expanded
                }
            }

            Item {
                id: content
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: header.bottom
                anchors.bottom: parent.bottom
                anchors.margins: 16
                opacity: 0

                Column {
                    anchors.fill: parent
                    spacing: 10

                    Rectangle { width: parent.width; height: 40; radius: 10; color: "#2a2a2e" }
                    Rectangle { width: parent.width; height: 40; radius: 10; color: "#2a2a2e" }
                    Rectangle { width: parent.width; height: 40; radius: 10; color: "#2a2a2e" }
                }
            }
        }
    }
}
```

你要抓住的点：

- `state` 是最终形态（稳定态），`Transition` 是两个稳定态之间的过渡
- `PropertyChanges` 是“切到这个 state 时，各属性应该是什么值”
- `Transition` 的 `properties` 可以一次写多个，用逗号分隔

---

## 4. Sequential / Parallel：做更复杂的节奏

当你需要“先 A 后 B”的节奏，用 `SequentialAnimation`：

```qml
SequentialAnimation {
    running: true
    loops: Animation.Infinite

    NumberAnimation { target: box; property: "opacity"; to: 1; duration: 200 }
    PauseAnimation { duration: 500 }
    NumberAnimation { target: box; property: "opacity"; to: 0.2; duration: 200 }
    PauseAnimation { duration: 500 }
}
```

当你需要“同时改变多个属性”，用 `ParallelAnimation`（或 `Transition` 里直接放多个动画）：

```qml
ParallelAnimation {
    NumberAnimation { target: box; property: "x"; to: 240; duration: 240; easing.type: Easing.OutCubic }
    NumberAnimation { target: box; property: "opacity"; to: 0.6; duration: 240 }
}
```

---

## 常见坑

- 在“绑定属性”上手动赋值：会导致绑定断开，后续不再自动跟随状态变化
- 直接用动画去“推状态”：比如只改 `x`、不改 `expanded`，久了状态与 UI 容易不一致
- 在 `Transition` 里动画 target 过多：建议只动画用户能感知的属性（高度/透明度/位移/缩放）

---

## 小练习

### 练习 1：做一个“按下反馈按钮”

- 题目：按下时颜色变深、缩小一点点，松开恢复
- 约束：不使用 QtQuick.Controls
- 提示：`pressed` + `Behavior on color/scale`

### 练习 2：做一个“展开卡片”

- 题目：点击卡片头部展开内容，内容淡入并向上轻微位移
- 提示：`states + Transition` 同时动画 `height/opacity/y`

---

## 小结

- 状态是稳定形态，动画是过渡方式
- `Behavior` 适合“某属性总要过渡”，`Transition` 适合“状态切换时才动画”
- 用状态变量驱动 UI，能最大限度减少“手动同步”的 bug

下一篇：见 [05-QML组件化规范：属性/信号/方法与可复用写法](05-QML组件化规范-属性信号方法-可复用写法.md)
