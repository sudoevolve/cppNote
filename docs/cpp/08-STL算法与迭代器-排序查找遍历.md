# 08-STL 算法与迭代器：排序、查找、遍历

目录页：见 [C++ 小白教程目录](00-目录.md)

## 目标

- 理解“迭代器”是什么：容器的通用指针/游标
- 会用常见算法：`sort`、`find`、`count`、`lower_bound`
- 理解“算法 + 容器”的配合方式：少写循环、少写 bug
- 会写简单 lambda 用作自定义排序规则

## 先决知识

- 会用 vector/map 等容器（见 07 章）

---

## 1. 迭代器（iterator）：容器的“通用指针”

- 专业名称：迭代器（iterator）
- 类比：书签/游标，指向容器里的某个位置
- 作用：把“怎么遍历”抽象出来，让算法能对不同容器通用
- 规则/坑点：`begin()` 指向第一个元素，`end()` 指向“最后一个元素后面的位置”（不是最后一个）

先看一个可运行例子：用迭代器遍历 vector。

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {3, 1, 4};

    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << '\n';
    return 0;
}
```

你可以把这段理解成：

- `it` 像指针
- `*it` 像解引用（取出当前位置的值）（见 04 章对 `*` 的直觉）

---

## 2. sort：排序（最常用算法之一）

- 专业名称：`std::sort`（introsort）
- 类比：把一堆牌按规则排好
- 作用：对随机访问范围快速排序（vector/string 很常用）
- 规则/坑点：默认从小到大；只对“随机访问迭代器”工作（vector/array/string OK）

### 2.1 默认升序排序（可运行）

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {5, 1, 4, 2, 3};
    std::sort(v.begin(), v.end());

    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    return 0;
}
```

### 2.2 自定义排序：用 lambda 指定规则

- 专业名称：lambda 表达式（lambda expression）
- 类比：临时写的一条“比较规则小纸条”
- 作用：把排序规则当作参数传给算法
- 规则/坑点：比较函数必须满足“严格弱序”（新手阶段你理解成：要稳定且自洽，不要乱写）

可运行例子：按绝对值从小到大排，如果绝对值相同，按原值从小到大。

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int absInt(int x) {
    return x < 0 ? -x : x;
}

int main() {
    std::vector<int> v = {-3, 2, -2, 1, 0};
    std::sort(v.begin(), v.end(), [](int a, int b) {
        int aa = absInt(a);
        int bb = absInt(b);
        if (aa != bb) {
            return aa < bb;
        }
        return a < b;
    });

    for (auto x : v) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
    return 0;
}
```

---

## 3. find / count：查找与计数

### 3.1 find：找到某个值的位置

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {3, 1, 4, 1, 5};
    int target = 4;

    auto it = std::find(v.begin(), v.end(), target);
    if (it == v.end()) {
        std::cout << "not found\n";
    } else {
        std::cout << "found=" << *it << '\n';
    }
    return 0;
}
```

你要记住这个套路：

- 找不到就返回 `end()`

### 3.2 count：统计等于某个值的数量

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 1, 3, 1};
    std::cout << "count=" << std::count(v.begin(), v.end(), 1) << '\n';
    return 0;
}
```

---

## 4. lower_bound：二分查找的标准写法

- 专业名称：`std::lower_bound`（binary search）
- 类比：在有序书架上找“第一个不小于目标的位置”
- 作用：在已排序的范围内 O(log n) 找位置
- 规则/坑点：必须先排序；它返回的是迭代器位置，不是下标

可运行例子：插入位置与是否存在

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 4, 4, 7};
    int x = 4;

    auto it = std::lower_bound(v.begin(), v.end(), x);
    std::size_t pos = static_cast<std::size_t>(it - v.begin());

    std::cout << "pos=" << pos << '\n';
    if (it != v.end() && *it == x) {
        std::cout << "exists=1\n";
    } else {
        std::cout << "exists=0\n";
    }
    return 0;
}
```

---

## 5. 算法思维：让循环变成“表达意图”

对新手来说，STL 算法最大的价值不是“炫技”，而是更少出错：

- 你写循环容易写错边界
- 你写 sort/find 的模板代码容易重复且不统一

把“意图”交给标准算法，代码更短、更稳定，也更容易被别人看懂。

---

## 常见坑

- 错误：把 `end()` 当成最后一个元素  
  结果：解引用 `*end()` 崩溃  
  正确：`end()` 是“末尾后面的位置”，不能解引用

- 错误：lower_bound 用在没排序的数组上  
  结果：结果完全不可信  
  正确：先 sort，再 lower_bound

- 错误：sort 用在不支持随机访问的容器  
  结果：编译失败  
  正确：vector/string 用 sort；其他容器用各自的方法（后面再扩展）

---

## 小练习

### 练习 1：排序并去重（vector + sort）

- 题目：输入 n 个整数，输出排序后去重的序列
- 输入/输出：输出去重后的有序序列
- 约束：必须使用 `std::sort`
- 提示：先 sort，再线性去重（新手先用一个新 vector 存结果）
- 目标：练 sort 与遍历

### 练习 2：二分查找位置（lower_bound）

- 题目：输入一个已排序数组和查询 x，输出 x 的第一次出现位置（下标），不存在输出 -1
- 输入/输出：输出下标或 -1
- 约束：必须用 `std::lower_bound`
- 提示：lower_bound 找到位置后再判断 `*it == x`
- 目标：练“迭代器转下标”的写法

### 练习 3：自定义排序规则

- 题目：输入 n 个整数，按“绝对值升序、绝对值相同按原值升序”输出
- 输入/输出：输出排序后的序列
- 约束：必须用 lambda 当比较函数
- 提示：比较函数返回 `a` 是否应排在 `b` 前
- 目标：练 sort 的自定义比较

### 练习 4：统计某个值出现次数（count）

- 题目：输入 n 个整数和 x，输出 x 出现次数
- 输入/输出：输出 count
- 约束：必须用 `std::count`
- 提示：别自己写循环
- 目标：建立“用算法表达意图”的习惯

---

## 小结

你现在应该掌握：

- 迭代器是容器的通用游标，`begin/end` 表示范围
- `sort/find/count/lower_bound` 是最常用的一批标准算法
- 二分查找必须在有序范围内使用

下一篇开始进入算法与数据结构：我们会系统讲复杂度、二分、排序、双指针等基础套路。
