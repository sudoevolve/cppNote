#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>
#include <algorithm>

using namespace std;

// ============================
// 工具函数: 打印 vector
// 记忆方法: 参数后面加 &，表示直接看原对象，不拷贝
// 应用场景: 传 vector、string 这类大对象时节省开销
// ============================
void printVector(const vector<int>& nums)
{
    for (auto x : nums)
    {
        cout << x << " ";
    }

    cout << endl;
}


// ============================
// 工具函数: 打印链表
// 记忆方法: 从头结点开始，一直顺着 next 往后走
// 应用场景: 检查链表题输出是否正确
// ============================
struct ListNode
{
    int val;
    ListNode* next;

    ListNode(int x) : val(x), next(nullptr) {}
};


void printList(ListNode* head)
{
    while (head != nullptr)
    {
        cout << head->val << " ";
        head = head->next;
    }

    cout << endl;
}


// ============================
// 二叉树结点
// 记忆方法: 每个结点最多连左右两个孩子
// 应用场景: 树题模板，前中后序遍历、层序遍历、递归
// ============================
struct TreeNode
{
    int val;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};


// ============================
// 默认参数
// 记忆方法: 不传参数就用系统给的默认值
// 应用场景: 某个参数经常固定时，减少重复传参
// ============================
int add(int a, int b = 10)
{
    return a + b;
}


// ============================
// 值传递 / 引用传递 / 指针传递
// 记忆方法: 值传递传副本，引用和指针能改原值
// 应用场景: 面试高频，判断函数里改动会不会影响外部变量
// ============================
void changeByValue(int x)
{
    x = 100;
}


void changeByReference(int& x)
{
    x = 100;
}


void changeByPointer(int* x)
{
    if (x != nullptr)
    {
        *x = 100;
    }
}


// ============================
// template 模板函数
// 记忆方法: 把类型当参数传进去，代码写一份复用多种类型
// 应用场景: 通用函数、通用类、STL 容器和算法底层思想
// ============================
template <typename T>
T myMax(T a, T b)
{
    return a > b ? a : b;
}


// ============================
// struct 结构体
// 记忆方法: struct = 把几项相关数据打包在一起
// 应用场景: 学生信息、链表结点、树结点、比赛数据
// ============================
struct Student
{
    string name;
    int score;

    void introduce() const
    {
        cout << "学生: " << name << ", 分数: " << score << endl;
    }
};


// ============================
// class 类
// private / public / 构造函数
// 记忆方法: class = 数据 + 操作数据的函数 封装在一起
// 应用场景: 面向对象建模、封装属性、管理更复杂的数据行为
// ============================
class Person
{
private:
    string name;
    int age;

public:
    Person(string n, int a) : name(n), age(a) {}

    void introduce() const
    {
        cout << "姓名: " << name << ", 年龄: " << age << endl;
    }

    void setAge(int a)
    {
        age = a;
    }

    int getAge() const
    {
        return age;
    }
};


// ============================
// DFS 递归
// 记忆方法: 函数自己调用自己，但一定要有结束条件
// 应用场景: 树遍历、图遍历、深度搜索、回溯
// ============================
void dfs(int depth)
{
    if (depth == 0)
    {
        return;
    }

    cout << "当前层数: " << depth << endl;
    dfs(depth - 1);
}


// ============================
// BFS 层序遍历
// 记忆方法: 用队列一层一层弹出结点
// 应用场景: 二叉树层序遍历、最短步数、最小层数
// ============================
void bfsTree(TreeNode* root)
{
    if (root == nullptr)
    {
        return;
    }

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty())
    {
        TreeNode* node = q.front();
        q.pop();

        cout << node->val << " ";

        if (node->left != nullptr)
        {
            q.push(node->left);
        }

        if (node->right != nullptr)
        {
            q.push(node->right);
        }
    }

    cout << endl;
}


