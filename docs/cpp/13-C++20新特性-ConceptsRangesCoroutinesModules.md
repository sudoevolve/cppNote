# 13-C++20 新特性：Concepts、Ranges、Coroutines、Modules（入门）

目录页：见 [C++ 小白教程目录](00-目录.md)

## 目标

- 知道 C++20 的“核心升级点”分别解决什么问题
- 会写最基础的 Concepts 约束与 Ranges 管道
- 对 Coroutines 与 Modules 有清晰直觉，知道适用场景与现实限制

## 先决知识（建议）

- 模板与函数重载的基本直觉（见 02 章、05 章）
- STL 容器与算法（见 07、08 章）

---

## 0. 先给结论：C++20 到底“升级了什么”

如果你只想抓重点，可以先记住这 4 句：

- 模板：Concepts 让“能不能用这个类型”变得可读、可控
- 组合：Ranges 让“过滤/变换/遍历”写成流水线
- 表达：`constexpr` 进化让更多逻辑能在编译期做完
- 工程：Modules 试图从根上解决头文件的老问题（但落地要看工具链）

除此之外，C++20 还有一些“写起来很爽、也很常用”的小升级（这章后面会补齐）。

---

## 1. Concepts：给模板加“可读的类型要求”

- 专业名称：概念（Concepts）
- 类比：函数参数的“入场券”，写清楚“什么类型可以进来”
- 作用：让模板报错更友好，重载选择更明确
- 规则/坑点：Concepts 不是运行时检查，是编译期约束；约束写得太死会降低复用

### 1.1 用 `requires` 写最小约束

下面的函数只接受“能相加且结果还是同一类型”的类型：

```cpp
#include <concepts>

template <typename T>
requires requires(T a, T b) { a + b; }
T add(T a, T b) {
    return a + b;
}
```

你会看到：如果把 `add(std::string{}, 1)` 这类不合理调用写出来，报错通常会比 C++17 的“模板爆炸”更聚焦在“约束不满足”。

### 1.2 直接用标准库的概念：`std::integral` / `std::floating_point`

```cpp
#include <concepts>

template <std::integral T>
T gcd(T a, T b) {
    while (b != 0) {
        T r = a % b;
        a = b;
        b = r;
    }
    return a;
}
```

### 1.3 Concepts 的真实价值：报错与重载选择更清晰

你以后会遇到这种场景：

- 你写了一个模板函数 A（泛型）
- 你又写了一个更具体的模板函数 B（只想让“整数类型”走这条路）

Concepts 让“谁更合适”更可控，也能把错误聚焦成“约束不满足”，而不是一长串模板实例化痕迹。

---

## 2. Ranges：把“遍历 + 过滤 + 变换”写成流水线

- 专业名称：范围库（Ranges）
- 类比：数据处理流水线：先筛选，再加工，最后消费（输出/统计）
- 作用：少写迭代器样板代码，让组合更自然
- 规则/坑点：很多 view 是惰性求值；临时对象生命周期要小心（dangling）

### 2.1 最常用：views 管道 + ranges 算法

例子：把一组数里偶数挑出来，平方后输出。

```cpp
#include <iostream>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6};

    auto pipeline = v
        | std::views::filter([](int x) { return x % 2 == 0; })
        | std::views::transform([](int x) { return x * x; });

    for (int x : pipeline) {
        std::cout << "x=" << x << '\n';
    }
    return 0;
}
```

### 2.2 常见坑：对临时容器做 view

如果你把 view 建立在“马上就要销毁”的临时对象上，后面再遍历就可能出问题。简单原则：

- view 尽量建立在“有名字的对象”上（比如 `std::vector<int> v`）
- 或者立刻把结果物化成容器（C++23 有更顺手的 `std::ranges::to`，见下一章）

### 2.3 你会经常用到的 view 组合

最常见的三个：

- `filter`：筛选
- `transform`：变换
- `take` / `drop`：截取前 N 个 / 丢掉前 N 个

