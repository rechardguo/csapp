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
 },
 {
     mov_reg_reg,
     {REG, 0,  (uint64_t *)&reg.rsp, NULL, 0},
     {REG, 0,  (uint64_t *)&reg.rbp, NULL, 0},
     "mov    %rsp,%rbp"
 },
 {
     sub_imm_reg,
     {IMM, 16, NULL, NULL, 0},
     {REG, 0,  (uint64_t *)&reg.rsp, NULL, 0},
     "sub    $0x10,%rsp"
 }
};