#include "memory/instruction.h"
#include "memory/dram.h"
#include "cpu/register.h"
#include "cpu/mmu.h"
#include <stdio.h>
//csapp p121
static uint64_t decode_od(od_t od){

    if(od.type == IMM)
    {
        return *((uint64_t *)&od.imm);
    }
    else if (od.type == REG)
    {
        return (uint64_t) od.reg1;
    }
    else
    {
        uint64_t vaddr=0;
        //内存地址立即数
        //内存地址是虚拟的
        if(od.type==MM_IMM){
            vaddr = od.imm;
        }else if (od.type==MM_REG) {
            vaddr = *(od.reg1);
        }else if (od.type==MM_IMM_REG) {
            vaddr = od.imm+*(od.reg1);
        }else if (od.type==MM_REG1_REG2){
            vaddr = *(od.reg1)+*(od.reg2);
        }else if (od.type==MM_IMM_REG1_REG2){
            vaddr = od.imm+*(od.reg1)+*(od.reg2);
        }else if (od.type==MM_REG2_S){
            vaddr = *(od.reg2)*od.scal;
        }else if (od.type==MM_IMM_REG2_S){
            vaddr = od.imm + *(od.reg2)*od.scal;
        }else if (od.type==MM_REG1_REG2_S){
            vaddr = *(od.reg1) + *(od.reg2)*od.scal;
        }else if (od.type==MM_IMM_REG1_REG2_S){
            vaddr = od.imm + *(od.reg1) + *(od.reg2)*od.scal;
        }
        
        return va2pa(vaddr);
        
    }
    
}

void instruction_cycle(){
    // rip是指针，指向了代码段里的1个代码
    // 指针指向的是一个inst的地址
    inst_t *instr=(inst_t *)reg.rip;
    
    //imm:imm  虚拟地址
    //reg:value 寄存器的值
    //mm:paddr physical adress
    uint64_t src=decode_od(instr->src);
    uint64_t dst=decode_od(instr->dst);
    handler_t fun=handler_table[instr->op];
    printf("\n\n%s\n\n" , instr->code);
    fun(src,dst);
}

void init_function_table(){
    //enum类型可以作为index
    handler_table[push_reg] = &push_reg_handler;
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
  //  function_table[mov_imm_reg] = &mov_imm_reg_handler;
}


void push_reg_handler(uint64_t src,uint64_t dst){
    // src: reg
    // dst: empty
    reg.rsp = reg.rsp - 0x8;
    write64bits_dram(
        va2pa(reg.rsp),
        *(uint64_t *)src
    );
    reg.rip = reg.rip + sizeof(inst_t);

}

void mov_reg_reg_handler(uint64_t src, uint64_t dst){

}


void mov_imm_reg_handler(uint64_t src,uint64_t dst){
   *(uint64_t*)dst = src;
    //跳到下条指令
    reg.rip=reg.rip+sizeof(inst_t);
}

/**
 * @brief 
 *  这里的src不是真的寄存器，而是一个地址
 *  所以要新转成地址 (uint64_t*)src 再用*取值
 * @param src 寄存器1
 * @param dst 寄存器2
 */
void add_reg_reg_handler(uint64_t src,uint64_t dst){
    *(uint64_t*)dst = *(uint64_t*)src+*(uint64_t*)dst;
    //跳到下条指令
    reg.rip=reg.rip+sizeof(inst_t);
}

