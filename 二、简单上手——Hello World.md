写C++程序其实很简单，直接用记事本写好代码，然后用一个编译器做编译运行就可以了；不过这意味这我们得自己保证语法正确，严重影响开发效率。所以实际应用中我们一般都会使用功能更强大的工具，除了提供编译器外，还可以给我们做语法检查和提醒，方便我们调试程序——这就是所谓的“集成开发环境”（IDE）。

这里我们使用ubuntu系统来演示，首先我们需要一个ide来编辑代码，这里我们选择 vscode,安装好软件后就可以写代码了
下面就是一段最简单的代码，我们在屏幕上输出Hello World。 

```cpp
#include<iostream>

int main()

{

std::cout << "Hello World!" << std::endl;

}
```
在Linux上运行C++程序的过程通常包括以下步骤：

 - 安装编译器： 确保安装了C++编译器。常见的C++编译器是`g++`。可以通过以下命令安装：
```bash
sudo apt update
sudo apt install g++
```

- **编写C++源代码**： 使用任何文本编辑器（如`vim`、`nano`、`gedit`等）编写C++源代码。例如，将代码保存为`example.cpp`。
- ![[Pasted image 20250117134817.png]]
    
- **编译链接代码**： 使用`g++`命令编译C++源代码为可执行文件。假设源代码文件名为`example.cpp`，你可以运行以下命令：
```bash
 g++ example.cpp -o example
```

    - 这里，`-o example`表示生成名为`example`的可执行文件。
    - example文件是一个经过编译和链接的ELF格式可执行文件，它包含了运行程序所需的机器码、数据以及元信息。在Linux中，不需要文件扩展名来标识其为可执行文件，系统通过文件权限和格式识别它。
    
- **运行程序**： 编译完成后，使用以下命令运行生成的可执行文件：
```bash
 ./example
```

![[Pasted image 20250117134942.png]]
终端就会打印出Hello World

当然我们还没有完全发挥IDE的优势（众多的实用插件）
写c++程序我们可以安装以下插件实现代码自动补全和一键编译运行
![[Pasted image 20250117135309.png]]
![[Pasted image 20250117135412.png]]

**代码解读** 

### 1. `#include "iostream"`

```cpp
#include "iostream"
```

- `#include`是预处理指令，用于引入头文件。这里引入的是`iostream`头文件，它包含了C++中与输入输出相关的功能。
- `iostream`提供了`cin`、`cout`、`cerr`等用于输入和输出的对象。`cout`用于输出，`cin`用于输入。

### 2. `using namespace std;`

```cpp
using namespace std;
```

- 这行代码告诉编译器，当前程序使用`std`（标准）命名空间中的所有对象和函数。
- `std`命名空间包含了标准库中的类和函数，如`cout`、`cin`、`string`等。
- 如果不使用这行代码，你需要在每次使用标准库的功能时都加上`std::`前缀。例如：`std::cout`。

### 3. `int main()`

```cpp
int main()
```

- `main`是程序的入口点，程序的执行从这里开始。
- `int`表示`main`函数返回一个整数类型的值。返回值通常用来表示程序的运行状态，`0`表示程序正常结束。

### 4. `cout << "Hello World!" << endl;`

```cpp
cout << "Hello World!" << endl;
```

- `cout`是标准输出流对象，用于将数据输出到屏幕上。
- `<<`是流插入运算符，用于将数据发送到输出流中。在这里，`"Hello World!"`字符串被发送到`cout`，也就是显示在屏幕上。
- `endl`是输出流中的换行符，它不仅会换行，还会刷新输出流，确保输出的内容立即显示。

### 5. `return 0;`

```cpp
return 0;
```

- `return`语句用于结束`main`函数，并将一个整数值返回给操作系统。返回`0`表示程序成功执行并正常退出。
- `main`函数返回的值通常用于表示程序的退出状态，`0`表示程序没有错误地运行。

### 程序执行流程：

1. 编译器将代码中的`#include "iostream"`替换为`iostream`头文件的内容。
2. `main`函数开始执行，`cout << "Hello World!" << endl;`输出`Hello World!`并换行。
3. 程序通过`return 0;`返回`0`，表示程序正常结束。

### 输出结果：

```
Hello World!
```

**注释** 

可以看到，纯粹的代码还是比较抽象的；特别是当代码越来越多、越来越复杂之后，就会变得越来越难理解。所以我们一般会插入一些解释说明的文字，这叫做“注释”。注释不会被执行，对代码的功能没有任何影响。 

在C++中，有两种注释的表示。一种是单行注释，用双斜线“//”，表示以它开始的当前行是注释内容；另一种是多行注释，使用一对“界定符”（/* 和 */）,在它们之间的所有内容都是注释。

**初步认识函数** 

通过一个最简单的Hello World程序，我们已经了解了C++基本的代码风格、简单的输入输出操作，以及程序编译运行的完整过程。利用这些知识我们可以为这个程序增加更多的功能，比如提示用户输入自己的名字XXX，然后显示“Hello， XXX”。 

代码如下：
```cpp
#include<iostream>

using namespace std;

int main()

{

// 输出一行信息 

cout << "Hello World!" << endl;

// 提示输入姓名 

cout << "请输入您的大名：" << endl;

// 用一个变量接收键盘输入 

string name;

cin >> name;

// 输出欢迎信息 

cout << "Hello, " << name << endl;

return 0;

}
```
但是这样代码就比较多了，可读性会变差。解决办法是，我们可以把中间一部分代码“包装”成函数，就像主函数一样。只不过这种函数不是启动直接调用的，而是需要在程序中明确地写出来什么时候调用。 

代码如下：
```cpp
#include<iostream>

using namespace std;

// 定义一个函数

void welcome()

{

cout << "Hello World!" << endl;

cout << "请输入您的大名：" << endl;

string name;

cin >> name;

cout << "Hello, " << name << endl;

}

int main()

{

// 调用函数

welcome();

return 0;

}
```

这样每一部分处理逻辑都可以分块包装成函数，主函数的执行过程看起来就简单多了。当然，如果认为一个文件中有太多函数也会影响可读性，我们还可以把它们分开。比如新建一个叫做welcom.cpp的源文件，专门放刚才的welcome函数。而在主函数中，需要额外对它做一个“声明”，表示有这样一个函数，它的实现在另外的文件里。
```cpp
#include <iostream> // 用于 cout 和 cin

#include <string> // 用于 string

using namespace std;

  

// welcome 函数定义

void welcome() {

cout << "Hello World!" << endl;

cout << "请输入您的大名：" << endl;

string name;

cin >> name;

cout << "Hello, " << name << endl;

}
```
main.cpp
```cpp
#include <iostream>

using namespace std;

  

// 声明 welcome 函数

void welcome();

  

int main() {

welcome(); // 调用 welcome 函数

return 0;

}
```
终端输入
```bash
g++ main.cpp welcome.cpp -o myprogram
./myprogram
```
将两个cpp文件编译链接到一起为myprogrm输出
![[Pasted image 20250117150904.png]]
函数是C++中基本的编程单元，也是“模块化编程”的核心思想，我们还会在后面的章节详细展开。
