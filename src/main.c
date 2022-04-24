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
    reg.rax = 0x8000650;
    reg.rbx = 0x0;
    reg.rcx = 0x8000650;
    reg.rdx = 0x7ffffffedce8;
    reg.rsi = 0x7ffffffedcd8;
    reg.rdi = 0x1;
    reg.rbp = 0x7ffffffedbf0;
    reg.rsp = 0x7ffffffedbf0; // 栈顶的内存地址
    //rig 是代码段的指针
    reg.rip = (uint64_t)&program[0];

    //初始化内存
    //由于程序的执行是压栈执行
    //reg.rbp = 0x7ffffffedbf0
    //reg.rsp = 0x7ffffffedbe0; // 栈顶的内存地址
    //通过 x/10 0x7ffffffedbe0 可以查到 0x7ffffffedbe0
    // (gdb) x/10 $rsp
    // 0x7ffffffedbe0: -74544  32767   0       0
    // 0x7ffffffedbf0: 134219344       0       -16638841       32767
    // 0x7ffffffedc00: 1       0
    write64bits_dram(va2pa(0x7ffffffedbf0), 0x134219344); // rbp
    write64bits_dram(va2pa(0x7ffffffedbc0), 0x0);
    write64bits_dram(va2pa(0x7ffffffedbe8), 0x0);
    write64bits_dram(va2pa(0x7ffffffedbe4), 0x32767);
    write64bits_dram(va2pa(0x7ffffffedbe0), -0x74544); // rsp

    printStack();
    printRegister();

    for (size_t i = 0; i < 10; i++)
    {
        //取值执行
        instruction_cycle();
        printStack();
        printRegister();

    }
    
    int match=0;
    //verify register
    // rax            0x7      7
    // rbx            0x0      0
    // rcx            0x8000640        134219328
    // rdx            0x2      2
    // rsi            0x5      5
    // rdi            0x2      2
    // rbp            0x7ffffffedbf0   0x7ffffffedbf0
    // rsp            0x7ffffffedbe0   0x7ffffffedbe0
    if( reg.rax == 0x7 && reg.rbx == 0x0 && reg.rcx ==0x8000650 && reg.rdx==0x2 
       &&reg.rsi == 0x5 && reg.rdi == 0x2 && reg.rbp == 0x7ffffffedbf0 && reg.rsp == 0x7ffffffedbe0 ){
         printf(" register match \n");
         match=1;
      } 
    match=0;
    // 0x7ffffffedbe0: -74544  32767   2       5
    // 0x7ffffffedbf0: 134219328       0       -16638841       32767
    // 0x7ffffffedc00: 1       0
    //verify memeory
    if( read64bits_dram(va2pa(0x7ffffffedbf0)) ==  0x134219344 &&
      read64bits_dram(va2pa(0x7ffffffedbc0)) ==  0x0 &&
      read64bits_dram(va2pa(0x7ffffffedb08)) ==  0x5 &&
      read64bits_dram(va2pa(0x7ffffffedb04)) ==  0x0 &&
      read64bits_dram(va2pa(0x7ffffffedbe0)) ==  0x2 ){
         printf(" memory match \n");
         match=1;
     } 
     if(match)
        printf(" match \n");
     else
        printf(" not match \n");
   return 0;
}

