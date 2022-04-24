#ifndef inst_guard
#define inst_guard

#include<stdlib.h>
#include<stdint.h>


/**
 * @brief 
 * 函数指针
 * 固定用法： 数据类型 (*指针变量名) ();
*  eg. int (*ptr) ();
*  已有声明函数 min,则ptr = min 可以令指针指向函数体；
*  而调用函数的min(…) 也变成 (*ptr)(…)
 */
typedef void (*handler_t) (uint64_t,uint64_t);

#define FUNCTION_LEN 100

handler_t handler_table[FUNCTION_LEN];


/**
 * 
 * operation operand
 * operand = OD
 * 
 */
typedef enum OD_TYPE {
    /* data */
    EMPTY,
    IMM, //立即数
    REG,  //寄存器 
    MM_IMM, 
    MM_REG,
    MM_IMM_REG,
    MM_REG1_REG2,
    MM_IMM_REG1_REG2,
    MM_REG2_S,
    MM_IMM_REG2_S,
    MM_REG1_REG2_S,
    MM_IMM_REG1_REG2_S
}od_type_t;

//操作数
typedef struct OD
{
    od_type_t type;
    int64_t imm;
    uint64_t *reg1;
    uint64_t *reg2;
    uint64_t scal;
} od_t;

//指令类型
typedef enum OP_TYPE{
    push_reg,
    pop_reg,
    mov_reg_reg,
    sub_imm_reg,
    mov_imm_mm,
    mov_mm_reg,
    add_reg_reg,
    mov_reg_mm,
    add,
    ret,
    call
}op_type_t;

//指令
typedef struct  INSTRUCTION{
    op_type_t op;  //指令的类型
    od_t src;  //操作数1
    od_t dst; //操作数2
    char code[100];
} inst_t;

//指令周期
//IF instruction fetch 取值
void instruction_cycle();
void init_function_table();

void mov_imm_reg_handler(uint64_t , uint64_t);
void push_reg_handler(uint64_t , uint64_t);
void sub_imm_reg_handler(uint64_t , uint64_t);
void mov_reg_reg_handler(uint64_t , uint64_t);
void mov_imm_mm_handler(uint64_t , uint64_t);
void mov_mm_reg_handler(uint64_t , uint64_t);
void mov_reg_mm_handler(uint64_t , uint64_t);
void call_handler(uint64_t , uint64_t);
void add_reg_reg_handler(uint64_t src, uint64_t dst);
void pop_reg_handler(uint64_t src, uint64_t dst);
void return_handler(uint64_t src, uint64_t dst);
#endif
