# 08-绘制基础：QPainter 画图与自定义控件

目录页：见 [Qt 教程目录](00-目录.md)

## 目标

- 理解“自定义绘制”在做什么：你负责把像素画出来
- 会在 QWidget 里重写 `paintEvent` 用 QPainter 画基本图形
- 做一个最小自定义控件：画一个进度条/仪表盘

## 先决知识（可选）

- 会搭一个简单窗口（见 05 章）

---

## 1. QPainter 是什么：你的“画笔”

- 专业名称：QPainter（2D 绘制引擎接口）
- 类比：画画：你拿笔、选颜色、画线、画矩形、写字
- 作用：在控件上绘制图形、文本、图片；实现自定义 UI
- 规则/坑点：绘制发生在 `paintEvent` 里；不要在 paintEvent 里做耗时计算；需要抗锯齿时显式开启

---

## 2. paintEvent 是什么：系统让你“该重画了”

- 专业名称：paintEvent（绘制事件）
- 类比：系统对你说“现在这块区域需要重新画一遍”
- 作用：让你的控件在需要时正确显示（窗口遮挡、缩放、内容变化等都会触发）
- 规则/坑点：不要直接调用 paintEvent；更新时用 `update()` 请求重绘

---

## 3. 最小自定义控件：画一个圆形进度（可运行）

这个例子做的事情：
- 一个自定义 QWidget，里面有一个 `value`（0~100）
- paintEvent 里用 QPainter 画背景圆 + 进度弧线 + 文字

```cpp
#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

class Ring : public QWidget {
public:
    explicit Ring(QWidget *parent = nullptr) : QWidget(parent) {}

    void setValue(int v) {
        if (v < 0) v = 0;
        if (v > 100) v = 100;
        if (value == v) return;
        value = v;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);

        QRectF r = rect().adjusted(12, 12, -12, -12);
        QPointF c = r.center();
        qreal radius = qMin(r.width(), r.height()) / 2.0;

        QPen bg(QColor("#e6e6e6"), 10);
        bg.setCapStyle(Qt::RoundCap);
        p.setPen(bg);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(c, radius, radius);

        QPen fg(QColor("#4c8bf5"), 10);
        fg.setCapStyle(Qt::RoundCap);
        p.setPen(fg);
        int span = static_cast<int>(-360.0 * 16.0 * (value / 100.0));
        p.drawArc(r, 90 * 16, span);

        p.setPen(QColor("#222222"));
        QFont f = font();
        f.setPointSize(16);
        p.setFont(f);
        p.drawText(rect(), Qt::AlignCenter, QString("%1%").arg(value));
    }

private:
    int value = 0;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget w;
    w.setWindowTitle("QPainter Ring");
    auto *layout = new QVBoxLayout(&w);
    auto *ring = new Ring;
    ring->setMinimumSize(220, 220);
    layout->addWidget(ring);

    int v = 0;
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &w, [&] {
        v = (v + 1) % 101;
        ring->setValue(v);
    });
    timer.start(30);

    w.show();
    return app.exec();
}
```

你需要抓住两点：
- `update()` 不是立刻画，它是“请求系统稍后重绘”
- 真正的画发生在 `paintEvent` 的 QPainter 里

---

## 4. 坐标与单位：你画的是“控件坐标系”

直觉记法：
- `(0,0)` 在左上角
- x 向右增大，y 向下增大
- `rect()` 表示当前控件区域

规则/坑点：
- DPI 不同会导致物理像素不同，但 Qt 会尽量帮你做缩放适配
- 你如果画得发虚，优先检查是否启用了抗锯齿，以及线宽/坐标是不是半像素导致模糊

---

## 常见坑

- 错误：在 paintEvent 里做网络请求/文件 I/O/复杂计算  
  结果：界面卡顿、拖动窗口都很慢  
  正确：把耗时工作放到别处，paintEvent 只负责画（数据准备好后 `update()`）

- 错误：希望“改变变量就自动刷新”，但没调用 `update()`  
  结果：值变了但画面不变  
  正确：数据变化后调用 `update()` 触发重绘

---

## 小练习

### 练习 1：做一个横向进度条

- 题目：自定义控件，画一个带圆角的横向进度条
- 输入/输出：value 变化时进度条长度变化
- 约束：必须在 paintEvent 里用 QPainter 画
- 提示：用 `drawRoundedRect`
- 目标：练基本绘制 API

### 练习 2：加一层渐变

- 题目：把进度弧线改成渐变色
- 输入/输出：颜色从蓝到紫变化
- 约束：必须用 QLinearGradient/QRadialGradient
- 提示：画笔 `QPen` 的 brush 可以用渐变
- 目标：练“画笔的刷子”

---

## 小结

- 自定义绘制=你在 paintEvent 里用 QPainter 把图画出来
- 数据变化后用 `update()` 请求重绘
- 避免在 paintEvent 做耗时工作，保证界面丝滑

下一篇：见 [09-线程与任务-不卡UI的正确姿势](09-线程与任务-不卡UI的正确姿势.md)
