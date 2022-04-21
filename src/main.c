#include<stdlib.h>
#include<stdio.h>

#include "cpu/register.h"
#include "cpu/mmu.h"
#include "memory/instruction.h"
#include "memory/dram.h"
#include "disk/elf.h"

int main()
{
    init_function_table();

    //gdb 
    //b 7 
    //run
    //info r
    //程序加载到内存里
    //手工完成
    reg.rax = 0x800065e;
    reg.rbx = 0x0;
    reg.rcx = 0x80006b0;
    reg.rdx = 0x7ffffffedce8;
    reg.rsi = 0x7ffffffedcd8;
    reg.rdi = 0x1;
    reg.rbp = 0x7ffffffedbf0;
    reg.rsp = 0x7ffffffedbe0; // 栈顶的内存地址
    //rig 是代码段的指针
    reg.rip = (uint64_t)&program[0];

    //初始化内存
    //由于程序的执行是压栈执行
    //reg.rsp = 0x7ffffffedbe0; // 栈顶的内存地址
    //通过 x/10 0x7ffffffedbe0 可以查到 0x7ffffffedbe0
    mm[va2pa(0x7ffffffedbe8)]=2;
    mm[va2pa(0x7ffffffedbec)]=5;

    printStack();
    printRegister();

    for (size_t i = 0; i < INST_LEN; i++)
    {
        //取值执行
      //  instruction_cycle();
    }
    
    //verify

   return 0;
}