```cpp
#include <iostream>
#include <ranges>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};

    auto first3SquaresOfOdd = v
        | std::views::filter([](int x) { return x % 2 == 1; })
        | std::views::transform([](int x) { return x * x; })
        | std::views::take(3);

    for (int x : first3SquaresOfOdd) std::cout << "x=" << x << '\n';
    return 0;
}
```

---

## 3. Coroutines：把“暂停与继续”变成语言能力

- 专业名称：协程（Coroutines）
- 类比：函数可以中途按下暂停键，之后从同一位置继续
- 作用：用更直观的写法表达“异步/生成器/状态机”
- 规则/坑点：C++20 提供的是底层机制，不是现成的 async 框架；很多实际用法依赖库封装

### 3.1 一个最小生成器（`co_yield`）

这个例子演示“逐个产生值”。它是可编译的最小骨架，但更偏底层（目的是建立直觉）。

```cpp
#include <coroutine>
#include <iostream>

struct Generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        int current_value{};

        Generator get_return_object() { return Generator{handle_type::from_promise(*this)}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    handle_type h{};

    explicit Generator(handle_type h_) : h(h_) {}
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept : h(other.h) { other.h = {}; }
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (h) h.destroy();
            h = other.h;
            other.h = {};
        }
        return *this;
    }

    ~Generator() {
        if (h) h.destroy();
    }

    bool next() {
        if (!h || h.done()) return false;
        h.resume();
        return !h.done();
    }

    int value() const { return h.promise().current_value; }
};

Generator range3() {
    co_yield 10;
    co_yield 20;
    co_yield 30;
}

int main() {
    auto g = range3();
    while (g.next()) {
        std::cout << "v=" << g.value() << '\n';
    }
    return 0;
}
```

什么时候值得用协程：

- 你本来就要写状态机（很多分支 + 需要“记住上次走到哪”）
- 你要表达“异步流程”，并且你所用的库/框架已经把协程封装成 `Task/awaitable` 这种易用接口

### 3.2 现实建议：把协程当成“底层机制”

对初学者更实用的心智模型是：

- 语言只给你“co_await/co_yield/co_return + promise/handle”
- 真正好用的 async/IO/网络协程，通常来自库（比如某些网络库/游戏引擎/任务系统）

所以你在学习阶段要达成的目标不是“手写协程框架”，而是：

- 看懂别人代码里 `co_await` 在表达“等待某件事完成”
- 能判断一个 API 是不是“可等待的”（awaitable）

---

## 4. Modules：替代头文件的“更干净的导入”

- 专业名称：模块（Modules）
- 类比：把“接口”打包成模块，使用者用 `import` 拿到它
- 作用：减少头文件重复解析，提高大型项目编译速度，避免宏污染
- 规则/坑点：生态仍在推进中；不同编译器与构建系统支持细节差异很大

### 4.1 一个最小示意

文件 1：`mymath.ixx`（模块接口）

```cpp
export module mymath;

export int add(int a, int b) {
    return a + b;
}
```

文件 2：`main.cpp`

```cpp
import mymath;
#include <iostream>

int main() {
    std::cout << "add=" << add(2, 3) << '\n';
    return 0;
}
```

现实建议：

- 学习阶段先把 Modules 当作“知道有这回事”，项目里先用你工具链稳定支持的方式
- 真正想用 Modules 时，优先从“只给自家库做模块化导出”开始，不要一上来就模块化整个工程

---

## 5. C++20 其他高频语言特性（你很快就会用到）

这一节是“刷存在感”的：不靠它们你也能写程序，但学会以后会明显更顺手。

### 5.1 `std::span`：把“指针 + 长度”打包成安全视图

- 专业名称：`std::span<T>`（span）
- 类比：数组的“窗口”，只看一段连续内存，不拥有数据
- 作用：函数参数更清晰（不用裸指针 + size 两个参数）
- 规则/坑点：span 不拥有数据；底层数据必须在 span 使用期间活着

