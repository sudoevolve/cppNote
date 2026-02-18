# 10-递归、DFS/BFS：树与图的基本走法

目录页：见 [C++ 小白教程目录](00-目录.md)

## 目标

- 理解递归的三要素：结束条件、递归关系、回溯
- 学会 DFS（深度优先）与 BFS（广度优先）的基本套路
- 会用栈/队列的直觉解释 DFS/BFS
- 能写一个最基础的“网格 BFS 最短路”

## 先决知识

- 会写函数与循环（见 01、02 章）
- 会用 vector（见 07 章）

---

## 1. 递归：函数调用自己

- 专业名称：递归（recursion）
- 类比：俄罗斯套娃：打开一个，里面还是同样的套娃，直到最小那个
- 作用：把“重复结构”的问题写得更自然（树、分治、回溯）
- 规则/坑点：必须有结束条件；递归太深会爆栈（stack overflow）

递归三要素（你必须能背出来，但更要能用）：

1. 结束条件（base case）：什么时候停
2. 递归关系（recursive step）：怎么把问题变小
3. 回溯（backtracking）：从更小问题返回后，怎么继续组合结果

### 1.1 阶乘（factorial）（可运行）

```cpp
#include <iostream>

long long factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return static_cast<long long>(n) * factorial(n - 1);
}

int main() {
    int n = 0;
    std::cin >> n;
    std::cout << "fact=" << factorial(n) << '\n';
    return 0;
}
```

理解：

- base case：n<=1
- 递归关系：n * fact(n-1)

---

## 2. DFS：一路走到底再回头

- 专业名称：DFS（Depth-First Search）
- 类比：走迷宫：先选一条路一直走，走不通再退回岔路口换另一条
- 作用：遍历树/图、找路径、连通块、回溯题
- 规则/坑点：图里必须记录 visited，不然会在环里无限绕

### 2.1 树的 DFS（前序遍历）示例（可运行）

先用一个最小树结构（指针会在 04 章讲过，这里只是用来建树）：

```cpp
#include <iostream>
#include <memory>

struct Node {
    int val;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

void preorder(const Node* root) {
    if (root == nullptr) {
        return;
    }
    std::cout << root->val << ' ';
    preorder(root->left.get());
    preorder(root->right.get());
}

int main() {
    auto root = std::make_unique<Node>();
    root->val = 1;
    root->left = std::make_unique<Node>();
    root->left->val = 2;
    root->right = std::make_unique<Node>();
    root->right->val = 3;

    preorder(root.get());
    std::cout << '\n';
    return 0;
}
```

你现在只需要看懂：DFS 在树上天然不会成环，但在图上会成环，所以图必须 visited。

---

## 3. BFS：一层一层扩散

- 专业名称：BFS（Breadth-First Search）
- 类比：水波纹扩散：先扩到距离 1 的点，再扩到距离 2 的点……
- 作用：无权图最短路、按层遍历、最近距离
- 规则/坑点：一般用队列（queue）；入队时就标记 visited，避免重复入队

---

## 4. 网格 BFS 最短路（最经典入门题）（可运行）

题目：给一个 0/1 网格，0 表示可走，1 表示墙。求从 (0,0) 到 (n-1,m-1) 的最少步数（只能上下左右）。

输入格式（示例）：

- 第一行 n m
- 接着 n 行，每行 m 个字符（0 或 1）

