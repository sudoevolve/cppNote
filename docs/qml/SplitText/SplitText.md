# SplitText

一段文本按字从左到右依次向上浮现（opacity + 位移），用于标题/提示语/引导文案的“丝滑出场”效果。

---

## 最小用法

```qml
SplitText {
    text: "第一行\n第二行\n第三行"
    color: "#000000"
    duration: 420
    stagger: 40
    rise: 18
    loop: true
}
```

如果你想手动控制开始/停止：

```qml
SplitText {
    id: st
    autoStart: false
    text: "Hello\nQtCraft"
}

// st.start()
// st.stop()
// st.restart()
```

---

## 属性

| 属性名 | 类型 | 默认值 | 可读写 | 说明 |
| --- | --- | --- | --- | --- |
| text | string | "" | 读写 | 文本内容。用 `\n` 分行；每行按字从左到右依次浮现 |
| color | color | "#000000" | 读写 | 文本颜色 |
| font | font | (默认) | 读写 | 字体设置（会应用到每一行的 Text） |
| lineSpacing | real | 6 | 读写 | 行间距 |
| horizontalAlignment | int | Text.AlignLeft | 读写 | 水平对齐方式（Text.AlignLeft/AlignHCenter/AlignRight） |
| duration | int | 420 | 读写 | 每一行的动画时长（毫秒） |
| stagger | int | 40 | 读写 | 相邻两个字符的延迟间隔（毫秒） |
| rise | real | 18 | 读写 | 每个字符起始向下偏移量（像素），动画会向上回到 0 |
| autoStart | bool | true | 读写 | text 变化或组件创建时是否自动播放 |
| loop | bool | false | 读写 | 是否循环播放 |
| loopDelay | int | 600 | 读写 | 每次播放结束到下一次开始的等待（毫秒） |
| running | bool | false | 只读 | 当前是否处于播放状态 |

---

## 信号

| 信号 | 参数 | 说明 |
| --- | --- | --- |
| finished() | 无 | 播放完成（最后一行完成浮现） |

---

## 方法

| 方法 | 参数 | 返回 | 说明 |
| --- | --- | --- | --- |
| start() | 无 | void | 开始播放（从头开始逐行浮现） |
| stop() | 无 | void | 停止并重置为未显示状态 |
| restart() | 无 | void | 等价于 `stop(); start()` |

---

## 备注 / 坑点

- 组件按 `\n` 分行显示；如果你需要“自动按宽度换行再逐行浮现”，建议先把文本按你的排版规则预处理成带 `\n` 的字符串再传入。
- 如果你看到“一片白”，最常见是两种原因：窗口背景是白色同时你把文字颜色也设成了白色；或者你没给组件任何尺寸/锚点导致容器为 0。建议先做最小排查：给 Window 加深色背景（或把 `color` 改黑），并给 SplitText 加 `anchors.centerIn: parent`。
- 组件会在有明确 `width/height` 时自动裁剪，避免起始位移时超出区域；如果你希望溢出也可见，可以在外层包一层 Item 并关闭外层裁剪。
- 本组件不使用 `QtQuick.Controls`。
