#include <stdio.h>
#include "cpu/mmu.h"
#include "memory/dram.h"
#include "cpu/register.h"

#define SRAM_CACHE_SETTING 0

// void printStack(){
//     //reg.rsp low
//     //va->pa
//     uint64_t vaddr = va2pa(reg.rsp);

//     //打印十个
//     for(int i=0;i<8*10;i++){

//     }
//     mm[vaddr];

// }


void printStack()
{
    int n = 10;

    uint64_t *high = (uint64_t*)&mm[va2pa(reg.rsp)];
    //high可以看成数组名，也是指针
    //high[n] 就是数组后的第n项
    high = &high[n];

    uint64_t rsp_start = reg.rsp + n * 8;

    for (int i = 0; i < 2 * n; ++ i)
    {
        uint64_t *ptr = (uint64_t *)(high - i);
        printf("0x%016lx : %16lx", rsp_start, (uint64_t)*ptr);

        if (i == n)
        {
            printf(" <== rsp");
        }

        rsp_start = rsp_start - 8;

        printf("\n");
    }
}

void printRegister(){
    printf("rax=0x%lx \t rbx=0x%lx \t rdx=0x%lx \t  rsi=0x%lx \t\n",reg.rax,reg.rbx,reg.rdx,reg.rsi);
    printf("rdi=0x%lx \t rbp=0x%lx \t rsp=0x%lx \t  rip=0x%lx \t\n",reg.rdi,reg.rbp,reg.rsp,reg.rip);
}

/**
 * @brief 将data写入到paddr里
 * 
 * @param data 
 * @param paddr 
 */
void write64bits_dram(uint64_t addr, uint64_t data){

    if (SRAM_CACHE_SETTING == 1)
    {
        return;
    }

    mm[addr]= data & 0xff;
    mm[addr + 1]= (data>>8)  & 0xff;
    mm[addr + 2]= (data>>16) & 0xff;
    mm[addr + 3]= (data>>24) & 0xff;
    mm[addr + 4]= (data>>32) & 0xff;
    mm[addr + 5]= (data>>40) & 0xff;
    mm[addr + 6]= (data>>48) & 0xff;
    mm[addr + 7]= (data>>56) & 0xff;
}

/**
 * @brief 从地址里读出数值
 * 
 * @param paddr 
 * @return uint64_t 
 */
uint64_t read64bits_dram(uint64_t addr){
    if (SRAM_CACHE_SETTING == 1)
    {
        return 0x0;
    }
   return  *((uint64_t *)&mm[addr]);
}