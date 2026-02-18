# QML 单页小组件规范（qtcraft）

这份文档把你提出的要求固化成可执行规范。后续我不会开始写任何组件，直到你说“开始”。

---

## 1. 目标与范围

- 位置：所有 QML 单页小组件放在 `docs/qml/` 目录下
- 形态：每个组件一个文件夹，文件夹内只包含 **1 个 `.qml`** + **1 个 `.md`**
- 组件定位：单文件、可复用、尽量无外部依赖（不需要工程就能看懂用法）

---

## 2. 目录结构（文件夹格式）

每个组件使用下面的固定结构：

```
docs/qml/
  <ComponentName>/
    <ComponentName>.qml
    <ComponentName>.md
```

命名规则：

- `<ComponentName>` 用 PascalCase（例如 `TagChip`、`IconButton`、`Toast`）
- QML 类型名与文件名保持一致：`TagChip.qml` 里 `TagChip { ... }`

---

## 3. QML 代码要求

### 3.1 禁止使用 QtQuick.Controls

- 禁止：`import QtQuick.Controls`、`import QtQuick.Controls.*`
- 允许（按需）：`import QtQuick`、`import QtQml`、`import QtQuick.Layouts`

### 3.2 每个 QML 文件必须包含版权与 MIT 许可声明

每个 `<ComponentName>.qml` 文件顶部必须包含以下声明（放在所有 `import` 之前）：

```qml
/*
 * Copyright (c) 2026 sudoevolve
 * Copyright (c) 2026 qtcraft
 * SPDX-License-Identifier: MIT
 */
```

说明：

- “sudoevolve + qtcraft” 两个归属必须同时出现
- 许可使用 MIT，并用 `SPDX-License-Identifier: MIT` 作为机器可识别标记

### 3.3 组件工程化要求（单文件也要“像组件”）

- 对外暴露能力：使用 `property` / `signal` / `function` 明确接口
- 默认值合理：组件不设置任何属性时也能正常显示
- 可复用：不要写死父项尺寸；优先用 `implicitWidth/implicitHeight` 或内容自适应
- 依赖收敛：不引入业务数据源；不写网络/文件 I/O

---

## 4. 每个组件的 MD 要求（介绍 + 用法 + 属性声明）

每个 `<ComponentName>.md` 必须包含以下内容（按顺序）：

1. 组件简介：一句话说明“它解决什么问题”
2. 最小用法：给一个最小可用示例
3. 属性列表：列出本组件声明的所有 `property`（必须逐个写清楚）
4. 信号列表（如有）：列出所有 `signal`
5. 方法列表（如有）：列出所有 `function`
6. 备注/坑点：比如尺寸、边界值、性能注意点

属性列表格式统一为表格：

| 属性名 | 类型 | 默认值 | 可读写 | 说明 |
| --- | --- | --- | --- | --- |
| text | string | "" | 读写 | 显示文本 |

最小用法示例必须可直接复制使用（示意）：

```qml
// 假设你把 <ComponentName>.qml 放进某个 QML 工程可 import 的路径
<ComponentName> {
    // 这里展示 1~3 个最常用属性
}
```

---

## 5. 组件清单与节奏

- 我先按这个规范在 `docs/qml/` 下逐个创建组件文件夹与文件
- 你说“开始”之后我才会开始往里面写具体组件内容

