﻿# 作者的源码
git clone https://github.com/yangminz/bcst_csapp.git

下载后要切到某个commit
git reset --hard <commit version>
就可以切换到某个 version

- 第一次版本重构的代码 
https://www.bilibili.com/video/BV17K4y1N7Q2?p=9
git reset --hard 004c77a
对应的我的tag :1strefactory

- 关于operand的解析
git reset --hard 34beeb4
![anchor text](./addr_access.png "title")

- 
git reset --hard 7ec8dbe

- 设计可执行与可链接的文本格式，.elf.txt。从磁盘上的文件中读取到内存
git reset --hard d7f871c

- 解析.elf.txt的节头表与符号表
git reset --hard 7842d9c

- 静态链接的第一步：符号解析，设计内部的符号表作为维护符号关系的数据结构
git reset --hard 32dff69

- 静态链接的第二步：将ELF的Section合并为EOF的Segment，并且计算Segment的运行时起始地址
git reset --hard d2c4f61

- 静态链接的第三步：将ELF文件中的符号引用重定位到它们在EOF中的符号
git reset --hard d0be385

- 完成静态链接，计算符号的运行时地址。动态链接：GOT与PLT

git reset --hard febae2c 


# target
使用c语言编写一个汇编处理器来解析汇编指令
例如一个简单的add c语言程序如下
```c
unsigned add(unsigned i1,unsigned i2){
    return i1+i2;
}

int main(){
    int i1 = 2;
    int i2 = 5;
    add(i1,i2);
}
```

通过gcc -Og -S add.c 得到add.s
通过gcc -g add.c 得到main

通过objdump -d add > add.txt , 保留main和add

```s
00000000000005fa <add>:
 5fa:	55                   	push   %rbp
 5fb:	48 89 e5             	mov    %rsp,%rbp
 5fe:	89 7d fc             	mov    %edi,-0x4(%rbp)
 601:	89 75 f8             	mov    %esi,-0x8(%rbp)
 604:	8b 55 fc             	mov    -0x4(%rbp),%edx
 607:	8b 45 f8             	mov    -0x8(%rbp),%eax
 60a:	01 d0                	add    %edx,%eax
 60c:	5d                   	pop    %rbp
 60d:	c3                   	retq   

000000000000060e <main>:
 60e:	55                   	push   %rbp
 60f:	48 89 e5             	mov    %rsp,%rbp
 612:	48 83 ec 10          	sub    $0x10,%rsp
 616:	c7 45 f8 02 00 00 00 	movl   $0x2,-0x8(%rbp)
 61d:	c7 45 fc 05 00 00 00 	movl   $0x5,-0x4(%rbp)
 624:	8b 55 fc             	mov    -0x4(%rbp),%edx
 627:	8b 45 f8             	mov    -0x8(%rbp),%eax
 62a:	89 d6                	mov    %edx,%esi
 62c:	89 c7                	mov    %eax,%edi
 62e:	e8 c7 ff ff ff       	callq  5fa <add>
 633:	b8 00 00 00 00       	mov    $0x0,%eax
 638:	c9                   	leaveq 
 639:	c3                   	retq   
 63a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)    
```

gdb add
- l -- 列出代码
- b 7 -- 断点到哪行
- run -- 运行
- n -- next 单步运行
- n 2 -- 下2步
- step -- 进入
- info r -- 查看寄存器里的值


Gdb调试过程：
    
1、程序经过预处理后，即进入编译阶段，进入编译阶段，首先声明编译：
2、格式：gdb -o test test.c -g
3、进入编译：gdb test
4、显示需要编译调试的源程序：l(list)//list filename
5、设置断点：b（break）行号
6、查看设置的断点：info b
7、运行调试程序：run
8、跳到下一个断点：c（continue）
9、单步运行的话使用：n（next）/s(step into)跳到函数体 //区别在与：next执行函数体，而step不执行函数体
10、调试过程中查看某个变量的变化：print i （每次都要手动设置）//display i（设置一次一直尾随，直到用“undisplay 变量标号” 停止）
11、退出当前的调试使用finish 跳出函数
12、清楚断点 clear 行号
13、Delete 断点信息序号// 删除所有断点或设置的要删除的断点
14、退出调试 q
15、b num if i20 设置断点的触发条件
16、condition num i50 改变断点的触发条件

