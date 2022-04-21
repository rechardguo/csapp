# target
使用c语言编写一个汇编处理器来解析汇编指令


```c
uint64_t add(uint64_t,uint64_t);
int main(int argc, char const *argv[])
{
   printf("%d",add(1,2));
   return 0;
}

uint64_t add(uint64_t i1,uint64_t i2){
    return i1+i2;
}
```
通过gcc -Og -S main.c 得到main.s,截取重要的汇编如下


```s
main:
	movl	$2, %edx
	movl	$1, %ecx
	call	add
	movq	%rax, %rdx
	leaq	.LC0(%rip), %rcx
	call	printf
	movl	$0, %eax
	addq	$40, %rsp
add:
    leaq	(%rcx,%rdx), %rax
	ret    
```

通过gcc -g main.c -o main 得到main 

gdb main
l -- 列出代码
b 7 -- 断点到哪行
run -- 单步运行
info r -- 查看寄存器里的值


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


# c语言的基础知识

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

- include

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

## 按ctrl+鼠标左键单击不能跳转问题

在vscode界面右侧，找到Extensions，在搜索框，输入C/C++,安装即可

