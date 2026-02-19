# 12-Qt 常用类型与自定义函数：QString / QColor 等

目录页：见 [Qt 教程目录](00-目录.md)

## 目标

- 搞清楚“为什么 Qt 有自己的类型”：QString、QColor、QByteArray…
- 学会写“像 Qt 一样好用”的自定义函数：参数怎么传、返回什么、怎么少踩坑
- 用两个最常见的例子（QString/QColor）把直觉建立起来

## 先决知识（可选）

- 会写基本的 C++ 函数与引用（如果你没学过引用，把它理解成“不给对象复制一份，而是借用原件”）

---

## 1. 先搞清楚一件事：Qt 不是“只给你 UI”，它给你一整套基础类型

你写 Qt 程序时会很快遇到这些名字：
- `QString`：字符串
- `QColor`：颜色
- `QByteArray`：字节数组（常用于网络/文件/二进制数据）
- `QDateTime`：时间
- `QUrl`：URL

它们的共同点：

- 专业名称：Qt Core Types（Qt 核心类型）
- 类比：Qt 自带的“通用零件箱”。UI 只是外壳，里面很多地方都用这些零件
- 作用：跨平台一致、功能完整、和 Qt 的信号槽/属性系统更契合
- 规则/坑点：同一个“概念”可能既有 C++ 标准库版本（比如 `std::string`），也有 Qt 版本（比如 `QString`），你要知道在 Qt 生态里什么时候用哪个

---

## 2. QString：为什么不用 std::string？

### 2.1 QString 解决的核心问题：Unicode

- 专业名称：Unicode（统一字符编码）/ UTF-8（常见编码格式）
- 类比：同一段文字有很多语言（中文、日文、emoji），要用一套统一规则才能稳定保存和显示
- 作用：Qt UI 几乎所有文本都要正确显示多语言，所以 Qt 把“字符串”做成了一等公民
- 规则/坑点：`std::string` 本质是“字节容器”，它不关心里面是不是 UTF-8；而 Qt 的很多 API 直接使用 `QString`

直觉结论（新手先记住这个就够）：
- **跟 Qt UI/属性/翻译相关的文本，用 `QString`**
- **跟纯 C++、纯字节处理相关的，用 `std::string` 或 `QByteArray`**

### 2.2 常见转换：QString / std::string / QByteArray

你会经常遇到三者互转，最常用的写法是：

```cpp
#include <QString>
#include <QByteArray>
#include <string>

QString a = "qtcraft";

QByteArray utf8 = a.toUtf8();
std::string s = utf8.toStdString();

QString b = QString::fromUtf8(s.c_str());
```

你可以把它理解成：
- `QString`：适合“文本”
- `QByteArray`：适合“字节”
- `std::string`：也可以装字节，但在 Qt 世界里，很多 API 更直接吃 `QString/QByteArray`

---

## 3. QColor：颜色不只是“一个 #RRGGBB 字符串”

### 3.1 QColor 是什么

- 专业名称：QColor（颜色类型）
- 类比：一个“颜色小盒子”，里面既能装 RGB，也能装透明度（alpha），还能装 HSL 等色彩空间
- 作用：你在设置控件颜色、绘制、主题色时，最常用的就是它
- 规则/坑点：颜色经常带透明度；只用 `"#RRGGBB"` 容易忽略 alpha

最常见的创建方式：

```cpp
#include <QColor>

QColor a("#4c8bf5");              // 从 #RRGGBB 解析
QColor b(76, 139, 245);           // RGB
QColor c(76, 139, 245, 128);      // RGBA，最后一个是 alpha(0~255)
```

### 3.2 QColor 和 QPalette 的区别

你会看到另一个东西：`QPalette`。

- QColor：一个具体颜色值
- QPalette：一套“角色颜色表”（按钮背景、文本颜色、窗口背景等不同角色各用什么颜色）

类比：
- QColor 是“单瓶颜料”
- QPalette 是“整套调色盘规则”

新手结论：
- 想给某个地方指定具体颜色，用 QColor
- 想做主题（整体风格），后面再了解 QPalette（不必急）

---

## 4. 自定义函数怎么写才像 Qt：参数传法比你想的重要

### 4.1 大原则：能不复制就不复制

Qt 的很多类型（例如 `QString`）拷贝很“便宜”，但你写函数时仍然要养成好习惯：

- 输入参数：优先 `const T&`（只读引用）
- 返回值：直接按值返回（让编译器优化/移动语义发挥作用）

类比：
- `const T&`：借用原件看一眼，不复印
- 按值返回：把结果交出去，交付时“快递公司”会尽量优化（编译器优化）

---

## 5. 两个实战：用 QString/QColor 写出“可复用的小函数”

### 5.1 例子 A：把用户输入规范化（QString）

需求：
- 用户输入可能有空格、大小写不统一
- 你想把它规范成 “首字母大写，其余小写”

```cpp
#include <QString>

QString normalizeName(const QString& raw) {
    QString s = raw.trimmed();
    if (s.isEmpty()) {
        return s;
    }
    QString lower = s.toLower();
    lower[0] = lower[0].toUpper();
    return lower;
}
```

你要注意的点：
- `trimmed()` 会去掉两端空白
- 处理空字符串要提前返回

### 5.2 例子 B：把颜色“变暗一点”（QColor）

需求：
- 做按钮按下态：颜色比正常态暗一点

```cpp
#include <QColor>

QColor darken(const QColor& c, int percent) {
    if (percent < 0) percent = 0;
    if (percent > 200) percent = 200;
    return c.darker(100 + percent);
}
```

你可以把 `darker(120)` 理解成：
- 把颜色“压暗”到原来的 120%（数值越大越暗）

---

## 常见坑

- 错误：用 `std::string` 直接当 UI 文本，不处理编码  
  结果：中文乱码、emoji 出问题  
  正确：UI 文本统一用 `QString`，需要字节再转 `toUtf8()`（见 2.2）

- 错误：用字符串拼颜色到处传（`"#RRGGBB"`）  
  结果：透明度、色彩空间、合法性检查都很难做  
  正确：用 `QColor` 在类型层面表达“这是颜色”（见 3.1）

- 错误：函数参数写成按值传 `QString s`，然后在热路径里频繁调用  
  结果：不必要的拷贝开销（虽然 Qt 很优化，但这习惯不利于写大项目）  
  正确：输入参数用 `const QString&`（见 4.1）

---

## 小练习

### 练习 1：生成显示名（QString）

- 题目：输入 firstName 和 lastName，返回 `"Last, First"` 的显示名
- 输入/输出：返回一个 QString
- 约束：必须处理空字符串（有一边空就只返回另一边）
- 提示：用 `trimmed()`、`isEmpty()`
- 目标：练字符串处理与边界情况

### 练习 2：颜色透明度工具（QColor）

- 题目：写一个函数 `withAlpha(color, alpha)` 返回一个新颜色
- 输入/输出：返回 QColor
- 约束：alpha 范围 0~255，超出要裁剪
- 提示：用 `setAlpha(int)`
- 目标：练“颜色是类型，不是字符串”

---

## 小结

- Qt 有自己的核心类型，是为了跨平台一致、和 Qt 生态更顺
- `QString` 面向文本（Unicode），`QByteArray` 面向字节；UI 文本优先用 `QString`
- `QColor` 表达颜色（含透明度），比字符串更可靠
- 自定义函数：输入用 `const T&`，输出按值返回，写起来像 Qt 一样稳

下一篇：你可以回到目录继续扩展主题（例如 QDateTime/QUrl/QVariant 这些也很常用）。

