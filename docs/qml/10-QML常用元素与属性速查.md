# 10-QML 常用元素与属性速查

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 用“速查表”的方式把常用元素和属性过一遍
- 知道每类元素什么时候用、最常搭配的属性是什么
- 延续本教程约束：示例不使用 `QtQuick.Controls`

---

## 0. 常用 import 速查

你会在很多文件看到这些 import（不一定每个都要用）：

```qml
import QtQuick
import QtQuick.Window
import QtQml
```

- `QtQuick`：Item/Rectangle/Text/Image/动画/列表等绝大多数基础能力
- `QtQuick.Window`：`Window` 类型
- `QtQml`：`Connections`、`Qt.callLater` 等（很多工程会显式引入）

如果你选择用布局系统（非必须）：

```qml
import QtQuick.Layouts
```

---

## 1. 所有元素通用的基础属性

这些属性几乎每个 QML 元素都有（或极常见），建议当作“默认词汇”记住：

| 属性 | 常见类型 | 用途 |
| --- | --- | --- |
| `id` | 标识符 | 在同一个 QML 文件里引用这个对象 |
| `parent` | Item | 父对象，决定坐标系与层级 |
| `x/y` | real | 相对父对象的坐标（与 anchors 二选一更稳） |
| `width/height` | real | 最终尺寸（也可用绑定） |
| `implicitWidth/implicitHeight` | real | 内容建议尺寸（做组件很重要） |
| `visible` | bool | 是否绘制 |
| `opacity` | real | 透明度（0~1） |
| `z` | real | 叠放顺序（越大越靠上） |
| `clip` | bool | 是否裁剪子项超出区域部分 |
| `enabled` | bool | 是否响应输入（常见于输入/交互相关元素） |

---

## 2. Item：一切可视元素的“容器基础”

`Item` 本身不画东西，但负责：

- 坐标系/尺寸
- 子项管理
- anchors

常用属性：

- `anchors.*`：定位与对齐（本教程第 03 章）
- `children`/`childrenRect`：对子项做一些布局计算时会用到

---

## 2.1 Window：窗口与顶层属性（QtQuick.Window）

常用属性：

| 属性 | 类型 | 用途 |
| --- | --- | --- |
| `visible` | bool | 显示/隐藏窗口 |
| `width/height` | int | 窗口尺寸 |
| `title` | string | 标题 |
| `color` | color | 窗口背景色（如果你不想每次都用最外层 Rectangle 铺背景） |

---

## 3. Rectangle：最常用的“能画背景”的盒子

适合做：

- 背景、卡片、按钮基底、分割块

常用属性：

| 属性 | 类型 | 用途 |
| --- | --- | --- |
| `color` | color | 填充颜色 |
| `radius` | real | 圆角 |
| `border.color` | color | 边框色 |
| `border.width` | int | 边框宽度 |
| `gradient` | Gradient | 渐变填充（需要时再用） |

一个“按钮基底”最小写法（不使用 Controls）：

```qml
Rectangle {
    width: 220
    height: 56
    radius: 14
    color: area.pressed ? "#3b6fd0" : "#4c8bf5"

    Text { anchors.centerIn: parent; text: "OK"; color: "#ffffff"; font.pixelSize: 18 }

    MouseArea { id: area; anchors.fill: parent; onClicked: console.log("clicked") }
}
```

---

## 4. Text：展示文本

常用属性：

| 属性 | 类型 | 用途 |
| --- | --- | --- |
| `text` | string | 文本内容 |
| `color` | color | 字色 |
| `font.pixelSize` | int | 字号 |
| `font.family` | string | 字体 |
| `wrapMode` | enum | 换行策略（例如 `Text.Wrap`） |
| `elide` | enum | 省略号（例如 `Text.ElideRight`） |
| `horizontalAlignment` | enum | 水平对齐 |
| `verticalAlignment` | enum | 垂直对齐 |
| `textFormat` | enum | 文本格式（Auto/PlainText/RichText） |
| `maximumLineCount` | int | 最大行数（配合 elide/换行使用） |
| `lineHeight` | real | 行高（配合 `lineHeightMode`） |

提示：

- 文本的 `implicitWidth/implicitHeight` 很常用来驱动容器尺寸（见第 03 章）

---

## 4.1 TextInput：输入框（不依赖 Controls 也能用）

常用属性与信号：