现在要做的就是要使用c语言来模拟上述的过程，最重要就是指令的解析


# c语言的基础知识

- 方法调用
- - 正常的做法是在fn.h里定义头，fn.c里实现，然后在main.c里include "fn.h"
- - 还有一种是
fn.c 和main.c 里都定义了头, fn.c里实现，main.c 里就可以直接用了
e.g

fn.c
```c
int add(int,int);
int add(int i,int j){
	int r= i+j;
	return r;
} 
```
main.c
```c
int add(int,int);
int main(){	
	add(1,2);
	return 0;
}
```
gcc fn.c main.c -o main 编译通过

- static 
> A static function in C is a function that has a scope that is limited to its object file. This means that the static function is only visible in its object file

static 定义function的可见性只在当前文件里

什么时候应该使用static？

- inline
告诉编译器进行优化使用的，https://blog.csdn.net/caoyaobin/article/details/43735361

inline 不需要在头的地方定义,例如
```h
void reset_cflags(core_t *cr);
```
实现
```c
inline void reset_cflags(core_t *cr){
    ...
}
```


- pointer
 
 ```c
int a=124;
int *p = &a;
// *p等同a
// 使用指针赋值操作
*p=567; 
 ```

- typedef的使用
- union
整个看成1个数据结构 
```
 union 
    {
        /* data */
        struct 
        {
            /* data */
            uint8_t al;
            uint8_t ah;
        };
        uint16_t ax;
        uint32_t eax;
        uint64_t rax;
    };
    
```
rax就是真个结构是64位，其中eax占了0-31, ax占了0-16，al占了0-7，ah占了8-15

- printf 里的一些格式化 
%x对应int 和unsigned int. %lx对应long 和unsigned long
%x是以16进制输出整型数据，%lx就是以16进制输出长整型数据
%p 对应的是指针


%a(%A)     浮点数、十六进制数字和p-(P-)记数法(C99) 
%c         字符 
%d         有符号十进制整数 
%f         浮点数(包括float和doulbe) 
%e(%E)     浮点数指数输出[e-(E-)记数法] 
%g(%G)     浮点数不显无意义的零"0" 
%i         有符号十进制整数(与%d相同) 
%u         无符号十进制整数 
%o         八进制整数 
%x(%X)     十六进制整数0f(0F)   e.g.   0x1234 
%lx        long 
%p         指针 
%s         字符串
%u         无符号

- 运算符的优先级
第1级是[],(),.,-> 等 ，*，+，-，&是第2级 同级别从左到右 


- include


- 数组的写法

不正确 
```c
char [MAX_ELF_FILE_LINES][MAX_ELF_FILE_PERLINE_COUNT]code
```

正确
```c
char code[MAX_ELF_FILE_LINES][MAX_ELF_FILE_PERLINE_COUNT]
```

- 数组的用法
**以下的问题需要注意，这个发了很长时间debug**
```c
    char arr3[10];
    for(int i=0;i<10;i++){
          arr3[i]=(i+1);
    }

    for(int i=0;i<10;i++){
          char ch = arr3[i];
          // 打印出来的&ch是永远不变的
          printf("%d ,%p \n",ch,&ch);
          // 要打印出char指针，需要使用  
          printf("%d ,%p \n",ch,&arr3[i]);
    }
```
相当于
```c
char arr3[10];
    for(int i=0;i<10;i++){
          arr3[i]=(i+1);
    }
    char ch ; 
    for(int i=0;i<10;i++){
          ch = arr3[i];
          char *chp = &ch;
          //&ch 永远不会变的
          printf("%p \n",chp);
    }
```




- malloc
什么时候需要使用malloc