```cpp
#include <iostream>
#include <queue>
#include <string>
#include <vector>

struct Pos {
    int r;
    int c;
};

int main() {
    int n = 0;
    int m = 0;
    std::cin >> n >> m;

    std::vector<std::string> g(n);
    for (int i = 0; i < n; i++) {
        std::cin >> g[i];
    }

    std::vector<std::vector<int>> dist(n, std::vector<int>(m, -1));
    if (g[0][0] == '1') {
        std::cout << "dist=-1\n";
        return 0;
    }

    std::queue<Pos> q;
    q.push({0, 0});
    dist[0][0] = 0;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};

    while (!q.empty()) {
        Pos cur = q.front();
        q.pop();

        for (int k = 0; k < 4; k++) {
            int nr = cur.r + dr[k];
            int nc = cur.c + dc[k];
            if (nr < 0 || nr >= n || nc < 0 || nc >= m) {
                continue;
            }
            if (g[nr][nc] == '1') {
                continue;
            }
            if (dist[nr][nc] != -1) {
                continue;
            }
            dist[nr][nc] = dist[cur.r][cur.c] + 1;
            q.push({nr, nc});
        }
    }

    std::cout << "dist=" << dist[n - 1][m - 1] << '\n';
    return 0;
}
```

你要记住 BFS 的关键套路：

- 队列里永远是“当前边界”
- dist 记录距离（也相当于 visited）
- 入队时就设置 dist，避免重复入队

---

## 5. 图的表示：邻接表（入门）

- 专业名称：邻接表（adjacency list）
- 类比：每个人都有一张“好友列表”
- 作用：存图的边，适合稀疏图
- 规则/坑点：无向边要加两次；有向边只加一次

下面是一个最小可运行的“图 DFS 遍历”（从 0 出发）：

```cpp
#include <iostream>
#include <vector>

void dfs(int u, const std::vector<std::vector<int>>& g, std::vector<int>& vis) {
    vis[u] = 1;
    std::cout << u << ' ';
    for (int v : g[u]) {
        if (!vis[v]) {
            dfs(v, g, vis);
        }
    }
}

int main() {
    int n = 5;
    std::vector<std::vector<int>> g(n);

    g[0].push_back(1);
    g[1].push_back(0);
    g[1].push_back(2);
    g[2].push_back(1);
    g[0].push_back(3);
    g[3].push_back(0);

    std::vector<int> vis(n, 0);
    dfs(0, g, vis);
    std::cout << '\n';
    return 0;
}
```

---

## 常见坑

- 错误：递归没有结束条件  
  结果：无限递归，最终崩溃  
  正确：先写 base case，再写递归关系

- 错误：图 DFS/BFS 不标记 visited  
  结果：在环里无限绕  
  正确：用 visited 或 dist 记录访问状态

- 错误：BFS 出队后才标记 visited  
  结果：同一个点会被重复入队很多次  
  正确：入队时就标记

---

## 小练习

### 练习 1：斐波那契（递归 vs 循环）

- 题目：写递归版与循环版 Fibonacci，比较速度差异（只用直觉，不用精确计时）
- 输入/输出：输入 n，输出 fib(n)
- 约束：n 不要太大（递归会很慢）
- 提示：递归会重复计算
- 目标：理解“递归不等于高效”

### 练习 2：网格连通块（DFS/BFS）

- 题目：给 0/1 网格，1 表示陆地，统计岛屿数量（四方向相连算一个）
- 输入/输出：输出 count
- 约束：必须用 visited
- 提示：遇到一个 1，就 DFS/BFS 把整个岛屿标记掉
- 目标：练连通块思路

### 练习 3：无权图最短路（BFS）

- 题目：给 n 个点 m 条边的无向图，求 0 到 n-1 的最短边数
- 输入/输出：输出距离或 -1
- 约束：用邻接表 + BFS
- 提示：dist 初始为 -1
- 目标：把网格 BFS 迁移到一般图

### 练习 4：树的三种遍历

- 题目：实现前序/中序/后序遍历（递归）
- 输入/输出：输出遍历结果
- 约束：递归实现
- 提示：三种遍历只差输出位置
- 目标：练递归框架

---

## 小结

你现在应该掌握：

- 递归三要素：base case、递归关系、回溯
- DFS：一路走到底再回头；图上要 visited
- BFS：一层一层扩散；无权最短路的首选
- 网格 BFS 的模板代码与 dist/visited 的用法

下一篇进入动态规划：把“重复子问题”用表格记下来，避免重复计算。
