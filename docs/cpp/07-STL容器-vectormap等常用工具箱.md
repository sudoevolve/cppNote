# 07-STL 容器：vector/map 等常用工具箱

目录页：见 [C++ 小白教程目录](00-目录.md)

## 目标

- 用 `std::vector` 解决“运行时才知道长度”的数组需求
- 学会遍历容器：for 循环、范围 for（range-based for）
- 学会用 `std::map/unordered_map` 做键值映射（词频统计等）
- 知道 `std::set/unordered_set` 的用途（去重）

## 先决知识

- 会写循环与函数（见 01、02 章）
- 理解 RAII 与“尽量不用 new[]”（见 06 章 2、7）

---

## 1. STL 与容器：标准工具箱

- 专业名称：STL（Standard Template Library）
- 类比：C++ 官方配的“工具箱”，很多常用结构都给你做好了
- 作用：少造轮子，少写 bug，性能也通常更好
- 规则/坑点：先把最常用的学会：`vector`、`string`、`map/unordered_map`、`set/unordered_set`

---

## 2. vector：可变长数组（最常用容器）

- 专业名称：`std::vector<T>`（dynamic array）
- 类比：一排可以自动扩建的储物格，格子永远是连续的
- 作用：运行时决定长度、自动管理内存、支持下标访问与快速尾插
- 规则/坑点：下标仍然从 0 开始；`v[i]` 不做越界检查；扩容会让某些指针/引用失效（先记住结论）

### 2.1 创建、push_back、遍历（可运行）

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    std::cout << "size=" << v.size() << '\n';
    for (std::size_t i = 0; i < v.size(); i++) {
        std::cout << "v[" << i << "]=" << v[i] << '\n';
    }
    return 0;
}
```

你会看到 `v.size()` 返回的是无符号类型，常用的是 `std::size_t`。

### 2.2 范围 for：更像“把每个元素都过一遍”

- 专业名称：范围 for（range-based for）
- 类比：不关心编号，只关心“依次拿到每个元素”
- 作用：遍历更简洁，不容易写错边界
- 规则/坑点：`for (auto x : v)` 是复制；想修改元素要用引用 `auto&`

可运行例子（先翻倍再输出）：

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 3, 4};

    for (auto& x : v) {
        x *= 2;
    }

    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    return 0;
}
```

---

## 3. 常用操作：查找、删除、插入（先学最通用的）

### 3.1 在 vector 里找一个值（朴素写法）

```cpp
#include <iostream>
#include <vector>

int findFirst(const std::vector<int>& v, int target) {
    for (std::size_t i = 0; i < v.size(); i++) {
        if (v[i] == target) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int main() {
    std::vector<int> v = {3, 1, 4, 1, 5};
    std::cout << "idx=" << findFirst(v, 4) << '\n';
    return 0;
}
```

这里 `const std::vector<int>&` 的意思是：不复制整个 vector（避免开销），并且不允许在函数里改它（见 04 章对引用与 const 的思路）。

### 3.2 删除某个下标的元素（会移动后面的元素）

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {10, 20, 30, 40};
    std::size_t idx = 1;
    v.erase(v.begin() + static_cast<std::ptrdiff_t>(idx));

    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    return 0;
}
```

这会把 20 删除，30/40 往前挪。新手阶段你先记住：vector 删除中间元素是 O(n) 的。

---

## 4. map 与 unordered_map：键值映射（字典）

- 专业名称：映射（map / associative container）
- 类比：通讯录：用“名字（键）”找到“电话（值）”
- 作用：按键快速查值、统计次数、建立对应关系
- 规则/坑点：
  - `std::map`：键有序（通常红黑树），操作 O(log n)
  - `std::unordered_map`：键无序（哈希表），平均 O(1)，但更依赖哈希质量

### 4.1 词频统计（unordered_map）（可运行）

```cpp
#include <iostream>
#include <string>
#include <unordered_map>

int main() {
    std::unordered_map<std::string, int> freq;

    int n = 0;
    std::cin >> n;
    for (int i = 0; i < n; i++) {
        std::string word;
        std::cin >> word;
        freq[word] += 1;
    }

    std::string query;
    std::cin >> query;
    std::cout << "count=" << freq[query] << '\n';
    return 0;
}
```

这里 `freq[word]` 的规则要记住：

- 如果键不存在，`operator[]` 会自动创建一个默认值（int 默认是 0），然后再加 1

如果你不想自动创建键，用 `find`（第 08 篇讲算法与迭代器时会更系统）。

---

## 5. set 与 unordered_set：去重集合

- 专业名称：集合（set）
- 类比：一个“只收不重复的会员名单”
- 作用：自动去重、判断某个元素是否存在
- 规则/坑点：set 不存重复元素；unordered_set 平均更快但无序

可运行例子：读入 n 个整数，输出去重后的个数

```cpp
#include <iostream>
#include <unordered_set>

int main() {
    std::unordered_set<int> s;
    int n = 0;
    std::cin >> n;
    for (int i = 0; i < n; i++) {
        int x = 0;
        std::cin >> x;
        s.insert(x);
    }
    std::cout << "unique=" << s.size() << '\n';
    return 0;
}
```

---

## 常见坑

- 错误：把 vector 当成“随便删插都很快”的结构  
  结果：中间删插 O(n)，数据一多就慢  
  正确：vector 擅长尾插与遍历；频繁中间操作考虑其他结构（后面再扩展）

- 错误：用 `v[i]` 访问但 i 越界  
  结果：行为不稳定  
  正确：确保 `i < v.size()`；需要检查可用 `v.at(i)`

- 错误：用 `unordered_map[key]` 查询却不想创建键  
  结果：容器里多出你没想要的键  
  正确：用 `find` 判断存在再取值

---

## 小练习

### 练习 1：变长输入统计（vector）

- 题目：输入 n，再输入 n 个整数，输出 sum、max、min
- 输入/输出：输出三项统计值
- 约束：必须用 `std::vector<int>` 保存数据
- 提示：先读入到 vector，再遍历统计
- 目标：练 vector 的创建、读入、遍历

### 练习 2：去重并保持输入顺序（unordered_set + vector）

- 题目：输入 n 个整数，按第一次出现顺序输出去重后的序列
- 输入/输出：输出去重后序列
- 约束：不能用排序
- 提示：用 set 判断是否出现过，用 vector 保存输出
- 目标：练“两个容器配合”

### 练习 3：词频 Top1（unordered_map）

- 题目：输入 n 个单词，输出出现次数最多的单词与次数
- 输入/输出：输出 `word=... count=...`
- 约束：如果并列，输出字典序最小的单词（需要处理并列规则）
- 提示：先统计，再遍历 map 找最大
- 目标：练 map 统计与遍历

### 练习 4：成绩表（map）

- 题目：支持两类操作：`set name score` 和 `get name`
- 输入/输出：get 输出 score，不存在输出 -1
- 约束：name 不含空格
- 提示：get 时用 `find` 判断是否存在
- 目标：练 map 的查找与更新

---

## 小结

你现在应该掌握：

- vector：可变长数组，最常用，适合遍历与尾插
- map/unordered_map：键值映射，用于统计与查询
- set/unordered_set：去重与存在性判断

下一篇会讲迭代器与算法：你会用 `sort/find/lower_bound` 等标准算法写更简洁的代码。
