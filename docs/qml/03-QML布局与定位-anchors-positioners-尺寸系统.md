# 03-QML 布局与定位：anchors / positioners / 尺寸系统

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 搞懂 QML 的尺寸系统：`width/height`、`implicitWidth/implicitHeight`、绑定与自适应
- 会用 `anchors` 做常见布局：铺满、居中、边距、左右对齐
- 会用 `Row/Column/Grid/Flow + Repeater` 快速排版一组元素（不依赖 `QtQuick.Controls`）

---

## 1. 尺寸系统：显式尺寸 vs 隐式尺寸

在 QML 里，一个元素最终尺寸通常来自三种来源（优先级从高到低）：

1. 显式设置：`width: 200; height: 40`
2. 绑定表达式：`width: parent.width * 0.6`
3. 隐式尺寸：`implicitWidth/implicitHeight`（由内容决定，比如 `Text` 的 `implicitWidth`）

一个“内容驱动尺寸”的最小例子：

```qml
import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 420
    height: 240
    visible: true

    Rectangle {
        anchors.centerIn: parent
        radius: 12
        color: "#202124"
        width: label.implicitWidth + 32
        height: label.implicitHeight + 18

        Text {
            id: label
            anchors.centerIn: parent
            text: "implicit size"
            color: "#ffffff"
            font.pixelSize: 18
        }
    }
}
```

你要记住的直觉：

- `implicitWidth/implicitHeight` 是“建议尺寸”，并不等于最终 `width/height`
- 做可复用组件时，优先给根元素提供 `implicitWidth/implicitHeight`，让外部可以“按内容自然生长”，也可以外部强行设定尺寸覆盖

---

## 2. 坐标系与父子关系：`x/y`、`parent`、`z`

- 每个元素的 `x/y` 都是相对父元素坐标系
- 同级元素默认后声明的在上面（可用 `z` 改叠放顺序）
- `visible` 决定是否绘制；`opacity` 决定透明度（但仍占布局空间）

```qml
Rectangle {
    width: 200
    height: 120
    color: "#111111"

    Rectangle { width: 80; height: 80; x: 20; y: 20; color: "#6750A4"; z: 1 }
    Rectangle { width: 80; height: 80; x: 60; y: 40; color: "#1E88E5"; z: 0 }
}
```

---

## 3. Anchors：90% 的界面都能靠它搞定

### 3.1 常用 anchors 写法

- 铺满：`anchors.fill: parent`
- 居中：`anchors.centerIn: parent`
- 贴边：`anchors.left: parent.left`（同理 right/top/bottom）
- 边距：`anchors.margins: 12` 或 `anchors.leftMargin: 12`
- 两边撑开：`anchors.left: parent.left; anchors.right: parent.right`

一个“顶部栏 + 内容区”的经典结构：

```qml
import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 480
    height: 320
    visible: true

    Rectangle {
        anchors.fill: parent
        color: "#0f0f10"

        Rectangle {
            id: header
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 56
            color: "#1b1b1f"

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 16
                text: "Header"
                color: "#ffffff"
                font.pixelSize: 18
            }
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: header.bottom
            anchors.bottom: parent.bottom
            anchors.margins: 16
            radius: 12
            color: "#202124"
        }
    }
}
```

### 3.2 Anchors 的两个常见坑

- 你给元素同时写了 `x/y` 又写了 `anchors.*`：最终会以 anchors 为主，`x/y` 的直觉容易“失效”
- 你给了 `anchors.left/right` 又手动写 `width`：可能导致约束冲突或尺寸不符合预期；更常用的是“左右撑开”让宽度自动算出来

---

## 4. Positioners：Row / Column / Grid / Flow

`Row/Column/Grid/Flow` 都属于 QtQuick 的“排版器”，适合“一组同类元素”快速排版。

