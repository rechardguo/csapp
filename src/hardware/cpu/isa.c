#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<headers/cpu.h>
#include<headers/memory.h>
#include<headers/common.h>
#include<headers/instruction.h>

/*======================================*/
/*      parse assembly instruction      */
/*======================================*/

// functions to map the string assembly code to inst_t instance
static void parse_instruction(const char *str, inst_t *inst, core_t *cr);
static void parse_operand(const char *str, od_t *od, core_t *cr);
static uint64_t decode_operand(od_t *od);
static uint64_t reflect_register(const char *str, core_t *cr);
op_t str2op_t (const char* str);

void TestParsingOperand();


// interpret the operand
static uint64_t decode_operand(od_t *od)
{
    if (od->type == IMM)
    {
        // immediate signed number can be negative: convert to bitmap
        return *(uint64_t *)&od->imm;
    }
    else if (od->type == REG)
    {
        // default register 1
        return od->reg1;
    }
    else if (od->type == EMPTY)
    {
        return 0;
    }
    else
    {
        // access memory: return the physical address
        uint64_t vaddr = 0;

        if (od->type == MEM_IMM)
        {
            vaddr = od->imm;
        }
        else if (od->type == MEM_REG1)
        {
            vaddr = *((uint64_t *)od->reg1);
        }
        else if (od->type == MEM_IMM_REG1)
        {
            vaddr = od->imm + (*((uint64_t *)od->reg1));
        }
        else if (od->type == MEM_REG1_REG2)
        {
            vaddr = (*((uint64_t *)od->reg1)) + (*((uint64_t *)od->reg2));
        }
        else if (od->type == MEM_IMM_REG1_REG2)
        {
            vaddr = od->imm +  (*((uint64_t *)od->reg1)) + (*((uint64_t *)od->reg2));
        }
        else if (od->type == MEM_REG2_SCAL)
        {
            vaddr = (*((uint64_t *)od->reg2)) * od->scal;
        }
        else if (od->type == MEM_IMM_REG2_SCAL)
        {
            vaddr = od->imm + (*((uint64_t *)od->reg2)) * od->scal;
        }
        else if (od->type == MEM_REG1_REG2_SCAL)
        {
            vaddr = (*((uint64_t *)od->reg1)) + (*((uint64_t *)od->reg2)) * od->scal;
        }
        else if (od->type == MEM_IMM_REG1_REG2_SCAL)
        {
            vaddr = od->imm + (*((uint64_t *)od->reg1)) + (*((uint64_t *)od->reg2)) * od->scal;
        }
        return vaddr;
    }
    
    // empty
    return 0;
}

static void parse_instruction(const char *str, inst_t *inst, core_t *cr)
{
    // str op
    // inst_t e.g  mov -0x18(%rbp),%rdx

    char op[60]={"/0"};
    int op_len=0;
    char od1[60]={"/0"};
    int od1_len=0;
    char od2[60]={"/0"};
    int od2_len=0;

    int state=0;
    for(int i=0;i<strlen(str);++i){
       char ch=str[i];
       if(state == 0){
        if(ch !=' ' ){
           op[op_len] = ch;
           op_len++;
           state = 1; 
           continue;
        }
       }else if(state == 1){
           if(ch == ' '){
               state = 2;
               continue;
           }else{
               op[op_len] = ch;
               op_len++;
           }
       }else if(state == 2 && ch!=' '){
          if( ch ==',')
             goto fail;
          state=3;
          od1[od1_len] = ch;
          od1_len++;
          continue;
       }else if(state == 3){
            if(ch == ','){
              state = 6;
              continue;
            }else if(ch =='('){  
                state = 4;
            }else if(ch ==')')
               goto fail; // ) must after (
            od1[od1_len] = ch;
            od1_len++;  
            continue;       
       }else if(state == 4){
            if( ch ==',')
             goto fail;
            if(ch ==')')
               state =5; 
            od1[od1_len] = ch;
            od1_len++;     
            continue;    
       }else if(state == 5){
            if( ch ==','){
               state = 6;
               continue;
            }
       }else if(state == 6){
            od2[od2_len] = ch;
            od2_len++;  
       }
    }

    // now we have op od1 od2
    inst->op = str2op_t(op);
    if( od1_len!=0 )
        parse_operand(od1 , &inst->src ,cr);
    if( od2_len!=0 )
        parse_operand(od2 , &inst->dst ,cr);

    return;

    fail:
     printf("%s can not parse \n" , str);
     exit(0);

}

