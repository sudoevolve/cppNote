---

📒 C++ 面向对象基础笔记


---

1. 构造函数（Constructor）

🌱 定义

类里的特殊函数，对象创建时自动调用，用来 初始化对象。

特点：

名字与类名相同

无返回值（连 void 都不能写）

可以有参数，也可以没有参数

可以重载（同名不同参数）



🌱 类型

1. 默认构造函数（无参数）


2. 有参构造函数（带参数）


3. 拷贝构造函数（用已有对象初始化新对象）


4. 委托构造函数（C++11）（一个构造函数调用另一个）


5. explicit 构造函数（防止隐式类型转换）



🌱 示例

class Person {
public:
    int age;
    std::string name;

    Person() : age(0), name("Unknown") {}                 // 默认构造
    Person(int a, std::string n) : age(a), name(n) {}    // 有参构造
    Person(const Person& other) : age(other.age), name(other.name) {} // 拷贝构造
};

🌱 注意事项

定义了有参构造函数，如果要 Person p; 必须手动写默认构造函数

构造函数不能有返回值

初始化列表比在函数体内赋值更高效



---

2. 析构函数（Destructor）

🌱 定义

类里的特殊函数，对象销毁时自动调用，用来 清理资源（内存、文件、网络等）

特点：

名字是 ~类名

无参数、无返回值

一个类只能有一个析构函数



🌱 示例

class Person {
public:
    Person() { std::cout << "对象创建\n"; }
    ~Person() { std::cout << "对象销毁\n"; }
};

🌱 注意事项

析构函数不能重载

如果类有指针或资源，析构函数必须释放，否则会内存泄漏

有虚函数的类，析构函数建议加 virtual



---

3. 基类（Base Class）与派生类（Derived Class）

🌱 定义

基类：父类，提供通用属性和方法

派生类：子类，继承基类的属性和方法，可以扩展


🌱 示例

class Person {
public:
    std::string name;
    int age;
    void sayHello() { std::cout << "Hello, I am " << name << "\n"; }
};

class Student : public Person {
public:
    int grade;
    void study() { std::cout << name << " is studying.\n"; }
};

🌱 注意事项

继承类型影响访问权限：public/protected/private

支持单继承、多继承，但多继承有菱形问题需注意

子类可以重写基类方法



---

4. 虚函数（Virtual Function）

🌱 定义

在父类函数前加 virtual，派生类重写后可实现 运行时多态

普通函数：编译时决定调用

虚函数：运行时决定调用


🌱 示例

class Animal {
public:
    virtual void speak() { std::cout << "Animal sound\n"; }
};

class Dog : public Animal { public: void speak() override { std::cout << "Woof!\n"; } };
class Cat : public Animal { public: void speak() override { std::cout << "Meow!\n"; } };

int main() {
    Animal* a;
    Dog d; Cat c;

    a = &d; a->speak();  // 输出 Woof!
    a = &c; a->speak();  // 输出 Meow!
}

🌱 注意事项

构造函数不能 virtual

析构函数建议 virtual

虚函数多用于基类指针或引用操作子类对象，实现多态



---

5. 多态（Polymorphism）

🌱 定义

同一个操作作用于不同类型对象时，表现出不同的行为

静态多态（编译时）：函数重载、运算符重载

动态多态（运行时）：虚函数 + 基类指针/引用


🌱 注意事项

动态多态必须通过 基类指针或引用

多态可让接口统一，增加代码扩展性



---

6. 友元函数（Friend Function）

🌱 定义

友元函数可以访问类的私有成员，即使它不是类成员


🌱 示例

class Box {
private:
    int length;
public:
    Box(int l) : length(l) {}
    friend void printLength(Box b); // 声明友元
};

void printLength(Box b) {
    std::cout << "Length = " << b.length << "\n"; // 可以访问 private
}

🌱 注意事项

友元函数增加耦合度，不要滥用

友元不是类成员，只是允许访问私有数据



---

7. 虚基类（Virtual Base Class）

🌱 问题背景：菱形继承

A
    / \
   B   C
    \ /
     D

B 和 C 都继承 A

D 继承 B 和 C → 会有 两份 A → 二义性


🌱 解决方案：虚继承

class B : virtual public A {};
class C : virtual public A {};
class D : public B, public C {};

D 只继承 一份 A

消除二义性


🌱 注意事项

虚继承会增加指针表开销

构造顺序：最底层派生类构造虚基类

只在多继承有共同基类时使用



---

8. 自带类内部初始化

标准库类（string, vector, fstream 等）已写好构造/析构函数

用起来：

构造函数自动初始化

析构函数自动释放资源



🌱 示例

#include <string>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    string s("Hello");
    vector<int> v{1,2,3};
    for (int x : v) cout << x << " ";
}


---

9. 综合记忆表

概念	作用	何时调用	注意事项

构造函数	初始化对象	对象创建	无返回值，可重载，初始化列表高效
析构函数	清理资源	对象销毁	无参数，无重载，指针需释放，建议 virtual
基类/派生类	父子继承	—	继承类型影响访问权限，多继承注意菱形问题
虚函数	动态多态	运行时	构造函数不能 virtual，析构函数建议 virtual
多态	同一接口不同表现	运行时	基类指针/引用操作子类对象
友元函数	访问类私有成员	被调用	增加耦合度，非成员函数
虚基类	解决菱形继承二义性	对象创建	构造顺序特殊，增加指针表开销
自带类	内部构造/析构	自动	string/vector/fstream 自动管理资源



---

