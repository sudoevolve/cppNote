# 09-性能与调试：Binding 循环、过度创建、Profiler/Scene Graph

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 能识别最常见的 QML 性能问题与“看起来像 bug 的性能抖动”
- 会排查 binding loop、过度创建 Item、列表卡顿
- 知道 Qt Creator / QML Profiler 的常用观察点（思路为主）

约束：本教程示例不使用 `QtQuick.Controls`。

---

## 1. Binding loop：最常见也最隐蔽

典型症状：

- 控制台出现 “Binding loop detected”
- UI 轻微抖动/尺寸跳动
- 某个属性值不稳定（每帧都在变）

常见原因：

- A 依赖 B，B 又依赖 A（直接或间接）
- 在 `onWidthChanged` 里又去改 `width`（事件里写回同属性）

一个容易踩的例子（不要这么写）：

```qml
Rectangle {
    id: box
    width: parent.width
    onWidthChanged: box.width = parent.width
}
```

更稳的写法：

- 用纯绑定表达式描述关系
- 或者把“输入状态”和“UI 表现属性”分离（状态驱动，见第 04 章）

---

## 2. 过度创建 Item：看似简单，实际很重

QML 性能最容易被拖垮的点之一是：

- 一次性创建了太多 Item（尤其是带 Text/Image/复杂动画的）

建议：

- 小数量重复 UI：`Repeater` 没问题
- 大列表：优先 `ListView`（delegate 复用）
- 可见范围之外的内容：用 `Loader` 按需加载

你可以把直觉记成一句话：

- “让屏幕上可见的东西尽量少创建，屏幕外的东西尽量不创建。”

---

## 3. ListView 卡顿：优先排这几类原因

### 3.1 delegate 太重

症状：

- 滑动掉帧
- 首次进入列表页卡一下

处理：

- delegate 里避免深层嵌套
- 少用复杂阴影、复杂路径绘制（能不用就不用）
- 动画尽量只做 opacity/transform（通常更便宜）

### 3.2 图片加载策略不当

处理：

- 大图尽量异步：`Image { asynchronous: true }`
- 避免每个 delegate 都加载大图且每帧缩放

### 3.3 绑定表达式过重

处理：

- 把复杂计算从绑定里抽出去（必要时用中间 property 缓存结果）
- 避免在绑定里做循环/字符串大量拼接

---

## 4. 动画与渲染：不要“每帧改布局”

原则：

- 尽量动画 `opacity/scale/rotation/x/y`（通常更友好）
- 尽量少动画会触发布局重算的属性（例如复杂层级下的 `width/height`）

并不是说不能动画 `width/height`，而是：

- 动画它们时，更要控制层级、避免联动太多绑定

---

## 5. 调试方法（思路清单）

当你遇到“卡/抖/不稳定”时，可以按顺序做：

1. **先看控制台**：有没有 binding loop、类型找不到、加载失败
2. **先缩小范围**：把可疑区域用 `visible: false` 或 `Loader.active: false` 隔离
3. **把重 UI 拆开**：列表 delegate 先删到只剩 Rectangle，看卡顿是否消失
4. **确认是否过度创建**：是否一次性创建了大量子项（Repeater + 大模型）
5. **用 Profiler 定位**：如果你在 Qt Creator，用 QML Profiler 看创建/绑定/动画的热点

---

## 6. Scene Graph 常识（够用级）

你不需要先精通 Scene Graph，但建议记住两点：

- “能合批渲染”的场景更快：过多不同材质/复杂效果会降低合批
- “减少层级与变化源”通常就能明显提升流畅度

---

## 常见坑

- 把调试逻辑写进绑定：绑定会频繁求值，容易造成额外开销
- 在 onXChanged 里疯狂 setProperty：容易引入 loop 与抖动
- 用 Repeater 做几千条列表：应该换 ListView

---

## 小结

- 优先修掉 binding loop，这是很多“玄学问题”的根
- 大数据列表用 ListView，按需加载用 Loader
- 动画优先 opacity/transform，减少每帧触发布局重算

下一篇：见 [10-QML常用元素与属性速查](10-QML常用元素与属性速查.md)
