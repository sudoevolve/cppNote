# 05-Widget 入门：窗口、按钮、布局

目录页：见 [Qt 教程目录](00-目录.md)

## 目标

- 能独立写出一个最简单的窗口程序（不用 Designer 也能写）
- 理解布局（Layout）解决了什么：让界面“自动排版”
- 学会三种最常用布局：竖排、横排、网格

## 先决知识（可选）

- 会创建工程并运行（见 02 章）
- 理解信号与槽（见 04 章）

---

## 1. QWidget 是什么：界面上的“积木块”

- 专业名称：QWidget（窗口部件）
- 类比：乐高积木，一块块拼起来就是你的界面
- 作用：按钮、文本框、列表、窗口本身都可以是 QWidget（或它的子类）
- 规则/坑点：父子关系决定生命周期：父控件销毁会自动销毁子控件

---

## 2. 写一个最小窗口（可运行）

```cpp
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget w;
    w.resize(360, 240);
    w.setWindowTitle("Hello Widget");
    w.show();

    return app.exec();
}
```

你要记住三件事：
- GUI 程序需要 `QApplication`
- `show()` 才会显示
- `app.exec()` 进入事件循环（程序就“活着”了）

---

## 3. 按钮与标签：最基本的交互

我们做一个“点按钮改变文字”的小例子：

```cpp
#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget w;
    w.setWindowTitle("Button + Label");

    auto *layout = new QVBoxLayout(&w);
    auto *label = new QLabel("ready");
    auto *btn = new QPushButton("Click");

    layout->addWidget(label);
    layout->addWidget(btn);

    QObject::connect(btn, &QPushButton::clicked, &w, [=] {
        label->setText("clicked");
    });

    w.show();
    return app.exec();
}
```

注意这里布局做了两件很重要的事：
- 帮你摆放位置
- 帮你管理子控件的大小变化（窗口变大，控件怎么跟着变）

---

## 4. 布局（Layout）：让界面“自己排好”

- 专业名称：Layout（布局管理器）
- 类比：收纳盒的格子规则：你只说“放进第几格”，它帮你自动排
- 作用：适配不同窗口大小、不同字体大小、不同 DPI
- 规则/坑点：不要用 `move(x,y)` 把控件硬摆死，新手一开始就养成用布局的习惯

### 4.1 QVBoxLayout：竖排（最常用）

类比：从上到下排队。

### 4.2 QHBoxLayout：横排

类比：从左到右排队。

### 4.3 QGridLayout：网格

类比：表格，指定行列坐标。

---

## 5. QWidget vs QMainWindow：主窗口用哪个？

- QWidget：通用积木，轻量
- QMainWindow：主窗口“套装”，自带菜单栏、工具栏、状态栏、中央区域

新手结论：
- 小 demo 用 QWidget 就够
- 正经桌面软件常用 QMainWindow，当你需要菜单/工具栏时再用

---

## 常见坑

- 错误：布局 + 手动 move/resize 同时用  
  结果：控件位置大小看起来“不听话”  
  正确：用了布局就把摆放交给布局，不要再手动移动

- 错误：没有设置父对象就 new 出一堆控件  
  结果：内存泄漏或生命周期难管  
  正确：让布局/父控件接管子控件（例子里 `new QVBoxLayout(&w)` 就是关键）

---

## 小练习

### 练习 1：做一个登录面板（不需要真的登录）

- 题目：两个输入框（用户名/密码）+ 一个按钮（Login）
- 输入/输出：点击按钮后把用户名显示到一个标签上
- 约束：必须使用布局（VBox + HBox 或 Grid）
- 提示：用 `QLineEdit`，密码框用 `setEchoMode(QLineEdit::Password)`
- 目标：练布局 + 最基本交互

### 练习 2：做一个两列网格

- 题目：用 QGridLayout 做一个 3 行 2 列表单
- 输入/输出：窗口拉伸时控件跟着伸缩
- 约束：禁止固定坐标摆放
- 提示：左边 QLabel，右边 QLineEdit
- 目标：熟悉网格布局

---

## 小结

- QWidget 是界面“积木”；父子关系决定生命周期
- 布局让界面自动排版，优先用布局而不是固定坐标
- QMainWindow 适合正式主窗口（菜单/工具栏/状态栏），小 demo 用 QWidget 更轻

下一篇：见 [06-资源系统-qrc图片字体翻译](06-资源系统-qrc图片字体翻译.md)
