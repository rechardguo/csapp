#include "memory/instruction.h"
#include "cpu/register.h"
#include "disk/elf.h"

//汇编指令
inst_t program[INST_LEN]={
 {
    push_reg,
    {REG, 0,  (uint64_t *)&reg.rbp, NULL, 0}, //type,imm,reg1,reg2,scal
    {EMPTY, 0, NULL, NULL, 0},       
     "push   %rbp"
 }
};