# 06-资源系统：qrc 图片、字体、翻译

目录页：见 [Qt 教程目录](00-目录.md)

## 目标

- 理解 `.qrc` 解决了什么：资源跟着程序走，不怕发布丢文件
- 会把图片、字体加入资源，并在代码里正确引用 `:/...`
- 知道翻译（.ts/.qm）的大概流程，能做出最小“中英切换”

## 先决知识（可选）

- 会写最简单的 Widgets 界面（见 05 章）

---

## 1. 资源系统是什么：把素材打包进可执行文件

- 专业名称：Qt Resource System（Qt 资源系统）
- 类比：旅行时把衣服装进箱子，出门后不需要再去原衣柜拿
- 作用：图片/图标/字体/翻译文件跟着程序走，路径统一、发布更稳
- 规则/坑点：资源路径是 `:/...`，不是磁盘路径；只有进了 `.qrc` 才能用 `:/...`

---

## 2. qrc 怎么用：三步

### 2.1 创建资源文件

在 Qt Creator 里：
- Add New... → Qt → Qt Resource File（会生成一个 `.qrc`）

### 2.2 往 qrc 里加文件

把你的 `logo.png`、`icon.svg`、`font.ttf` 等加入 `.qrc`。

你会在 `.qrc` 里看到类似结构（示意）：
- prefix：资源前缀（像目录）
- files：被打包的文件列表

### 2.3 在代码里用 `:/` 路径引用

比如图片：

```cpp
#include <QLabel>
#include <QPixmap>

auto *label = new QLabel;
label->setPixmap(QPixmap(":/images/logo.png"));
```

比如窗口图标：

```cpp
#include <QIcon>
#include <QWidget>

QWidget w;
w.setWindowIcon(QIcon(":/icons/app.png"));
```

---

## 3. 字体打包与使用：不依赖用户电脑装没装

- 专业名称：QFontDatabase（字体数据库）
- 类比：把字体文件“注册进系统的临时清单”，你才能用字体名去选它
- 作用：发布时字体不缺失，跨机器一致
- 规则/坑点：先 `addApplicationFont` 再 `setFont`；加载失败要能退回默认字体

最小用法（假设你已把 `fonts/MyFont.ttf` 加进 qrc）：

```cpp
#include <QApplication>
#include <QFontDatabase>
#include <QLabel>

int id = QFontDatabase::addApplicationFont(":/fonts/MyFont.ttf");
QString family = id >= 0 ? QFontDatabase::applicationFontFamilies(id).value(0) : QString();

QLabel label("Hello");
if (!family.isEmpty()) {
    QFont f(family);
    f.setPointSize(18);
    label.setFont(f);
}
label.show();
```

---

## 4. 翻译（国际化）：让文本能切语言

- 专业名称：Internationalization（i18n，国际化） / Localization（l10n，本地化）
- 类比：同一本书的不同语言版本，内容一样但文字不同
- 作用：同一套程序支持中文/英文等多语言
- 规则/坑点：翻译是“运行时加载”，不是编译时写死；要用 `tr()` 标记可翻译文本

### 4.1 最小流程（概念版）

1. 在代码里把需要翻译的字符串写成 `tr("...")`
2. 用工具从源码里提取出 `.ts`（翻译源文件）
3. 在 Qt Linguist 里把 `.ts` 翻译成目标语言
4. 把 `.ts` 编译成 `.qm`（运行时加载的二进制翻译包）
5. 程序启动时用 `QTranslator` 加载 `.qm`

### 4.2 运行时加载（最小代码）

假设你把 `i18n/app_zh_CN.qm` 加进 qrc：

```cpp
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTranslator tr;
    tr.load(":/i18n/app_zh_CN.qm");
    app.installTranslator(&tr);

    return app.exec();
}
```

---

## 常见坑

- 错误：用磁盘路径读图片（例如 `C:\Users\...\a.png`）  
  结果：发布到别人电脑就找不到  
  正确：把资源放进 `.qrc`，用 `:/...`（见第 2 节）

- 错误：字体加载后直接写一个“字体文件名”当 family 用  
  结果：字体不生效  
  正确：用 `QFontDatabase::applicationFontFamilies` 拿到 family 再设置（见第 3 节）

- 错误：字符串没用 `tr()`，翻译工具提取不到  
  结果：翻译文件里没有这句  
  正确：界面文本统一用 `tr()` 标记

---

## 小练习

### 练习 1：窗口图标来自 qrc

- 题目：给你的窗口设置一个图标
- 输入/输出：窗口左上角有图标
- 约束：图标必须来自 `:/...`
- 提示：`setWindowIcon(QIcon(":/icons/app.png"))`
- 目标：练 qrc 引用路径

### 练习 2：自定义字体标题

- 题目：把一个 ttf 字体加入 qrc，并用于显示一个大标题
- 输入/输出：标题字体和系统默认明显不同
- 约束：字体必须通过 `addApplicationFont` 加载
- 提示：family 可能不止一个，用 `value(0)` 先取第一个
- 目标：练字体打包与使用

---

## 小结

- `.qrc` 把资源打包进程序，代码里用 `:/...` 引用更稳定
- 字体用 `QFontDatabase` 注册后再用 family 设置
- 翻译的关键是：`tr()` 标记 + `.qm` 运行时加载

下一篇：见 [07-常用控件-输入列表对话框](07-常用控件-输入列表对话框.md)
