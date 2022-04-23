#ifndef reg_guard
#define reg_guard
#include<stdlib.h>
#include<stdint.h>

/**
 * @brief 
 * cpu 内部所有的寄存器集合
 * 
 * csapp p120
 * 
 * 只定义了前8个
 */
typedef struct REG_STRUCT
{
    /* data */
    union 
    {
        struct 
        {
            uint8_t al;
            uint8_t ah;
        };
        uint16_t ax;
        uint32_t eax;
        uint64_t rax;
    };
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;

    uint64_t rip; //指针
} reg_t;

//全局变量
 reg_t reg;
#endif