static void parse_operand(const char *str, od_t *od, core_t *cr)
{

    //初始化
    od->type = EMPTY;
    od->imm = 0;
    od->scal = 0;
    od->reg1 = 0;
    od->reg2 = 0;

    //make sure str is not empty
    if(str == NULL || strlen(str) == 0){
        printf(" operand str must be not empty \n");
        goto fail;
    }

    char ch= str[0];
    if(ch == '$'){
        //IMM
        od->type = IMM;
        od->imm = string2uint(&str[1]);
        od->reg1 = 0;
        od->reg2 = 0;
        od->scal = 0;
        return;
    }else if( ch == '%'){
        //REG
        od->type = REG;
        od->imm = 0;
        od->reg1 = reflect_register(str , cr);
        od->reg2 = 0;
        od->scal = 0;
        return;
    }else{
        //type = mm
        //parsing str 
        char immStr[60] = {'\0'};
        int immLen = 0;
        char reg1Str[60] = {'\0'};
        int reg1Len = 0;
        char reg2Str[60] = {'\0'};
        int reg2Len = 0;
        char scalStr[60] = {'\0'};
        int scalLen = 0;
        //e.g  -0x18(%rbp)  -0x18(%rbp,,)
        // state =0 ??
        // state =1 ??(...
        // state =2 (??,
        // state =3 (??,??,
        // state =4 (??)
        int state = 0;
        for (int i=0;i<strlen(str);++i){
            char ch = str[i];
            if( ch == '(' ){
                if( state == 0 ){ // (  or  ??(
                   state = 1;
                }else{
                    printf("oprand %s is incorrect format \n", str);
                    goto fail; 
                }   
            }else if( ch == ')' ){
                 if( state >=1 && state <4 ){
                     state = 4;
                 }else{
                    printf("oprand %s is incorrect format \n", str);
                    goto fail; 
                }   
            }else if( ch == ',' ){
                if(state==1){
                    //???(??,
                    state = 2;
                }else if(state==2){
                    //???(??,??,
                    state = 3;
                }else{
                    goto fail;
                }
            }else{ 
                 // action for meet char not  ( or ) or ，
               if( state == 0 ){
                   //imm
                   immStr[immLen] = ch;
                   immLen++;
               }else{
                  if( state == 1 ){
                      //reg1
                      reg1Str[reg1Len] = ch;
                      reg1Len++;
                  } else if( state ==2 ){
                      //reg2
                      reg2Str[reg2Len] = ch;
                      reg2Len++;
                  }else if( state ==3 ){
                      //scal
                      scalStr[scalLen] = ch;
                      scalLen++;
                  }else{ // state =4 , should not meet any letter
                      printf("oprand %s is incorrect format \n", str);
                      goto fail;
                  }
               }
            }
        }
        // now we got immStr, reg1Str, reg2Str, scalStr 
        od_type_t type = EMPTY;
        if(immLen!=0){
           od->imm =  string2uint(immStr);
           type = MEM_IMM;
        }
        if(reg1Len!=0){
           if (type==EMPTY) 
              type = MEM_REG1;
           else
               type = MEM_IMM_REG1;
           od->reg1 = reflect_register(reg1Str , cr);
        }

        if(reg2Len!=0){
           if (type==EMPTY && scalLen!=0) 
              type = MEM_REG2_SCAL; // only this one
           else if(type == MEM_IMM && scalLen!=0){
              type = MEM_IMM_REG2_SCAL;   // only this one  
           }else if (type == MEM_REG1){
              type = MEM_REG1_REG2;
           }else if (type == MEM_IMM_REG1)
               type = MEM_IMM_REG1_REG2;
           od->reg2 = reflect_register(reg2Str , cr);
        }
 
        if(scalLen!=0){
           if(type==MEM_IMM_REG1_REG2){
              type = MEM_IMM_REG1_REG2_SCAL;
           }
           od->scal = string2uint(scalStr);
           // check od->scal must be in 1 2 4 8
           if(od->scal !=1 && od->scal !=2 && od->scal !=4 && od->scal !=8){
                printf("oprand %s is incorrect format ,scal must be in 1,2,4,8. now is %ld \n", str,od->scal);
                goto fail;
           }
        }

        if (type == EMPTY){
           printf("oprand %s is incorrect format , can not get oprand type \n", str);
           goto fail;
        }

        od->type = type;
    }

    return;

    fail:
     printf("can not parse oprand %s \n", str);
     exit(0);
    
}



