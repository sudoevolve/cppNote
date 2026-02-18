# 10-网络与 JSON：请求接口与解析数据

目录页：见 [Qt 教程目录](00-目录.md)

## 目标

- 能用 Qt 发送一个最简单的 HTTP GET 请求
- 理解网络请求的本质：异步，结果通过信号回来
- 会用 QJsonDocument/QJsonObject 解析 JSON 字符串

## 先决知识（可选）

- 理解信号与槽（见 04 章）
- 知道“不卡 UI”的基本结论（见 09 章）

---

## 1. Qt 的网络是怎么工作的：默认就是异步

- 专业名称：Asynchronous I/O（异步 I/O）
- 类比：你点外卖下单后不会一直盯着骑手，你继续做别的；外卖到了会通知你（信号）
- 作用：网络等待时 UI 不卡
- 规则/坑点：请求发出后函数立刻返回；真正结果在 reply 的信号里处理

---

## 2. QNetworkAccessManager：网络请求的“总管”

- 专业名称：QNetworkAccessManager
- 类比：快递总台，负责发件、收件、分发通知
- 作用：创建请求、收到响应
- 规则/坑点：manager 要有稳定生命周期（通常放在窗口成员里）；reply 用完要释放（`deleteLater`）

---

## 3. 最小 GET 请求：下载一段文本（可运行）

这个例子把响应文本显示在窗口里。

```cpp
#include <QApplication>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget w;
    w.setWindowTitle("Network Demo");
    auto *layout = new QVBoxLayout(&w);
    auto *label = new QLabel("requesting...");
    label->setWordWrap(true);
    layout->addWidget(label);

    QNetworkAccessManager mgr;
    QNetworkRequest req(QUrl("https://httpbin.org/get"));
    auto *reply = mgr.get(req);

    QObject::connect(reply, &QNetworkReply::finished, &w, [=] {
        QByteArray body = reply->readAll();
        label->setText(QString::fromUtf8(body));
        reply->deleteLater();
    });

    w.resize(640, 420);
    w.show();
    return app.exec();
}
```

你要记住两个点：
- `mgr.get(req)` 立刻返回 `reply`
- 真正的结果在 `reply->finished` 之后取

---

## 4. JSON 是什么：结构化的数据“包裹”

- 专业名称：JSON（JavaScript Object Notation）
- 类比：带标签的快递箱：每个字段都有名字和值
- 作用：接口返回结构化数据，跨语言好解析
- 规则/坑点：JSON 字符串需要解析才能取字段；字段可能缺失，要做健壮处理

---

## 5. 解析 JSON：QJsonDocument/QJsonObject（最小示例）

你可以先从“解析一段字符串”练起：

```cpp
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

QString parseName() {
    QByteArray json = R"({"name":"qtcraft","age":18})";
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return "parse error";
    }
    QJsonObject obj = doc.object();
    return obj.value("name").toString("unknown");
}
```

规则/坑点：
- 先判断 parse 是否成功
- 先判断是不是 object
- 取字段时给默认值（例如 `toString("unknown")`）

---

## 6. 把网络和 JSON 串起来：请求 → 解析 → 更新 UI

写法顺序（新手推荐）：

1. `finished` 里先拿到 `body`
2. 解析 JSON
3. 把你关心的字段拼成一段可读文本
4. 设置到 label / list / table 上

这样做的好处是：
- 每一步都能单独打印/检查，出错更容易定位

---

## 常见坑

- 错误：忘了在工程里链接 Network 模块  
  结果：编译时报找不到 QNetworkAccessManager  
  正确：CMake 里把 `Qt::Network` 加进链接；qmake 里加 `QT += network`

- 错误：reply 用完不释放  
  结果：内存泄漏（请求越多越明显）  
  正确：`reply->deleteLater()`

- 错误：假设 JSON 字段一定存在  
  结果：取到空值、逻辑异常  
  正确：用默认值，并判断类型（例如是 string 还是 number）

---

## 小练习

### 练习 1：请求天气/时间接口并显示关键字段

- 题目：请求一个公开接口，显示其中 2~3 个字段
- 输入/输出：窗口上显示字段值
- 约束：必须异步处理，不允许阻塞 UI
- 提示：先用浏览器看接口返回的 JSON 再写解析
- 目标：练“网络 + JSON + UI”

### 练习 2：错误处理

- 题目：把网络断开或改成一个错误 URL，显示“错误原因”
- 输入/输出：提示错误信息
- 约束：不能崩溃
- 提示：看 `reply->error()` 和 `reply->errorString()`
- 目标：练健壮性

---

## 小结

- Qt 网络请求默认异步：请求发出后，结果通过信号回来
- QNetworkAccessManager 管请求；QNetworkReply 带响应
- JSON 用 QJsonDocument 解析，取字段要做默认值和类型判断

下一篇：见 [11-QML入门-快速搭界面（不使用QtQuick.Controls）](11-QML入门-快速搭界面（不使用QtQuick.Controls）.md)