// ============================
// 二分查找
// 记忆方法: 有序数组里每次看中间，砍掉一半
// 应用场景: 查目标值、找边界、答案二分
// ============================
int binarySearchIndex(const vector<int>& nums, int target)
{
    int left = 0;
    int right = nums.size() - 1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;

        if (nums[mid] == target)
        {
            return mid;
        }
        else if (nums[mid] < target)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return -1;
}


// ============================
// 滑动窗口
// 记忆方法: 一个窗口往前滑，右边扩张，左边收缩
// 应用场景: 连续子数组、最短/最长区间、子串问题
// ============================
int minSubArrayLen(int target, const vector<int>& nums)
{
    int left = 0;
    int sum = 0;
    int ans = nums.size() + 1;

    for (int right = 0; right < nums.size(); right++)
    {
        sum += nums[right];

        while (sum >= target)
        {
            ans = min(ans, right - left + 1);
            sum -= nums[left];
            left++;
        }
    }

    return ans == nums.size() + 1 ? 0 : ans;
}


// ============================
// 前缀和
// 记忆方法: 先把前面累计起来，后面区间和一减就出来
// 应用场景: 多次区间求和、子数组和、二维前缀和
// ============================
vector<int> buildPrefixSum(const vector<int>& nums)
{
    vector<int> prefix(nums.size() + 1, 0);

    for (int i = 0; i < nums.size(); i++)
    {
        prefix[i + 1] = prefix[i] + nums[i];
    }

    return prefix;
}


int rangeSum(const vector<int>& prefix, int left, int right)
{
    return prefix[right + 1] - prefix[left];
}


// ============================
// 回溯
// 记忆方法: 做选择 -> 递归 -> 撤销选择
// 应用场景: 子集、组合、排列、棋盘搜索
// ============================
void backtrackSubsets(const vector<int>& nums,
                      int start,
                      vector<int>& path,
                      vector<vector<int>>& result)
{
    result.push_back(path);

    for (int i = start; i < nums.size(); i++)
    {
        path.push_back(nums[i]);
        backtrackSubsets(nums, i + 1, path, result);
        path.pop_back();
    }
}


// ============================
// 贪心
// 记忆方法: 每一步都先做当前最划算的选择
// 应用场景: 区间问题、跳跃游戏、分发类问题
// ============================
bool canJump(const vector<int>& nums)
{
    int farthest = 0;

    for (int i = 0; i < nums.size(); i++)
    {
        if (i > farthest)
        {
            return false;
        }

        farthest = max(farthest, i + nums[i]);
    }

    return true;
}


// ============================
// 动态规划
// 记忆方法: 当前答案由前面的答案推出来
// 应用场景: 计数、最值、背包、子序列问题
// ============================
int climbStairs(int n)
{
    if (n <= 2)
    {
        return n;
    }

    int a = 1;
    int b = 2;

    for (int i = 3; i <= n; i++)
    {
        int c = a + b;
        a = b;
        b = c;
    }

    return b;
}