```cpp
#include <iostream>
#include <span>
#include <vector>

int sum(std::span<const int> s) {
    int ans = 0;
    for (int x : s) ans += x;
    return ans;
}

int main() {
    std::vector<int> v{1, 2, 3, 4};
    std::cout << "sum=" << sum(v) << '\n';
    return 0;
}
```

### 5.2 三路比较 `<=>`：让比较运算“自动补齐”

- 专业名称：三路比较（spaceship operator）
- 类比：比较一次就能得出“小于/等于/大于”
- 作用：写一个 `<=>`，编译器能生成 `==`、`<` 等比较（按规则）
- 规则/坑点：生成规则与成员字段顺序有关；遇到浮点要小心 NaN

```cpp
#include <compare>

struct Point {
    int x{};
    int y{};
    auto operator<=>(const Point&) const = default;
};
```

### 5.3 指定初始化（designated initializers）：让聚合初始化更不容易写错

```cpp
struct User {
    int id;
    int score;
};

int main() {
    User u{.id = 1, .score = 100};
    (void)u;
}
```

### 5.4 `consteval` / `constinit`：更明确的“编译期”语义

- `consteval`：这个函数必须在编译期求值（强约束）
- `constinit`：这个对象必须用“常量初始化”（避免静态初始化顺序问题）

```cpp
consteval int twice(int x) { return x * 2; }

constinit int g_value = 42;

int main() {
    constexpr int k = twice(21);
    (void)k;
}
```

### 5.5 `std::format`：更清晰的格式化（支持情况要看环境）

```cpp
#include <format>
#include <iostream>

int main() {
    std::cout << std::format("x={} y={}\n", 1, 2);
    return 0;
}
```

---

## 6. C++20 并发与线程：更容易“正确停止线程”

这一块对初学者最有用的不是“更快”，而是“更容易写对”。

- `std::jthread`：自动 join（更安全）
- `std::stop_token`：让线程能被请求停止（比自己造一个全局 bool 更稳）

```cpp
#include <chrono>
#include <iostream>
#include <stop_token>
#include <thread>

int main() {
    std::jthread t([](std::stop_token st) {
        int i = 0;
        while (!st.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            ++i;
        }
        std::cout << "stopped i=" << i << '\n';
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    t.request_stop();
    return 0;
}
```

## 常见坑（C++20 这一章的）

- 错误：把 Concepts 当成运行时检查 → 结果：误以为能捕获动态错误 → 正确：它只在编译期约束模板实例化
- 错误：view 建在临时对象上 → 结果：遍历时访问悬空数据 → 正确：先让数据有稳定生命周期，再建立 view
- 错误：自己裸写 coroutine 框架当日常工具 → 结果：代码复杂、维护成本高 → 正确：优先用成熟库的封装接口
- 错误：把 Modules 当成“开关一开就万事大吉” → 结果：构建系统不兼容、第三方库卡住 → 正确：小步迁移

## 小练习

### 练习 1：Concepts 约束

- 输入/输出：无（编译期练习）
- 约束：写一个模板函数 `square`，只允许整数与浮点
- 提示：优先用 `std::integral` 与 `std::floating_point`
- 目标：体会“约束不满足”时的报错质量

### 练习 2：Ranges 管道

- 输入/输出：输入一串整数，输出其中所有大于 0 的数的立方
- 约束：必须使用 `views::filter` + `views::transform`
- 提示：先把输入读进 `std::vector<int>`
- 目标：熟悉 view 管道与惰性求值

---

## 小结

- Concepts 让模板的“类型要求”变得可读、可控
- Ranges 让遍历与组合更自然，但要注意 view 与生命周期
- Coroutines 是底层机制，日常使用依赖库封装
- Modules 是大工程的长期方向，但工具链成熟度要现实评估
- 你最该先掌握的是：Concepts（写模板更可控）与 Ranges（组合遍历更自然）

下一篇：见 [14-C++23 新特性：标准库更好用与语法补强（入门）](14-C++23新特性-标准库更好用与语法补强.md)