const static struct 
{
    op_t op;
    const char *str;
}  OPERATOR_STR[] = {
    {INST_MOV,"mov"},
    {INST_MOV,"movq"},
    {INST_MOV,"movl"},
    {INST_PUSH,"push"},
    {INST_POP,"pop"},
    {INST_LEAVE,"leave"},
    {INST_CALL,"call"},
    {INST_CALL,"callq"},
    {INST_RET,"ret"},
    {INST_RET,"retq"},
    {INST_ADD,"add"},
    {INST_SUB,"sub"},
    {INST_CMP,"cmp"},
    {INST_CMP,"cmpq"},
    {INST_JNE,"jne"},
    {INST_JMP,"jmp"}
};

op_t str2op_t(const char *str)
{
     for (int j = 0;  j < sizeof (OPERATOR_STR) / sizeof (OPERATOR_STR[0]);  ++j){
         if (!strcmp (str, OPERATOR_STR[j].str))
             return OPERATOR_STR[j].op;    
     }
     printf("can not find opration for %s \n" , str);
     exit(0);
}


// const static struct {
//     od_type_t      type;
//     const char *str;
// } od_type_conversion [] = {
//     {MEM_IMM, "MEM_IMM"},
//     {MEM_IMM_REG1, "MEM_IMM_REG1"},
//     {MEM_IMM_REG1_REG2, "MEM_IMM_REG1_REG2"},
//     {MEM_IMM_REG2_SCAL, "MEM_IMM_REG2_SCAL"},
//     {MEM_IMM_REG1_REG2_SCAL, "MEM_IMM_REG1_REG2_SCAL"},
//     {MEM_REG1, "MEM_REG1"},
//     {MEM_REG1_REG2, "MEM_REG1_REG2"},
//     {MEM_REG2_SCAL, "MEM_REG2_SCAL"},
//     {MEM_REG1_REG2_SCAL, "MEM_REG1_REG2_SCAL"}
// };

// od_type_t str2odType (const char *str)
// {
//      int j;
//      for (j = 0;  j < sizeof (od_type_conversion) / sizeof (od_type_conversion[0]);  ++j)
//          if (!strcmp (str, od_type_conversion[j].str))
//              return od_type_conversion[j].type;    
//      error_message ("no such string");
// }

/*======================================*/
/*      instruction handlers            */
/*======================================*/

// insturction (sub)set
// In this simulator, the instructions have been decoded and fetched
// so there will be no page fault during fetching
// otherwise the instructions must handle the page fault (swap in from disk) first
// and then re-fetch the instruction and do decoding
// and finally re-run the instruction

static void mov_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void push_handler            (od_t *src_od, od_t *dst_od, core_t *cr);
static void pop_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void leave_handler           (od_t *src_od, od_t *dst_od, core_t *cr);
static void call_handler            (od_t *src_od, od_t *dst_od, core_t *cr);
static void ret_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void add_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void sub_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void cmp_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void jne_handler             (od_t *src_od, od_t *dst_od, core_t *cr);
static void jmp_handler             (od_t *src_od, od_t *dst_od, core_t *cr);

// handler table storing the handlers to different instruction types
typedef void (*handler_t)(od_t *, od_t *, core_t *);
// look-up table of pointers to function
static handler_t handler_table[NUM_INSTRTYPE] = {
    &mov_handler,               // 0
    &push_handler,              // 1
    &pop_handler,               // 2
    &leave_handler,             // 3
    &call_handler,              // 4
    &ret_handler,               // 5
    &add_handler,               // 6
    &sub_handler,               // 7
    &cmp_handler,               // 8
    &jne_handler,               // 9
    &jmp_handler,               // 10
};

// reset the condition flags
// inline to reduce cost
inline void reset_cflags(core_t *cr)
{
    cr->cpu_flags.__cpu_flag_value=0;
}

// update the rip pointer to the next instruction sequentially
static inline void next_rip(core_t *cr)
{
    // we are handling the fixed-length of assembly string here
    // but their size can be variable as true X86 instructions
    // that's because the operands' sizes follow the specific encoding rule
    // the risc-v is a fixed length ISA
    cr->rip = cr->rip + sizeof(char) * MAX_INSTRUCTION_CHAR;
}

// instruction handlers

