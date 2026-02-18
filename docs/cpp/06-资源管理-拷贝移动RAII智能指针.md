# 06-资源管理：拷贝/移动/RAII/智能指针

目录页：见 [C++ 小白教程目录](00-目录.md)

## 目标

- 理解“资源”不止是内存：文件、锁、网络连接都算
- 理解 RAII：让资源跟着对象生命周期自动释放
- 知道拷贝/移动解决什么问题（先理解直觉）
- 会用 `std::unique_ptr`、`std::shared_ptr` 做基本资源管理

## 先决知识

- 理解作用域与生命周期（见 02 章 5）
- 理解指针是什么、悬空指针是什么（见 04 章 2、8）
- 会写类与构造函数（见 05 章 2、3）

---

## 1. 什么是资源：不释放就会“占着不走”

- 专业名称：资源（resource）
- 类比：租来的东西（房间/车/工位/钥匙），用完要归还
- 作用：程序运行时占用的外部东西
- 规则/坑点：资源最怕两件事：忘记释放（泄漏）和重复释放（崩溃）

常见资源：

- 动态内存（new 出来的东西）
- 文件句柄（打开的文件）
- 互斥锁（加锁后必须解锁）
- 网络连接（用完要关闭）

---

## 2. RAII：让资源跟着对象生命周期走

- 专业名称：RAII（Resource Acquisition Is Initialization）
- 类比：办门禁卡：你拿到卡（构造）就能进门；你退卡（析构）就自动退权限
- 作用：把“申请资源”放进构造函数，把“释放资源”放进析构函数，自动、安全
- 规则/坑点：RAII 的核心不是“聪明”，而是“把释放写死在一个必然会执行的地方”

你其实已经用过很多 RAII 了：

- `std::string`、`std::vector` 会自己管理内部内存
- `std::ifstream/std::ofstream` 会在对象销毁时自动关闭文件

可运行例子：文件 RAII（不用你手动 close）

```cpp
#include <fstream>
#include <iostream>
#include <string>

int main() {
    {
        std::ofstream out("demo.txt");
        out << "hello\n";
    }

    std::ifstream in("demo.txt");
    std::string line;
    std::getline(in, line);
    std::cout << "line=" << line << '\n';
    return 0;
}
```

---

## 3. 为什么 new/delete 容易出事（新手阶段尽量别用）

你当然可以这样写：

```cpp
int* p = new int(10);
delete p;
```

但新手非常容易犯错：

- 忘记 delete（泄漏）
- delete 两次（崩溃）
- delete 之后还继续用 p（悬空指针）
- 发生异常/提前 return 导致 delete 没执行（泄漏）

所以这章的原则是：

- 优先用 `std::string/std::vector` 等容器
- 需要动态对象时，优先用智能指针

---

## 4. 拷贝与移动：先理解“为什么会发生复制”

当你把一个对象赋值给另一个对象，或者把对象按值传入函数，就会发生拷贝：

- 专业名称：拷贝（copy）
- 类比：复印一份新对象，两份互不影响
- 作用：需要两份独立的数据时很有用
- 规则/坑点：如果对象内部有资源（比如指针指向的内存），拷贝就会变复杂

移动（move）解决的是“我不想复制那么贵，我只想把所有权交出去”：

- 专业名称：移动（move）
- 类比：把钥匙交给别人，你自己不再持有
- 作用：提升效率，尤其是容器、临时对象返回时
- 规则/坑点：被移动过的对象仍然要能析构，但内容可能变成“空壳”

新手阶段不要求你手写拷贝构造/移动构造，但你必须知道：有些类型拷贝便宜（int），有些类型拷贝可能很贵（大字符串、大数组、资源对象）。

---

## 5. unique_ptr：独占所有权（最常用）

- 专业名称：`std::unique_ptr`（unique ownership smart pointer）
- 类比：一把唯一的钥匙，同一时刻只能有一个人持有
- 作用：自动 delete，避免泄漏与重复释放
- 规则/坑点：不能拷贝，只能移动；一般用 `std::make_unique`

可运行例子：

```cpp
#include <iostream>
#include <memory>

int main() {
    auto p = std::make_unique<int>(42);
    std::cout << "*p=" << *p << '\n';

    auto q = std::move(p);
    if (p == nullptr) {
        std::cout << "p is null\n";
    }
    std::cout << "*q=" << *q << '\n';
    return 0;
}
```

你应该观察到：

