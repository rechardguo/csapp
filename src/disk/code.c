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
     {IMM, 0x10, 0, NULL, 0},
     {REG, 0, (uint64_t *)&reg.rsp, NULL, 0},
     "sub    $0x10,%rsp"
 },
 {
     mov_imm_mm,
     {IMM, 0x2, 0, NULL, 0},
     {MM_IMM_REG, -0x10, (uint64_t *)&reg.rbp, NULL, 0},
     "movq   $0x2,-0x10(%rbp)"
 },
 {
     mov_imm_mm,
     {IMM, 0x5, 0, NULL, 0},
     {MM_IMM_REG, -0x8, (uint64_t *)&reg.rbp, NULL, 0},
     "movq   $0x5,-0x8(%rbp)"
 },
 {
     mov_mm_reg,
     {MM_IMM_REG, -0x8, (uint64_t *)&reg.rbp, NULL, 0},
     {REG, 0, (uint64_t *)&reg.rdx, NULL, 0},
     "mov    -0x8(%rbp),%rdx"
 },
{
     mov_mm_reg,
     {MM_IMM_REG, -0x10, (uint64_t *)&reg.rbp, NULL, 0},
     {REG, 0, (uint64_t *)&reg.rax, NULL, 0},
     "mov    -0x10(%rbp),%rax"
 },
{
     mov_reg_reg,
     {REG, 0, (uint64_t *)&reg.rdx, NULL, 0},
     {REG, 0, (uint64_t *)&reg.rsi, NULL, 0},
     "mov    %rdx,%rsi"
 },
 {
     mov_reg_reg,
     {REG, 0, (uint64_t *)&reg.rax, NULL, 0},
     {REG, 0, (uint64_t *)&reg.rdi, NULL, 0},
     "mov    %rax,%rdi"
 },
 {
      call,
      {IMM, (uint64_t)&(program[11]),  NULL,  0 },
      {EMPTY, 0, NULL, NULL, 0},
      "callq  5fa <add>"
 },
 {
     mov_mm_reg,
     {IMM, 0x0, NULL, NULL, 0},
     {REG, 0, (uint64_t *)&reg.eax, NULL, 0},
     "mov    $0x0,%eax"
 },
 // add funcion
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
   mov_reg_mm,
   {REG, 0, (uint64_t *)&reg.rdi, NULL, 0},
   {MM_IMM_REG, -0x8, (uint64_t *)&reg.rbp, NULL, 0},
   "mov    %rdi,-0x8(%rbp)"
 },
  {
   mov_reg_mm,
   {REG, 0, (uint64_t *)&reg.rsi, NULL, 0},
   {MM_IMM_REG, -0x10, (uint64_t *)&reg.rbp, NULL, 0},
   "mov    %rsi,-0x10(%rbp)"
 },
 {
   mov_mm_reg,
   {MM_IMM_REG, -0x8,(uint64_t *)&reg.rbp, NULL, 0},
   {REG, 0, (uint64_t *)&reg.rdx, NULL, 0},
   "mov    -0x8(%rbp),%rdx"
 },
 {
   mov_mm_reg,
   {MM_IMM_REG, -0x10,  (uint64_t *)&reg.rbp, NULL, 0},
   {REG, 0, (uint64_t *)&reg.rax, NULL, 0},
   "mov    -0x10(%rbp),%rax"
 },
 {
  add,
   {REG, 0, (uint64_t *)&reg.rdx, NULL, 0},
   {REG, 0, (uint64_t *)&reg.rax, NULL, 0},
   "add    %rdx,%rax"
 },
 {
   pop_reg,
    {REG,    0,   (uint64_t *)&reg.rbp,   NULL , 0 },
    {EMPTY,  0,   NULL,                   NULL  ,0},
    "pop   %rbp"
 },
 {
    ret,
    {EMPTY,  0,   NULL,   NULL ,0 },
    {EMPTY,  0,   NULL,   NULL ,0 },
    "retq"
 }

};