static void mov_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG && dst_od->type == REG)
    {
        // src: register
        // dst: register
        *(uint64_t *)dst = *(uint64_t *)src;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type == REG && dst_od->type >= MEM_IMM)
    {
        // src: register
        // dst: virtual address
        write64bits_dram(
            va2pa(dst, cr), 
            *(uint64_t *)src,
            cr
            );
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type >= MEM_IMM && dst_od->type == REG)
    {
        // src: virtual address
        // dst: register
        *(uint64_t *)dst = read64bits_dram(
            va2pa(src, cr), 
            cr);
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
    else if (src_od->type == IMM && dst_od->type == REG)
    {
        // src: immediate number (uint64_t bit map)
        // dst: register
        *(uint64_t *)dst = src;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void push_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG)
    {
        // src: register
        // dst: empty
        (cr->reg).rsp = (cr->reg).rsp - 8;
        write64bits_dram(
            va2pa((cr->reg).rsp, cr), 
            *(uint64_t *)src, 
            cr
            );
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void pop_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);
    
    if (src_od->type == REG)
    {
        // src: register
        // dst: empty
        uint64_t old_val = read64bits_dram(
            va2pa((cr->reg).rsp, cr),
            cr
            );
        (cr->reg).rsp = (cr->reg).rsp + 8;
        *(uint64_t *)src = old_val;
        next_rip(cr);
        reset_cflags(cr);
        return;
    }
}

static void leave_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
}

static void call_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    // src: immediate number: virtual address of target function starting
    // dst: empty
    // push the return value
    (cr->reg).rsp = (cr->reg).rsp - 8;
    write64bits_dram(
        va2pa((cr->reg).rsp, cr),
        cr->rip + sizeof(char) * MAX_INSTRUCTION_CHAR,
        cr
        );
    // jump to target function address
    cr->rip = src;
    reset_cflags(cr);
}

static void ret_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    // uint64_t src = decode_operand(src_od);
    // uint64_t dst = decode_operand(dst_od);

    // src: empty
    // dst: empty
    // pop rsp
    uint64_t ret_addr = read64bits_dram(
        va2pa((cr->reg).rsp, cr),
        cr
        );
    (cr->reg).rsp = (cr->reg).rsp + 8;
    // jump to return address
    cr->rip = ret_addr;
    reset_cflags(cr);
}

static void add_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    uint64_t src = decode_operand(src_od);
    uint64_t dst = decode_operand(dst_od);

    if (src_od->type == REG && dst_od->type == REG)
    {
        // src: register (value: int64_t bit map)
        // dst: register (value: int64_t bit map)
        uint64_t res = *(uint64_t *)dst + *(uint64_t *)src;

        // set condition flags
        //进位是10进制的进位？
        cr->cpu_flags.CF= 
        //是否溢出的检测
        cr->cpu_flags.OF= res < *(uint64_t *)dst;    //overflow flag
        cr->cpu_flags.SF= (res>>63);    //sign flag,highest bit=0
        cr->cpu_flags.ZF= (res==0) ;    //zero flag
        // update registers
        *(uint64_t *)dst = res;
        // signed and unsigned value follow the same addition. e.g.
        // 5 = 0000000000000101, 3 = 0000000000000011, -3 = 1111111111111101, 5 + (-3) = 0000000000000010
        next_rip(cr);
        return;
    }
}

static void sub_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    //实现减法指令,e.g.
    //sub    $0x10,%rsp
    //uint64_t res = *(uint64_t *)dst_od->reg1 - src_od->imm;
    uint64_t prev = *(uint64_t *)dst_od->reg1;
    uint64_t prev2 = src_od->imm;
    uint64_t res = prev + (~prev2+1); //等同减法
    *(uint64_t *)dst_od->reg1 = res;
  
    //实现 conditon flag的设置
    cr->cpu_flags.CF= 0;    //carry flag
    //是否溢出的检测
    cr->cpu_flags.OF= res < prev;    //overflow flag  a<a+b
    cr->cpu_flags.SF= (res>>63);    //sign flag,highest bit=0
    cr->cpu_flags.ZF= (res==0) ;    //zero flag
    next_rip(cr);
}

static void cmp_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    printf(" todo cmp \n");
}

static void jne_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    printf(" todo jne \n");
}

static void jmp_handler(od_t *src_od, od_t *dst_od, core_t *cr)
{
    printf(" todo jmp \n");
}