### 4.1 Column + Repeater：最常用组合

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

        Column {
            anchors.centerIn: parent
            spacing: 10

            Repeater {
                model: [ "Alpha", "Beta", "Gamma", "Delta" ]

                Rectangle {
                    width: 280
                    height: 44
                    radius: 10
                    color: "#202124"

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 14
                        text: modelData
                        color: "#e8eaed"
                        font.pixelSize: 16
                    }
                }
            }
        }
    }
}
```

你会经常用到的点：

- `Repeater` 只负责“创建多个子项”，不负责滚动；需要滚动时再用 `Flickable`/`ListView`
- `modelData` 是 `Repeater` 默认暴露的当前元素数据

### 4.2 Flow：做“自适应换行”的标签云

```qml
Flow {
    width: 320
    spacing: 8

    Repeater {
        model: [ "QML", "anchors", "animation", "component", "C++", "signal" ]

        Rectangle {
            radius: 999
            color: "#2a2a2e"
            width: tagText.implicitWidth + 18
            height: 30

            Text {
                id: tagText
                anchors.centerIn: parent
                text: modelData
                color: "#ffffff"
                font.pixelSize: 14
            }
        }
    }
}
```

### 4.3 QtQuick.Layouts：真正的“布局系统”（可用但需要额外 import）

你问的 “layout” 指的一般是 `QtQuick.Layouts` 这个模块（`RowLayout/ColumnLayout/GridLayout`）。

本章前面优先写 anchors/positioners 的原因：

- anchors/positioners 是 `QtQuick` 基础模块，自带、轻量、规则简单，足够覆盖大多数界面
- `QtQuick.Layouts` 是额外模块，需要显式 `import QtQuick.Layouts`，并且有一套 `Layout.*` 附加属性（学习成本更高一些）
- layout 容器管理子项几何时，通常不建议子项再用 anchors 去“抢位置”（容易互相打架）；但 anchors 仍然很适合做“页面结构层”的大块区域切分

一个不用 `QtQuick.Controls`、但使用 `QtQuick.Layouts` 的最小例子：

```qml
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Window {
    width: 480
    height: 320
    visible: true

    Rectangle {
        anchors.fill: parent
        color: "#0f0f10"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 56
                radius: 12
                color: "#202124"
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 12

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 12
                    color: "#2a2a2e"
                }

                Rectangle {
                    Layout.preferredWidth: 140
                    Layout.fillHeight: true
                    radius: 12
                    color: "#2a2a2e"
                }
            }
        }
    }
}
```

什么时候更推荐用 layouts：

- 你需要“比例/伸缩”这类规则：`Layout.fillWidth/fillHeight`、`Layout.preferredWidth`、`Layout.minimumWidth`
- 你想做更“像桌面 UI”一样的自适应：窗口缩放时自动分配空间

什么时候 anchors 更合适：

- 页面结构切分（顶部栏/内容区/底栏）
- 需要绝对对齐（贴边、居中、边距）的场景

---

## 5. 可点击区域：MouseArea 的最小规则

- 覆盖：`anchors.fill: parent`
- 按下态：`pressed`（常用来驱动颜色/缩放）
- 悬停：`hoverEnabled: true` 后用 `containsMouse` 做 hover 效果

```qml
Rectangle {
    id: chip
    width: 120
    height: 36
    radius: 999
    color: area.pressed ? "#6750A4" : (area.containsMouse ? "#3a3a40" : "#2a2a2e")

    Text { anchors.centerIn: parent; text: "Chip"; color: "#ffffff" }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        onClicked: chip.visible = false
    }
}
```

---

## 小练习

### 练习 1：做一个“顶部栏 + 右上角按钮”

- 题目：顶部栏左侧是标题，右侧是一个 32x32 的圆形按钮
- 约束：不使用 QtQuick.Controls
- 提示：标题用 `anchors.left`，按钮用 `anchors.right`，都 `anchors.verticalCenter`

### 练习 2：做一个“标签云”

- 题目：用 `Flow + Repeater` 把一组标签排出来
- 约束：不使用 QtQuick.Controls
- 目标：练 implicit size 与 Flow 换行

---

## 小结

- 尺寸优先级：显式尺寸/绑定表达式 > 隐式尺寸
- anchors 适合“结构型布局”，positioners 适合“重复元素排版”
- MouseArea 常配合 `pressed/containsMouse` 做交互反馈

下一篇：见 [04-QML状态与动画：states / transitions / Behavior](04-QML状态与动画-states-transitions-behavior.md)