| 名称 | 类型 | 用途 |
| --- | --- | --- |
| `text` | string | 当前输入 |
| `placeholderText` | string | 占位文本（Qt 6 可用） |
| `echoMode` | enum | 密码/普通（Normal/Password） |
| `validator` | Validator | 输入校验（IntValidator/RegExpValidator 等） |
| `inputMethodHints` | flags | 输入法提示（移动端更常用） |
| `activeFocus` | bool | 当前是否拥有焦点（只读） |
| `focus` | bool | 是否可获取焦点 |
| `onAccepted` | signal handler | 回车确认 |
| `onEditingFinished` | signal handler | 编辑结束 |

最小“自绘输入框”结构（背景用 Rectangle，输入用 TextInput）：

```qml
Rectangle {
    width: 280
    height: 44
    radius: 12
    color: "#202124"
    border.width: input.activeFocus ? 2 : 1
    border.color: input.activeFocus ? "#4c8bf5" : "#3a3a40"

    TextInput {
        id: input
        anchors.fill: parent
        anchors.margins: 12
        color: "#e8eaed"
        selectionColor: "#4c8bf5"
        selectedTextColor: "#0f0f10"
        focus: true
        placeholderText: "Type here"
    }
}
```

---

## 5. Image：显示图片

常用属性：

| 属性 | 类型 | 用途 |
| --- | --- | --- |
| `source` | url | 图片地址（qrc 或本地路径） |
| `fillMode` | enum | 填充方式（例如 `Image.PreserveAspectCrop`） |
| `asynchronous` | bool | 异步加载（大图/网络图更重要） |
| `cache` | bool | 是否缓存 |
| `smooth` | bool | 缩放平滑 |
| `sourceSize` | size | 希望加载的目标尺寸（控制解码尺寸，性能很有用） |
| `mipmap` | bool | 缩小时更平滑（有成本） |

---

## 5.1 BorderImage / AnimatedImage

- `BorderImage`：九宫格拉伸（做气泡/对话框背景常用）
  - 常用属性：`border.left/right/top/bottom`、`horizontalTileMode/verticalTileMode`
- `AnimatedImage`：gif/动图（别滥用，注意性能）

---

## 6. MouseArea：鼠标/触摸交互入口

常用属性与信号：

| 名称 | 类型 | 用途 |
| --- | --- | --- |
| `anchors.fill` | anchor | 覆盖父区域 |
| `pressed` | bool | 按下态（常用来驱动颜色/缩放） |
| `hoverEnabled` | bool | 开启 hover 检测 |
| `containsMouse` | bool | hover 状态 |
| `onClicked` | signal handler | 点击 |
| `onPressed/onReleased` | signal handler | 按下/松开 |
| `onPositionChanged` | signal handler | 拖拽/移动 |

建议：

- 交互反馈尽量用“状态驱动 + Behavior/Transition”（见第 04 章）

---

## 6.1 Pointer Handlers：TapHandler/DragHandler/WheelHandler（更现代的输入方式）

在 Qt 6 里，Pointer Handlers 很常用（尤其是要同时兼容鼠标+触摸时）。

常见类型：

- `TapHandler`：点击/轻触
- `DragHandler`：拖拽
- `PinchHandler`：双指缩放
- `WheelHandler`：滚轮

最小点击示例：

```qml
Rectangle {
    width: 160
    height: 56
    radius: 14
    color: tap.pressed ? "#3b6fd0" : "#4c8bf5"

    TapHandler { id: tap; onTapped: console.log("tapped") }
    Text { anchors.centerIn: parent; text: "Tap"; color: "#ffffff"; font.pixelSize: 18 }
}
```

---

## 7. Repeater：重复创建子项（不负责滚动）

适合：

- 小数量、静态/轻量的重复 UI（例如标签云、简单列表）

关键点：

- `model` 可以是数组/整数/模型对象
- 当前项数据通过 `modelData` 访问（数组/简单模型时）

如果你需要滚动/大量数据复用，更适合用 `ListView`（下一节）。

---

## 8. Flickable / ListView：滚动与大量数据的核心

### 8.1 Flickable：给任意内容加滚动

常用属性：

| 属性 | 类型 | 用途 |
| --- | --- | --- |
| `contentWidth/contentHeight` | real | 内容尺寸 |
| `flickableDirection` | enum | 滚动方向 |
| `interactive` | bool | 是否可拖动 |
| `contentX/contentY` | real | 当前滚动偏移 |
| `boundsBehavior` | enum | 越界回弹行为 |
| `maximumFlickVelocity` | real | 最大惯性速度（手感调参） |
| `flickDeceleration` | real | 惯性减速度（手感调参） |

最小结构通常是：

- `Flickable` 作为滚动容器
- 里面放一个 `Column`（或其它内容容器）
- 通过绑定计算 `contentHeight`