// lookup table
static const char *reg_name_list[72] = {
    "%rax","%eax","%ax","%ah","%al",
    "%rbx","%ebx","%bx","%bh","%bl",
    "%rcx","%ecx","%cx","%ch","%cl",
    "%rdx","%edx","%dx","%dh","%dl",
    "%rsi","%esi","%si","%sih","%sil",
    "%rdi","%edi","%di","%dih","%dil",
    "%rbp","%ebp","%bp","%bph","%bpl",
    "%rsp","%esp","%sp","%sph","%spl",
    "%r8","%r8d","%r8w","%r8b",
    "%r9","%r9d","%r9w","%r9b",
    "%r10","%r10d","%r10w","%r10b",
    "%r11","%r11d","%r11w","%r11b",
    "%r12","%r12d","%r12w","%r12b",
    "%r13","%r13d","%r13w","%r13b",
    "%r14","%r14d","%r14w","%r14b",
    "%r15","%r15d","%r15w","%r15b",
};
static uint64_t reflect_register(const char *str, core_t *cr)
{    
    // lookup table
    reg_t *reg = &(cr->reg);
    uint64_t reg_addr[72] = {
        (uint64_t)&(reg->rax),(uint64_t)&(reg->eax),(uint64_t)&(reg->ax),(uint64_t)&(reg->ah),(uint64_t)&(reg->al),
        (uint64_t)&(reg->rbx),(uint64_t)&(reg->ebx),(uint64_t)&(reg->bx),(uint64_t)&(reg->bh),(uint64_t)&(reg->bl),
        (uint64_t)&(reg->rcx),(uint64_t)&(reg->ecx),(uint64_t)&(reg->cx),(uint64_t)&(reg->ch),(uint64_t)&(reg->cl),
        (uint64_t)&(reg->rdx),(uint64_t)&(reg->edx),(uint64_t)&(reg->dx),(uint64_t)&(reg->dh),(uint64_t)&(reg->dl),
        (uint64_t)&(reg->rsi),(uint64_t)&(reg->esi),(uint64_t)&(reg->si),(uint64_t)&(reg->sih),(uint64_t)&(reg->sil),
        (uint64_t)&(reg->rdi),(uint64_t)&(reg->edi),(uint64_t)&(reg->di),(uint64_t)&(reg->dih),(uint64_t)&(reg->dil),
        (uint64_t)&(reg->rbp),(uint64_t)&(reg->ebp),(uint64_t)&(reg->bp),(uint64_t)&(reg->bph),(uint64_t)&(reg->bpl),
        (uint64_t)&(reg->rsp),(uint64_t)&(reg->esp),(uint64_t)&(reg->sp),(uint64_t)&(reg->sph),(uint64_t)&(reg->spl),
        (uint64_t)&(reg->r8),(uint64_t)&(reg->r8d),(uint64_t)&(reg->r8w),(uint64_t)&(reg->r8b),
        (uint64_t)&(reg->r9),(uint64_t)&(reg->r9d),(uint64_t)&(reg->r9w),(uint64_t)&(reg->r9b),
        (uint64_t)&(reg->r10),(uint64_t)&(reg->r10d),(uint64_t)&(reg->r10w),(uint64_t)&(reg->r10b),
        (uint64_t)&(reg->r11),(uint64_t)&(reg->r11d),(uint64_t)&(reg->r11w),(uint64_t)&(reg->r11b),
        (uint64_t)&(reg->r12),(uint64_t)&(reg->r12d),(uint64_t)&(reg->r12w),(uint64_t)&(reg->r12b),
        (uint64_t)&(reg->r13),(uint64_t)&(reg->r13d),(uint64_t)&(reg->r13w),(uint64_t)&(reg->r13b),
        (uint64_t)&(reg->r14),(uint64_t)&(reg->r14d),(uint64_t)&(reg->r14w),(uint64_t)&(reg->r14b),
        (uint64_t)&(reg->r15),(uint64_t)&(reg->r15d),(uint64_t)&(reg->r15w),(uint64_t)&(reg->r15b),
    };
    for (int i = 0; i < 72; ++ i)
    {
        if (strcmp(str, reg_name_list[i]) == 0)
        {
            // now we know that i is the index inside reg_name_list
            return reg_addr[i];
        }
    }
    printf("parse register %s error\n", str);
    exit(0);
}


