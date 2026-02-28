# 07-主题与配色体系：用 Theme 管理颜色、字体、圆角

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 把“颜色/字体/圆角/间距”从业务 UI 里抽离出来，形成统一的主题体系
- 让组件只关心“语义色”，而不是到处写 `"#4c8bf5"`
- 在不使用 `QtQuick.Controls` 的前提下，也能做出一致的视觉风格

---

## 1. 为什么要做 Theme

当项目只有 1~2 个页面时，直接写颜色值没问题；一旦页面/组件变多，就会出现：

- 同一种“主色”在不同地方出现不同色值
- 黑暗模式/换肤成本高（要全局替换）
- 组件复用困难（组件里写死了样式）

Theme 的目标就是让你写 UI 时只写“语义”，例如：

- `Theme.color.primary`
- `Theme.radius.card`
- `Theme.spacing.m`

---

## 2. Theme 该包含什么（建议最小集合）

建议先把 Theme 控制在三块：

- **颜色**：背景/表面/文字/描边/主色/危险色/成功色
- **圆角**：小/中/大（chip/card/dialog）
- **间距与字号**：xs/s/m/l + 主要字号

---

## 3. 推荐写法：QML 单例（概念示例）

你可以用“QML 单例”的方式把 Theme 暴露给全局 QML 使用。

概念上（示例代码只用于说明结构）：

```qml
pragma Singleton
import QtQuick 2.15

QtObject {
    id: Theme

    readonly property QtObject color: QtObject {
        readonly property color background: "#0f0f10"
        readonly property color surface: "#202124"
        readonly property color primary: "#4c8bf5"
        readonly property color textPrimary: "#e8eaed"
        readonly property color textSecondary: "#9aa0a6"
        readonly property color danger: "#e53935"
    }

    readonly property QtObject radius: QtObject {
        readonly property real s: 10
        readonly property real m: 14
        readonly property real l: 18
    }

    readonly property QtObject spacing: QtObject {
        readonly property int xs: 6
        readonly property int s: 10
        readonly property int m: 16
        readonly property int l: 24
    }
}
```

使用时组件里只用“语义 token”：

```qml
Rectangle {
    color: Theme.color.surface
    radius: Theme.radius.m
}
```

### 3.1 这段 Theme 放哪，才能在整个工程里都能用

上面的 `pragma Singleton` 只是“写法”，要让 `Theme` 在任意 QML 文件都能 `Theme.xxx`，还需要让它进入 QML 的 import 体系。常见两种落地方式：

- **做成 QML module**：把 `Theme.qml` 放到你的 QML module 里（见第 08 章的 module 组织思路），然后在业务 QML 中 `import 你的模块名 1.0`，即可直接用 `Theme.xxx`
- **做成相对路径导入**：把 Theme 放到某个目录下，在需要使用的文件里 `import "theme"` 或 `import "theme" as X`（这更适合小项目或早期）

---

## 4. 组件里怎么用 Theme（不写死样式）

推荐让组件同时支持：

- 默认使用 Theme（保持一致性）
- 外部覆盖颜色/圆角（保留可定制性）

```qml
Rectangle {
    id: root

    property color backgroundColor: Theme.color.primary
    property color textColor: Theme.color.textPrimary
    property real cornerRadius: Theme.radius.m
    property int paddingH: Theme.spacing.m

    radius: cornerRadius
    color: backgroundColor
}
```

---

## 5. 深色/浅色怎么做（思路）

最稳妥的方式是让 Theme 里有一个模式开关：

- `property bool dark: true`
- 所有色值用表达式根据 `dark` 选择

这样 UI 绑定到 Theme 的属性后，切换模式时界面自然自动刷新。

---

## 常见坑

- 把“具体色值”当作 API 暴露：后期换肤会很痛；优先暴露语义色
- Theme 里 token 太多：先把最常用的 10~20 个 token 做好，够用再扩展
- 组件内部仍然写死 `"#xxxxxx"`：会让 Theme 失去意义

---

## 小结

- Theme 的本质是“语义 token”，用它来统一视觉并降低换肤成本
- 组件默认用 Theme，同时保留外部可覆盖能力
- 不用 QtQuick.Controls 也完全可以做出稳定一致的样式系统

下一篇：见 [08-资源与工程组织：qrc、qmldir、QML module、CMake](08-资源与工程组织-qrc-qmldir-QMLModule-CMake.md)