int main()
{
    cout << "============================" << endl;
    cout << "1. 语言基础" << endl;
    cout << "============================" << endl;

    // ============================
    // 基本类型
    // int / double / char / bool
    // 记忆方法: int 整数，double 小数，char 单个字符，bool 真或假
    // 应用场景: 所有 C++ 变量的基础，后面容器和算法都建立在它们上面
    // ============================
    int num = 10;
    double pi = 3.14;
    char ch = 'A';
    bool isCppFun = true;

    cout << "int: " << num << endl;
    cout << "double: " << pi << endl;
    cout << "char: " << ch << endl;
    cout << "bool: " << isCppFun << endl;

    // ============================
    // const 常量
    // 记忆方法: const = 只读，定义后不让改
    // 应用场景: 固定配置、函数参数保护、提高代码安全性
    // ============================
    const int days = 7;

    cout << endl;
    cout << "一周天数: " << days << endl;

    // ============================
    // 引用 &
    // 记忆方法: 引用就是变量的别名，改引用等于改原变量
    // 应用场景: 传参省拷贝、给变量起别名、范围 for 常用
    // ============================
    int a = 5;
    int& ref = a;
    ref = 20;

    cout << endl;
    cout << "a: " << a << endl;
    cout << "ref: " << ref << endl;

    // ============================
    // 指针 *
    // 保存变量地址
    // 记忆方法: 指针里装地址，*ptr 才是地址指向的值
    // 应用场景: 链表、树、动态内存、函数改外部变量
    // ============================
    int score = 95;
    int* ptr = &score;

    cout << endl;
    cout << "score 的值: " << score << endl;
    cout << "score 的地址: " << ptr << endl;
    cout << "指针解引用后的值: " << *ptr << endl;

    // ============================
    // 函数默认参数
    // 记忆方法: 少传的参数自动补默认值
    // 应用场景: 常用参数有默认选项时，让调用更简洁
    // ============================
    cout << endl;
    cout << "add(5): " << add(5) << endl;
    cout << "add(5, 3): " << add(5, 3) << endl;

    // ============================
    // 值传递 / 引用传递 / 指针传递
    // 记忆方法: 值传递不影响外面，引用和指针可能影响外面
    // 应用场景: 判断函数修改范围、写高效参数传递
    // ============================
    int valueTest = 1;

    changeByValue(valueTest);
    cout << endl;
    cout << "值传递后: " << valueTest << endl;

    changeByReference(valueTest);
    cout << "引用传递后: " << valueTest << endl;

    valueTest = 1;
    changeByPointer(&valueTest);
    cout << "指针传递后: " << valueTest << endl;

    // ============================
    // template 模板函数
    // 记忆方法: 一个函数模板，适配多种类型
    // 应用场景: 写通用工具函数，减少重复代码
    // ============================
    cout << endl;
    cout << "myMax(3, 7): " << myMax(3, 7) << endl;
    cout << "myMax(2.5, 1.2): " << myMax(2.5, 1.2) << endl;

    // ============================
    // struct 结构体
    // 记忆方法: 多个相关变量打包成一个整体
    // 应用场景: 自定义数据类型，刷题里经常拿来定义结点或对象
    // ============================
    Student cppStudent = {"Bob", 95};

    cout << endl;
    cppStudent.introduce();

    // ============================
    // class 类
    // 记忆方法: class 比 struct 更强调封装和权限控制
    // 应用场景: 系统学习 C++ 面向对象时必备
    // ============================
    Person person("Alice", 20);

    cout << endl;
    person.introduce();
    person.setAge(21);
    cout << "修改后的年龄: " << person.getAge() << endl;

    cout << endl;
    cout << "============================" << endl;
    cout << "2. STL 容器与常用工具" << endl;
    cout << "============================" << endl;

    // ============================
    // vector 动态数组
    // 记忆方法: vector 就像会自动扩容的数组
    // 应用场景: 题目里最常用的顺序容器，存一组数据
    // ============================
    vector<int> nums;
    nums.push_back(5);
    nums.push_back(1);
    nums.push_back(8);
    nums.push_back(3);

    cout << "原数组: ";
    printVector(nums);

    // ============================
    // sort 排序
    // 记忆方法: sort = 先排好，再更容易查找或贪心
    // 应用场景: 排名、去重前预处理、二分前预处理、区间题
    // ============================
    sort(nums.begin(), nums.end());

    cout << "升序排序后: ";
    printVector(nums);

    // ============================
    // lambda 匿名函数
    // 降序排序
    // 记忆方法: lambda = 当场写一个小函数
    // 应用场景: 自定义排序规则、局部比较逻辑
    // ============================
    sort(nums.begin(), nums.end(),
    [](int x, int y)
    {
        return x > y;
    });

    cout << "降序排序后: ";
    printVector(nums);

    // ============================
    // string 字符串
    // 记忆方法: string 就是字符组成的数组
    // 应用场景: 子串、回文、模拟、双指针字符串题
    // ============================
    string s = "hello";

    cout << endl;
    cout << "字符串长度: " << s.size() << endl;
    cout << "第一个字符: " << s[0] << endl;

    // ============================
    // pair
    // 存两个值
    // 记忆方法: pair = 一对数据绑在一起
    // 应用场景: 坐标、键值对、区间左右端点
    // ============================
    pair<string, int> studentPair;
    studentPair.first = "Bob";
    studentPair.second = 95;

    cout << endl;
    cout << "姓名: " << studentPair.first << endl;
    cout << "分数: " << studentPair.second << endl;

    // ============================
    // unordered_map 哈希表
    // key -> value
    // 记忆方法: 哈希表 = 用 key 快速找 value
    // 应用场景: 计数、映射、查重、两数之和
    // ============================
    unordered_map<string, int> age;
    age["Tom"] = 18;
    age["Alice"] = 20;

    cout << endl;
    cout << "Tom 的年龄: " << age["Tom"] << endl;
    cout << "Alice 是否存在: " << age.count("Alice") << endl;

    // ============================
    // set / unordered_set
    // 记忆方法: set 负责去重，unordered_set 查找更快
    // 应用场景: 判重、去重、判断元素是否出现过
    // ============================
    set<int> orderedSet = {3, 1, 2, 2};
    unordered_set<int> hashSet = {10, 20, 10};

    cout << endl;
    cout << "set 去重后: ";
    for (int x : orderedSet)
    {
        cout << x << " ";
    }
    cout << endl;
    cout << "20 是否存在: " << hashSet.count(20) << endl;

    // ============================
    // map
    // 记忆方法: map = 有序的 key-value
    // 应用场景: 需要按 key 从小到大遍历、统计频次
    // ============================
    map<string, int> scoreMap;
    scoreMap["math"] = 90;
    scoreMap["english"] = 85;

    cout << endl;
    cout << "map 中按 key 排序后的遍历: ";
    for (auto item : scoreMap)
    {
        cout << "(" << item.first << ", " << item.second << ") ";
    }
    cout << endl;

    // ============================
    // stack 栈
    // 后进先出
    // 记忆方法: 像一摞盘子，最后放的先拿
    // 应用场景: 括号匹配、单调栈、函数调用过程
    // ============================
    stack<int> st;
    st.push(10);
    st.push(20);
    st.push(30);

    cout << endl;
    cout << "栈顶元素: " << st.top() << endl;
    st.pop();
    cout << "弹出后栈顶: " << st.top() << endl;

    // ============================
    // queue 队列
    // 先进先出
    // 记忆方法: 像排队，先来的先处理
    // 应用场景: BFS 层序遍历、任务调度
    // ============================
    queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);

    cout << endl;
    cout << "队头元素: " << q.front() << endl;
    q.pop();
    cout << "弹出后队头: " << q.front() << endl;

    // ============================
    // priority_queue 优先队列
    // 记忆方法: 每次先拿最大值
    // 应用场景: Top K、堆、每次取当前最优值
    // ============================
    priority_queue<int> maxHeap;
    maxHeap.push(5);
    maxHeap.push(1);
    maxHeap.push(9);

    cout << endl;
    cout << "堆顶最大值: " << maxHeap.top() << endl;

    cout << endl;
    cout << "============================" << endl;
    cout << "3. 常见数据结构" << endl;
    cout << "============================" << endl;

    // ============================
    // 链表
    // 记忆方法: 每个结点只知道下一个结点
    // 应用场景: 反转链表、快慢指针、删除结点
    // ============================
    ListNode node1(1);
    ListNode node2(2);
    ListNode node3(3);
    node1.next = &node2;
    node2.next = &node3;

    cout << "链表: ";
    printList(&node1);

    // ============================
    // 二叉树 + BFS
    // 记忆方法: 树是分叉结构，BFS 是一层一层看
    // 应用场景: 层序遍历、最短步数、最小层数
    // ============================
    TreeNode root(1);
    TreeNode leftNode(2);
    TreeNode rightNode(3);
    TreeNode leftLeftNode(4);

    root.left = &leftNode;
    root.right = &rightNode;
    leftNode.left = &leftLeftNode;

    cout << endl;
    cout << "二叉树 BFS: ";
    bfsTree(&root);

    cout << endl;
    cout << "============================" << endl;
    cout << "4. 算法思想" << endl;
    cout << "============================" << endl;

    // ============================
    // 双指针
    // 记忆方法: 两个指针一起移动，减少重复遍历
    // 应用场景: 有序数组、回文、区间压缩、快慢指针
    // ============================
    vector<int> arr = {1, 2, 3, 4, 5};
    int left = 0;
    int right = arr.size() - 1;

    cout << "双指针:" << endl;
    while (left < right)
    {
        cout << arr[left] << " " << arr[right] << endl;
        left++;
        right--;
    }

    // ============================
    // 二分查找
    // 记忆方法: 每次砍掉一半区间
    // 应用场景: 有序数组查找、答案范围查找
    // ============================
    vector<int> binaryNums = {1, 3, 5, 7, 9};

    cout << endl;
    cout << "数字 7 的下标: " << binarySearchIndex(binaryNums, 7) << endl;
    cout << "lower_bound(6) 的位置: "
         << lower_bound(binaryNums.begin(), binaryNums.end(), 6) - binaryNums.begin()
         << endl;

    // ============================
    // 滑动窗口
    // 记忆方法: 左右边界像一扇会滑动的窗户
    // 应用场景: 最短/最长子数组、子串问题、连续区间
    // ============================
    vector<int> windowNums = {2, 3, 1, 2, 4, 3};

    cout << endl;
    cout << "和至少为 7 的最短子数组长度: "
         << minSubArrayLen(7, windowNums)
         << endl;

    // ============================
    // 前缀和
    // 记忆方法: prefix[i] 表示前 i 个数的总和
    // 应用场景: 多次区间求和、子数组和问题
    // ============================
    vector<int> prefixNums = {1, 2, 3, 4};
    vector<int> prefix = buildPrefixSum(prefixNums);

    cout << endl;
    cout << "区间 [1, 3] 的和: " << rangeSum(prefix, 1, 3) << endl;

    // ============================
    // DFS 递归
    // 记忆方法: 一条路先走到底，再回来换路
    // 应用场景: 树遍历、图遍历、回溯搜索
    // ============================
    cout << endl;
    cout << "DFS递归:" << endl;
    dfs(3);

    // ============================
    // 回溯
    // 记忆方法: 先选，再递归，不合适就撤回
    // 应用场景: 子集、排列、组合、N 皇后
    // ============================
    vector<int> backtrackNums = {1, 2};
    vector<int> path;
    vector<vector<int>> subsets;
    backtrackSubsets(backtrackNums, 0, path, subsets);

    cout << endl;
    cout << "子集个数: " << subsets.size() << endl;

    // ============================
    // 贪心
    // 记忆方法: 每一步都先拿当前看起来最优的选择
    // 应用场景: 跳跃游戏、区间选择、分发问题
    // ============================
    vector<int> jumpNums = {2, 3, 1, 1, 4};

    cout << endl;
    cout << "是否能跳到终点: " << canJump(jumpNums) << endl;

    // ============================
    // 动态规划
    // 记忆方法: 大问题拆小问题，答案从前面推出来
    // 应用场景: 爬楼梯、背包、最长子序列
    // ============================
    cout << endl;
    cout << "爬 5 阶楼梯的方法数: " << climbStairs(5) << endl;

    return 0;
}