// instruction cycle is implemented in CPU
// the only exposed interface outside CPU
void instruction_cycle(core_t *cr)
{
    // FETCH: get the instruction string by program counter
    //const char *inst_str = (const char *)cr->rip;
    char inst_str[MAX_INSTRUCTION_CHAR] ;
    readinst_dram(va2pa(cr->rip ,cr),inst_str,cr);
    debug_printf(DEBUG_INSTRUCTIONCYCLE, "%lx    %s\n", cr->rip, inst_str);

    // DECODE: decode the run-time instruction operands
    inst_t inst;
    parse_instruction(inst_str, &inst, cr);
    
    // EXECUTE: get the function pointer or handler by the operator
    handler_t handler = handler_table[inst.op];
    // update CPU and memory according the instruction
    handler(&(inst.src), &(inst.dst), cr);
}

void print_register(core_t *cr)
{
    if ((DEBUG_VERBOSE_SET & DEBUG_REGISTERS) == 0x0)
    {
        return;
    }

    reg_t reg = cr->reg;
    
    printf("rax = %16lx\trbx = %16lx\trcx = %16lx\trdx = %16lx\n",
        reg.rax, reg.rbx, reg.rcx, reg.rdx);
    printf("rsi = %16lx\trdi = %16lx\trbp = %16lx\trsp = %16lx\n",
        reg.rsi, reg.rdi, reg.rbp, reg.rsp);
    printf("rip = %16lx\n", cr->rip);
    printf("CF = %u\tZF = %u\tSF = %u\tOF = %u\n",
        cr->cpu_flags.CF, cr->cpu_flags.ZF, cr->cpu_flags.SF, cr->cpu_flags.OF);
}

void print_stack(core_t *cr)
{
    if ((DEBUG_VERBOSE_SET & DEBUG_PRINTSTACK) == 0x0)
    {
        return;
    }

    int n = 10;    
    uint64_t *high = (uint64_t*)&pm[va2pa((cr->reg).rsp, cr)];
    high = &high[n];
    uint64_t va = (cr->reg).rsp + n * 8;
    
    for (int i = 0; i < 2 * n; ++ i)
    {
        uint64_t *ptr = (uint64_t *)(high - i);
        printf("0x%16lx : %16lx", va, (uint64_t)*ptr);

        if (i == n)
        {
            printf(" <== rsp");
        }
        printf("\n");
        va -= 8;
    }
}


void TestParsingOperand()
{
    ACTIVE_CORE = 0x0;
    
    core_t *ac = (core_t *)&cores[ACTIVE_CORE];

    // init state
    ac->reg.rax = 0xabcd;
    ac->reg.rbx = 0x8000670;
    ac->reg.rcx = 0x8000670;
    ac->reg.rdx = 0x12340000;
    ac->reg.rsi = 0x7ffffffee208;
    ac->reg.rdi = 0x1;
    ac->reg.rbp = 0x7ffffffee110;
    ac->reg.rsp = 0x7ffffffee0f0;

    reset_cflags(ac);

    write64bits_dram(va2pa(0x7ffffffee110, ac), 0x0000000000000000, ac);    // rbp
    write64bits_dram(va2pa(0x7ffffffee108, ac), 0x0000000000000000, ac);
    write64bits_dram(va2pa(0x7ffffffee100, ac), 0x0000000012340000, ac);
    write64bits_dram(va2pa(0x7ffffffee0f8, ac), 0x000000000000abcd, ac);
    write64bits_dram(va2pa(0x7ffffffee0f0, ac), 0x0000000000000000, ac);    // rsp
    
    const char *strs[11] = {
        "$0x1234",
        "%rax",
        "0xabcd",
        "(%rsp)",
        "0xabcd(%rsp)",
        "(%rsp,%rbx)",
        "0xabcd(%rsp,%rbx)",
        "(,%rbx,8)",
        "0xabcd(,%rbx,8)",
        "(%rsp,%rbx,8)",
        "0xabcd(%rsp,%rbx,8)",
    };
    
    printf("rax %p\n", &(ac->reg.rax));
    printf("rsp %p\n", &(ac->reg.rsp));
    printf("rbx %p\n", &(ac->reg.rbx));
    
    for (int i = 0; i < 11; ++ i)
    {
        od_t od;
        //printf("od %p \n", &od);
        parse_operand(strs[i], &od, ac);
        printf("\n%s\n", strs[i]);
        printf("od enum type: %d\n", od.type);
        printf("od imm: %lx\n", od.imm);
        printf("od reg1: %lx\n", od.reg1);
        printf("od reg2: %lx\n", od.reg2);
        printf("od scal: %lx\n", od.scal);
    }
}