### 8.2 ListView：列表（delegate 复用，性能更稳）

常用属性：

| 属性 | 类型 | 用途 |
| --- | --- | --- |
| `model` | var | 数据模型 |
| `delegate` | Component | 每行长什么样 |
| `spacing` | real | 行间距 |
| `currentIndex` | int | 当前项 |
| `orientation` | enum | 水平/垂直列表 |
| `cacheBuffer` | int | 预创建缓存区域（性能调参） |

提示：

- `ListView` 更适合“真实项目列表页”，因为它会做 delegate 复用，数据多也不容易卡

---

## 8.3 ListModel / ListElement：最轻量的列表数据

在 QML 内部做小数据模型时常用：

```qml
ListModel {
    id: m
    ListElement { title: "Alpha"; value: 1 }
    ListElement { title: "Beta"; value: 2 }
}

ListView {
    width: 240
    height: 200
    model: m
    delegate: Text { text: title + " = " + value; color: "#ffffff" }
}
```

---

## 9. Loader / Component：按需加载与动态 UI

常见用法：

- 页面切换：只加载当前页面
- 弹层/对话框：需要时加载，不需要时销毁

常用属性：

| 属性 | 类型 | 用途 |
| --- | --- | --- |
| `Loader.source` | url | 加载某个 qml 文件 |
| `Loader.sourceComponent` | Component | 加载内联 Component |
| `Loader.active` | bool | 是否激活加载 |
| `Loader.item` | Item | 当前加载出来的对象 |

---

## 9.1 StackView / Dialog 为什么没写

这类“应用级控件”大多在 `QtQuick.Controls` 里，而你的教程约束是禁止使用 Controls，所以这里不列。

需要“页面切换”时：

- 小项目：用 `Loader` + 自己维护 `currentPage` 状态即可
- 稍复杂：建议自研一个非常薄的 Router（核心还是 Loader）

---

## 10. Timer：定时器

常用属性：

| 属性 | 类型 | 用途 |
| --- | --- | --- |
| `interval` | int | 间隔毫秒 |
| `repeat` | bool | 是否重复 |
| `running` | bool | 是否在运行 |
| `triggeredOnStart` | bool | 启动时是否立即触发一次 |
| `onTriggered` | signal handler | 定时回调 |

---

## 11. Connections：把 QObject 信号接进 QML

当你需要监听 C++ 对象的信号（不一定是属性变更信号）时很常用：

```qml
Connections {
    target: appState
    function onCountChanged() {
        console.log("count =", appState.count)
    }
}
```

---

## 12. 动画与状态相关元素速查

你会频繁用到这些“非可视元素/语法块”：

- `Behavior`：属性变化自动过渡
- `State/Transition`：多个稳定形态 + 切换过渡
- `NumberAnimation/ColorAnimation/PropertyAnimation`：最常见动画
- `SequentialAnimation/ParallelAnimation`：编排节奏

---

## 13. 常用“声明式写法”速查

### 13.1 property：定义状态与对外 API

```qml
Item {
    property int count: 0
    property color themeColor: "#4c8bf5"
}
```

### 13.2 绑定：让 UI 跟着状态自动更新

```qml
Rectangle {
    width: parent.width * 0.6
    color: count > 3 ? "#e53935" : "#4c8bf5"
}
```

### 13.3 signal / function：组件事件与命令式能力

```qml
Item {
    signal clicked()
    function reset() { count = 0 }
}
```

---

## 14. 焦点与键盘（Keys/FocusScope）

做输入/快捷键时必用：

- `focus`：是否可获取焦点
- `activeFocus`：当前是否真的拿到了焦点
- `forceActiveFocus()`：强制获取焦点
- `FocusScope`：管理一片区域的焦点分配（多个输入控件时很有用）
- `Keys` 附加属性：键盘事件

最小键盘处理：

```qml
Item {
    focus: true
    Keys.onPressed: (event) => {
        if (event.key === Qt.Key_Escape) {
            console.log("escape")
            event.accepted = true
        }
    }
}
```

---

## 15. 常用“附加属性”（Attached Properties）速查

很多能力不是“元素自带属性”，而是“附加属性”：

- `anchors.*`：定位（属于 Item 的附加属性）
- `Layout.*`：布局参数（当父容器是 RowLayout/ColumnLayout/GridLayout 时生效）
- `Keys.*`：键盘事件（目标需要能获取焦点）

---
---

## 小结

- `Item/Rectangle/Text/MouseArea` 是不用 Controls 也能搭 UI 的“四件套”