- `std::move` 的效果是“把所有权交出去”
- 交出去以后 `p` 变成空指针，这就是“你不再持有钥匙”

---

## 6. shared_ptr：共享所有权（需要时再用）

- 专业名称：`std::shared_ptr`（shared ownership smart pointer）
- 类比：一把钥匙复制了很多份，大家都能进；最后一个人离开时才归还
- 作用：多个地方需要共同持有对象生命周期时使用
- 规则/坑点：有引用计数开销；循环引用会导致泄漏（后面会讲 weak_ptr）

可运行例子（看引用计数变化）：

```cpp
#include <iostream>
#include <memory>

int main() {
    auto p = std::make_shared<int>(7);
    std::cout << "count=" << p.use_count() << '\n';

    {
        auto q = p;
        std::cout << "count=" << p.use_count() << '\n';
        std::cout << "*q=" << *q << '\n';
    }

    std::cout << "count=" << p.use_count() << '\n';
    return 0;
}
```

---

## 7. 在类里管理资源：用“成员对象”而不是“裸指针”

你写类时，最省心的做法通常是：

- 需要一组数据：用 `std::vector`
- 需要字符串：用 `std::string`
- 需要动态对象：用 `std::unique_ptr`

可运行例子：一个拥有动态对象的类（用 unique_ptr）

```cpp
#include <iostream>
#include <memory>
#include <string>

class User {
public:
    User(std::string name) : name_(std::move(name)) {}
    const std::string& name() const { return name_; }

private:
    std::string name_;
};

class UserBox {
public:
    UserBox(std::string name) : user_(std::make_unique<User>(std::move(name))) {}

    const std::string& name() const { return user_->name(); }

private:
    std::unique_ptr<User> user_;
};

int main() {
    UserBox box("alice");
    std::cout << "name=" << box.name() << '\n';
    return 0;
}
```

这类代码的关键收益是：

- 你不用写析构函数去 delete
- 不容易出现重复释放、泄漏、悬空指针

---

## 常见坑

- 错误：new 了对象但忘记 delete  
  结果：泄漏  
  正确：优先用容器/智能指针，让释放自动发生

- 错误：delete 后继续用指针  
  结果：悬空指针，行为不稳定  
  正确：不用裸指针管理所有权；用智能指针或把指针置为 nullptr

- 错误：把 unique_ptr 当成能拷贝的类型  
  结果：编译失败  
  正确：unique_ptr 只能移动，用 `std::move`

- 错误：滥用 shared_ptr  
  结果：复杂度和开销增加，容易出现循环引用  
  正确：默认 unique_ptr；确实需要共享生命周期才用 shared_ptr

---

## 小练习

### 练习 1：把裸指针改成 unique_ptr

- 题目：写一个类 `IntBox`，内部保存一个动态 int
- 输入/输出：创建对象、设置值、输出值
- 约束：内部必须用 `std::unique_ptr<int>`
- 提示：构造里用 `std::make_unique<int>(初值)`
- 目标：练 unique_ptr 基本用法

### 练习 2：写一个小型资源守卫（RAII 思想）

- 题目：实现一个类 `ScopeLogger`，构造时输出 `enter`，析构时输出 `leave`
- 输入/输出：在一个代码块里创建对象观察输出顺序
- 约束：不需要动态内存
- 提示：用作用域 `{}` 触发析构
- 目标：体会“离开作用域自动发生”的感觉

### 练习 3：shared_ptr 引用计数

- 题目：创建一个 shared_ptr，复制两次，再依次销毁，输出 use_count
- 输入/输出：输出每一步的 count
- 约束：必须用 `use_count()`
- 提示：用代码块控制生命周期
- 目标：理解共享所有权的直觉

### 练习 4：vector 替代 new[]

- 题目：读入 n 个整数，求和与最大值
- 输入/输出：输入 n 和 n 个数，输出 sum、max
- 约束：必须用 `std::vector<int>`，不允许 `new[]`
- 提示：vector 的大小可以在运行时决定
- 目标：用 RAII 容器解决“变长数组”问题

---

## 小结

你现在应该掌握：

- 资源不止内存；忘记释放与重复释放都很危险
- RAII：资源跟着对象生命周期自动释放，是 C++ 的核心思想之一
- unique_ptr：独占所有权，默认首选
- shared_ptr：共享所有权，确实需要再用，注意循环引用风险

下一篇进入 STL 容器：你会用 `vector/map` 等标准工具写更少、更安全的代码。
