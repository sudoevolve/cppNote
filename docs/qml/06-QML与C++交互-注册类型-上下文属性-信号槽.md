# 06-QML 与 C++ 交互：注册类型/上下文属性/信号槽

目录页：见 [QML 教程目录](00-目录.md)

## 目标

- 搞清 QML 与 C++ 交互的三种主流方式：注册类型、上下文属性、单例
- 会写一个最小 `QObject`，用 `Q_PROPERTY + signal` 驱动 QML UI
- 能把 QML 事件回传给 C++（信号槽 / Q_INVOKABLE）

说明：本章以 Qt 6 + CMake 的典型工程为背景；示例代码为“结构模板”，细节按你的工程实际文件名调整即可。

---

## 1. 三种方式怎么选

### 1.1 qmlRegisterType：QML 里 new 一个 C++ 类型

适用：

- 你希望一个类型能被多处创建（每处一个实例）
- 你希望它像 QML 类型一样被 import 使用（`import MyApp 1.0`）

核心：在 `main.cpp` 注册类型，在 QML 里直接写 `MyType { }`。

### 1.2 上下文属性：把一个现成对象“塞给 QML”

适用：

- 全局状态对象（例如 AppState、Settings、Router）
- 生命周期由 C++ 管（QML 只引用，不负责创建销毁）

核心：`engine.rootContext()->setContextProperty("appState", &appState);`，然后 QML 里直接用 `appState.xxx`。

### 1.3 单例：QML import 一个全局对象/工具

适用：

- 纯工具/配置（如 Theme、Logger、BuildInfo）
- 你想保证整个进程只有一个实例，并且 QML 以模块方式 import

核心：`qmlRegisterSingletonInstance` 或 `qmlRegisterSingletonType`。

---

## 2. 写一个最小 QObject：属性 + 变更信号

一个可被 QML 使用的 C++ 对象通常具备：

- `Q_OBJECT`
- 用 `Q_PROPERTY` 暴露属性
- 属性改变时发出 `xxxChanged` 信号（QML 绑定依赖它自动更新）
- 需要从 QML 调用的方法：`Q_INVOKABLE` 或 `public slots`

示例（头文件）：

```cpp
#pragma once

#include <QObject>

class AppState : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit AppState(QObject* parent = nullptr) : QObject(parent) {}

    int count() const { return m_count; }
    void setCount(int v) {
        if (m_count == v) {
            return;
        }
        m_count = v;
        emit countChanged();
    }

    Q_INVOKABLE void increment() { setCount(m_count + 1); }

signals:
    void countChanged();

private:
    int m_count = 0;
};
```

---

## 3. main.cpp：注册类型 / 设置上下文属性

### 3.1 上下文属性方式（推荐用于全局状态）

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "AppState.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AppState appState;
    engine.rootContext()->setContextProperty("appState", &appState);

    const QUrl url(u"qrc:/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject* obj, const QUrl& objUrl) {
                         if (!obj && url == objUrl) {
                             QCoreApplication::exit(-1);
                         }
                     }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
```

QML 使用：

```qml
import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 420
    height: 240
    visible: true

    Rectangle {
        anchors.fill: parent
        color: "#0f0f10"

        Rectangle {
            anchors.centerIn: parent
            width: 240
            height: 76
            radius: 14
            color: "#202124"

            Text {
                anchors.centerIn: parent
                text: "count = " + appState.count
                color: "#ffffff"
                font.pixelSize: 18
            }

            MouseArea {
                anchors.fill: parent
                onClicked: appState.increment()
            }
        }
    }
}
```

### 3.2 qmlRegisterType：把类型变成 QML 可 import 的模块类型

```cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "AppState.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qmlRegisterType<AppState>("MyApp", 1, 0, "AppState");

    QQmlApplicationEngine engine;
    engine.load(QUrl(u"qrc:/main.qml"_qs));

    return app.exec();
}
```

QML 使用：

```qml
import QtQuick 2.15
import QtQuick.Window 2.15
import MyApp 1.0

Window {
    visible: true
    width: 420
    height: 240

    AppState { id: appState }
    Text { text: "count = " + appState.count }
}
```

---

## 4. 信号槽：让 C++ 推事件到 QML

QML 里对 `QObject` 信号的常用接法：

- 直接写 `onXxxChanged: { ... }`
- 或用 `Connections { target: obj; function onXxx(...) { ... } }`

当你希望“信号名不是属性变更信号”时，`Connections` 更通用：

```qml
Connections {
    target: appState
    function onCountChanged() {
        console.log("count changed:", appState.count)
    }
}
```

---

## 5. 常见坑

- C++ 属性变了但 QML 不更新：大概率是没发 `NOTIFY` 信号，或信号没按规范命名/没 emit
- 对象生命周期问题：把临时对象塞给 QML（函数局部变量）会导致悬空指针；全局/成员对象更安全
- 在非 GUI 线程更新 QML 相关对象：应通过 `QMetaObject::invokeMethod` 或 signal 到主线程处理

---

## 小练习

### 练习 1：做一个 Settings 对象

- 题目：写一个 `Settings`，暴露 `property string userName` 给 QML，并提供 `Q_INVOKABLE void setUserName(string)`
- 目标：练 Q_PROPERTY 与 Q_INVOKABLE

### 练习 2：从 QML 触发 C++ 信号

- 题目：在 C++ 里加一个 `signal toastRequested(QString message)`，QML 点击按钮触发并显示日志
- 目标：练事件通路：QML -> C++ -> QML

---

## 小结

- 注册类型：让 C++ 类型成为 QML 类型
- 上下文属性：把现成对象注入 QML（全局状态最常用）
- Q_PROPERTY + NOTIFY 是“绑定自动更新”的关键

下一篇：见 [07-主题与配色体系：用 Theme 管理颜色、字体、圆角](07-主题与配色体系-Theme管理颜色字体圆角.md)
