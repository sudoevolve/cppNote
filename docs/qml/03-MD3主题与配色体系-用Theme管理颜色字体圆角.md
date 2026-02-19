# 03-MD3 主题与配色体系：用 Theme 管理颜色、字体、圆角

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 理解 Material Design 3（MD3）里“主题”到底是什么：一套可复用的设计 token
- 学会用一个 `Theme` 把颜色/字号/圆角集中管理，避免到处写魔法数
- 在不使用 `QtQuick.Controls` 的前提下，写出“可换肤、可维护”的 QML

## 先决知识（建议）

- 会写基本 QML（见 QML 02 章）

---

## 1. 先给结论：主题不是“换个主色”，而是一个系统

新手最容易把主题理解成：
- 把 `"#6750A4"` 换成别的颜色

但在 MD3 里，主题更像是一套“约定”：
- 颜色：primary、onPrimary、surface、background、outline……
- 字体：字号、字重、行高（标题/正文/注释分别用什么）
- 形状：圆角、阴影、边框厚度
- 间距：padding、gap、组件高度等

你可以把它类比成：
- 不是“买一瓶颜料”，而是“买一整套装修规范”

---

## 2. 为什么要用 Theme：解决三类痛点

### 2.1 避免魔法数蔓延

如果你到处写：
- `radius: 12`
- `color: "#6750A4"`
- `font.pixelSize: 16`

后果是：
- 想统一改风格很难
- 组件复用到别的页面就不一致

### 2.2 让组件“自动跟随”主题

你希望做到：
- 主题换了，按钮/卡片/文本颜色自动跟着变
- 不用每个组件都手改

### 2.3 为“动态配色”留接口

MD3 很强调从一个“种子色（seed color）”生成整套色板：
- 你只给一个 seed，系统生成 primary/onPrimary/surface…的一组颜色
- 甚至可以模拟“壁纸取色”的动态主题

---

## 3. 最小可行的 Theme 形态：一个集中入口

你在 QML 里想要的调用体验大概是这样：

```qml
Rectangle {
    color: Theme.color.background
    radius: Theme.shape.cornerM

    Text {
        text: "Hello"
        color: Theme.color.onBackground
        font.pixelSize: Theme.typography.body
    }
}
```

重点不在于 Theme 一定长什么样，而在于：
- **所有“设计取值”从一个地方拿**

---

## 4. MD3 的颜色角色：新手先掌握 8 个就够

你不需要一上来把所有角色背下来，先记住这些高频：

- `primary`：主色（按钮、强调元素）
- `onPrimary`：放在 primary 上的文字/图标颜色
- `background`：页面背景
- `onBackground`：背景上的文字颜色
- `surface`：卡片/弹层表面
- `onSurface`：表面上的文字颜色
- `outline`：描边/分割线
- `error` / `onError`：错误态

新手用法建议：
- 页面用 `background/onBackground`
- 卡片用 `surface/onSurface`
- 强调按钮用 `primary/onPrimary`

---

## 5. 形状与间距：把“视觉一致性”变成约束

你会发现一个工程里常见圆角就那几档：
- 小圆角：chip、输入框
- 中圆角：按钮、卡片
- 大圆角：弹层、浮动面板

所以你可以用固定档位表达：
- `cornerS/cornerM/cornerL`

间距也是一样：
- `space1/space2/space3...`

这样你的组件会“自动风格统一”。

---

## 6. 如果你接入 md3.Core：直接使用 Theme 单例

如果你使用 `material-components-qml` 的 `md3.Core` 模块，它提供了集中 Theme 系统（颜色/排版/形状）和动态配色能力。

典型用法是：

```qml
import QtQuick
import QtQuick.Window
import md3.Core

Window {
    width: 800
    height: 600
    visible: true
    color: Theme.color.background

    Text {
        anchors.centerIn: parent
        text: "Hello MD3"
        color: Theme.color.onBackground
    }
}
```

---

## 常见坑

- 错误：把主题当成“给按钮一个颜色”  
  结果：按钮好看了，但卡片/文字/分割线风格全乱  
  正确：把“颜色角色”作为约定使用（background/surface/primary）

- 错误：组件内部写死 `color: "#..."`  
  结果：组件无法跟随主题  
  正确：组件内部尽量从 Theme 取值，把“自定义”暴露成 property（并给合理默认值）

---

## 小练习

### 练习 1：把你的页面改成“主题驱动”

- 题目：把 main.qml 里所有颜色/圆角/字号集中到一个 Theme 入口
- 约束：禁止 QtQuick.Controls
- 目标：体验“一处改动，全局生效”

### 练习 2：做一个卡片组件（不写死颜色）

- 题目：用 Rectangle 写一个 `Card`，默认用 Theme 的 surface/onSurface
- 约束：禁止 QtQuick.Controls
- 提示：做成可复用组件时，把 `backgroundColor/textColor` 做成 property
- 目标：练“组件可换肤”

---

## 小结

- MD3 主题是一套 token：颜色角色 + 字体排版 + 形状间距
- 用 Theme 集中管理，能显著降低维护成本
- 接入 md3.Core 后可以直接使用其 Theme 系统，并为动态配色铺路

下一篇：见 [04-接入 md3.Core 组件库：从源码集成到 QML import](04-接入md3.Core组件库-从源码集成到QML导入.md)