# vscode+gcc+win10环境配置

参考 https://blog.csdn.net/weixin_40877998/article/details/114713368

gcc 后面跟的参数 -w  -Wall什么意思？

-w的意思是关闭编译时的警告，也就是编译后不显示任何warning，因为有时在编译之后编译器会显示一些例如数据转换之类的警告，这些警告是我们平时可以忽略的。

-Wall选项意思是编译后显示所有警告。


# makefile的编写

注意在Makefile文件中，命令必须以【tab】键开始。

## -I 的解释
-I DIRECTORY, --include-dir=DIRECTORY
                              Search DIRECTORY for included makefiles

> 需要将src目录包含进源文件的路径，预处理器通常会在特定的系统路径下搜索，例如，在 Unix 系统中，会搜索路径 /usr/local/include 与 /usr/include, -I$(src)就是将源文件包含进路径里



# 遇到的问题

## 问题1

> make : 无法将“make”项识别为 cmdlet、函数、脚本文件或可运行程序的名称

解决方法：

> 找到mingw64\bin 复制mingw32-make.exe一份成make.ex

## 问题2

> 运行make main 时候报
> makefile:12: *** missing separator.  Stop.

解决方法：

> 注意在Makefile文件中，命令必须以【tab】键开始。
例如：
```
target：prerequisites
command
……
```

改为：
```
target：prerequisites
    command
……
```
就可以了。

## 问题3
> process_begin: CreateProcess(NULL, cc -c -o main.o main.c, ...) failed.


## 问题4
> gdb 出现 no debugging symbols found

一定要加 -g
```
gcc -g main.c -o main
``` 
gdb main


## 问题5

> /src/disk/code.c:10:14: error: initialization makes pointer from integer without a cast [-Werror=int-conversion]
     {REG, 0, reg.rbp, NULL, 0}, //type,imm,reg1,reg2,scal

为什么要写成 (uint64_t *)&reg.rbp
这是因为  inst的reg1的寻址类型 imm ,地址1，地址2 。所以这里要写成地址


# 问题6
> ./src/hardware/cpu/isa.c:234:20: error: comparison between pointer and integer [-Werror]

代码如下
```c
typedef enum {
    A,B
}op_type_t;


op_type_t type;
if (type==NULL){
    ...
}
```
由于java代码转过来，不知道c的语法.op_type_t type 是不为NULL的，并且初始值是0

# 问题7

```c
 for (int i = 0; i < 11; ++ i)
    {
        od_t od; //为啥每次地址都一样
        printf("od %p \n", &od);
    }
```

## 问题8

> unknown type name
原因是在c里, struct a 引用了struct b. b必须定义在a前

## 问题9 
**未解决**
为了能够断点debug，没有按up作者使用python建立cmd.py。
而是使用.vscode里建立 launch.json 和task.json

launch.json里需要配置
```json
"cwd": "${workspaceFolder}/csapp"
```
才能elf文件解析成功,否者就是报错

# vscode 使用问题

## 按ctrl+鼠标左键单击不能跳转问题

在vscode界面右侧，找到Extensions，在搜索框，输入C/C++,安装即可

## 代码不能提示错误，每次要gcc的时候才能提示，有没像java一样可以直接提示错误的
【文件】-【首选项】-【设置】，然后输入Error Squiggles回车，调成Enable


## debug
出现这种问题的原因⼤部分是因为GCC⾃动开启了O2优化选项，也有可能是⼈为的O1，O2，O3，Os优化，这都有可能导致在单步调试 查看某个变量出现optimized out,所⽰代码就会导致⽆法单步调试。
解决⽅案：
对于GCC⾃动开启的优化：
在编译选项中把  改为 -O2 为-O0




# github里添加ssh

- 1. ssh-keygen -t rsa -b 4096 -C "sdragonguo@163.com"
- 2. 将生产的文件

# git

- git reset --hard <tagname>



# vocabulary

1. dram  内存
2. va 虚拟地址, 所有的寄存里的都