﻿#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <headers/common.h>
#include <headers/linker.h>
#define MAX_SYMBOL_MAP_LENGTH 64
// internal mapping between source and destination symbol entries
typedef struct
{
    elf_t       *src_elf;   // src elf file
    st_entry_t  *src;   // src symbol
    st_entry_t  *dst;   // dst symbol: used for relocation - find the function referencing the undefined symbol
    // TODO:
    // relocation entry (referencing section, referenced symbol) converted to (referencing symbol, referenced symbol) entry
} smap_t;

// static st_bind_t symbol_bind( int bind ){
//     switch (bind)
//     {
//     case 0:
//        return STB_LOCAL;
//         break;
//     case 1:
//        return STB_GLOBAL;
//         break;
//     case 2:
//        return STB_WEAK;
//         break;    
//     default:
//         //todo print error
//         printf(" can not parse symbol bind = %d \n",bind);
//         exit(0);
//         break;
//     }
// }
// static st_type_t symbol_type( int type ){
//     switch (type)
//     {
//     case 0:
//         return STT_NOTYPE;
//         break;
//      case 1:
//         return STT_OBJECT;
//         break;
//      case 2:
//         return STT_FUNC;
//         break;   
//     default:
//          //todo print error
//         printf(" can not parse symbol type = %d \n",type);
//         exit(0);
//         break;
//     }

// }


/**
 * @brief 
 * 
 * bind      type      ndx                        precedence
 * ---------------------------------------------------------
 * global    notype    und                           0
 * global    object    com                           1
 * global    fun       .text                         2
 * global    object    .data or .bss or .rodata      2 
 * @param sym 
 * @return int 0 or 1 or 2
 */
static int symbol_precedence(st_entry_t *sym){

  assert(sym->bind == STB_GLOBAL);  
  //st_type_t st = symbol_type();  
  if( sym->type == STT_NOTYPE ){
      return 0;
  } else if ( sym->type == STT_OBJECT && strcmp( sym->st_shndx, "COMMON") ==0) {
      return 1;
  }else{
      return 2;
  }
}
/**
 * @brief 
 * 
 * rule
 * --------------------------------
 * s    s    x
 * w    s    s
 * w1  w2    w1
 * @param smap 
 * @param elf 
 * @param sym 
 */
static void symbol_resolution(smap_t *smap, elf_t *elf ,st_entry_t *sym){

    assert( smap!=NULL );
    assert( elf!=NULL );
    assert( sym!=NULL );
    assert( sym->bind == STB_GLOBAL) ;
    st_entry_t *exist_sym = smap->src;
    assert ( strcmp(sym->st_name,exist_sym->st_name)==0 );
    assert(  exist_sym->bind == STB_GLOBAL );
    
    int pre1 = symbol_precedence(sym);
    int pre2 = symbol_precedence(exist_sym);
    
    //rule 1 not allowed
    if(pre1 == 2 && pre2 ==2 ){
        printf(" both symbol [%s ,%s] strong not allowed !! \n",sym->st_name,exist_sym->st_name);
        exit(0);
    }else if( pre1 > pre2 ){

        assert(smap->src_elf!=elf);
        smap->src_elf = elf;
        smap->src = sym;
    }

}

static sh_entry_t* get_sh(sh_entry_t *addr,int sh_count ,char *sh_name){
    for(int i=0 ;i<sh_count;i++){
        if( strcmp(addr[i].sh_name,sh_name) ==0 ){
                return &addr[i];
        }
    }
    return NULL;
}

static void process_sh_entry(sh_entry_t *she, elf_t *dst, int sht_index,
                   char* name, uint64_t addr, int session_offset,int size){

      assert(sht_index<dst->sh_entry_count);

      strcpy(she->sh_name,name);
      she->sh_addr = addr;
      she->sh_size = size;
      she->sh_offset = session_offset ; // .text offset
      
      //protol  line=1  she_line=1  sht_index= session header table index
      sprintf(dst->code[1+1+sht_index],"%s,0x%lx,%ld,%ld",
         she->sh_name,
         she->sh_addr,
         she->sh_offset,
         she->sh_size);

}

static void compute_section_header(smap_t *smapArr,int smapArrCount, elf_t srcs[],int srcs_size,elf_t *dst){
   int txtCount=0;
   int dataCount=0;
   int rodataCount=0;
   for(int i=0;i<smapArrCount;i++){
        smap_t smap = smapArr[i];
        st_entry_t *sym = smap.src;
        if (strcmp(sym->st_shndx,".text")==0){
            txtCount += sym->st_size;
        }
        if (strcmp(sym->st_shndx, ".rodata") == 0){
            // .rodata section symbol
            rodataCount += sym->st_size;
        }
        if (strcmp(sym->st_shndx,".data")==0){
            dataCount += sym->st_size;
        }
    }

    //txtCount>0 txtCount!=0为true true =1
    //.text .rodata .data .symtab
    int shtCount=(txtCount!=0) + (rodataCount!=0) +(dataCount!=0) + (smapArrCount!=0);
    int elfLineCount=1 + 1 + shtCount + txtCount + rodataCount + dataCount + smapArrCount;

    dst->lineCount = elfLineCount;
    dst->sh_entry_count = shtCount;
    dst->symt_count = smapArrCount;

    // print to buffer
    sprintf(dst->code[0], "%ld", dst->lineCount);
    sprintf(dst->code[1], "%ld", dst->sh_entry_count);

    //todo free this mm
    sh_entry_t *sh_e = malloc( dst->sh_entry_count*sizeof(sh_entry_t));
    dst->sht = sh_e;
    
    // compute the run-time address of the sections: compact in memory
    uint64_t text_runtime_addr = 0x00400000;
    uint64_t rodata_runtime_addr = text_runtime_addr + txtCount * MAX_INSTRUCTION_CHAR * sizeof(char);
    uint64_t data_runtime_addr = rodata_runtime_addr + dataCount * sizeof(uint64_t);
    uint64_t symtab_runtime_addr = 0; // For EOF, .symtab is not loaded into run-time memory but still on disk
    
    int sht_index = 0;
    //start from 0, so (1 + 1 + dst->sh_entry_count)-1
    int session_offset = (1 + 1 + dst->sh_entry_count);
    //process elf.sht.text  
    if(txtCount>0){
        sh_entry_t *she = &(dst->sht[sht_index]);
        process_sh_entry(she, dst, sht_index,
                   ".text", text_runtime_addr,session_offset,txtCount);
       sht_index++;   
       session_offset += she->sh_size;
    }

    //process elf.sht.rodata
    if(rodataCount>0){
      sh_entry_t *she = &(dst->sht[sht_index]);
        process_sh_entry(she, dst, sht_index,
                   ".rodataCount", rodata_runtime_addr,session_offset,rodataCount);
       sht_index++;   
       session_offset += she->sh_size;
    }

    //process elf.sht.data
    if(dataCount>0){
     sh_entry_t *she = &(dst->sht[sht_index]);
        process_sh_entry(she, dst, sht_index,
                   ".data", data_runtime_addr,session_offset,dataCount);
       sht_index++;   
       session_offset += she->sh_size;  
    }

    //process elf.sht.symtab
    if(smapArrCount>0){
        sh_entry_t *she = &(dst->sht[sht_index]);
        process_sh_entry(she, dst, sht_index,
                   ".symtab", symtab_runtime_addr,session_offset,smapArrCount);
       sht_index++;   
       session_offset += she->sh_size; 
    }

    //process elf.symtab
    //st_entry_t *st_e = malloc( dst->sh_entry_count*sizeof(st_entry_t));
    //dst->symt = st_e;
    assert(sht_index  == dst->sh_entry_count);

    // print and check
    if ((DEBUG_VERBOSE_SET & DEBUG_LINKER) != 0)
    {
        printf("----------------------\n");
        debug_print_sht(dst);
        //debug_print_syt(dst);
    }

}

static void process_symbol(smap_t *smapArr,int *smapArrCount, elf_t *srcs, int srcs_size){
    
    smap_t *smap = NULL;
    //int symmap_index=0;
    for( int i=0;i<srcs_size;i++ ){
        elf_t *src = &srcs[i];
        int  sym_count =src->symt_count;
        st_entry_t *ste;
        // symbol process
        for(int j=0;j<sym_count;j++){
            smap = NULL;
            ste = &(src->symt[j]);
            if( ste->bind == STB_LOCAL ){
                 //add directly
                assert (*smapArrCount<MAX_SYMBOL_MAP_LENGTH);
                //todo: free memory
                smap = malloc(sizeof(smap_t));
                smap->src_elf = src;
                smap->src = ste;
                smapArr[*smapArrCount] = *smap; 
                (*smapArrCount)++;
                continue;
            }

            //here to parse symbol = STB_GLOBAL
            //to check if same name sym exist in symmap
            for( int z=0;z<*smapArrCount;z++ ){

                if( strcmp(smapArr[z].src->st_name,ste->st_name) == 0 ){
                    smap = &smapArr[z];
                    break;
                }
            }
            if(smap == NULL){
                assert (*smapArrCount<MAX_SYMBOL_MAP_LENGTH);
                smap = malloc(sizeof(smap_t));
                smap->src_elf = src;
                smap->src = ste;
                smapArr[*smapArrCount] = *smap; 
                (*smapArrCount)++;
            }else{
                symbol_resolution(smap, src, ste);
            }
           
        }
    }

    for( int i=0;i<*smapArrCount;i++ ){

        st_entry_t  *s = smapArr[i].src;
        //STT_NOTYPE , SHN_UNDEF
        assert( strcmp(s->st_shndx,"SHN_UNDEF")!=0);
        assert( s->type!=STT_NOTYPE );

        // the remaining COMMON go to .bss
        if (strcmp(s->st_shndx, "COMMON") == 0)
        {
            char *bss = ".bss";
            for (int j = 0; j < MAX_CHAR_SECTION_NAME; ++ j)
            {
                if (j < 4)
                {
                    s->st_shndx[j] = bss[j];
                }
                else
                {
                    s->st_shndx[j] = '\0';
                }
            }
            s->st_value = 0;
        }

    }

   
    printf("----------------------\n");

    for (int i = 0; i < *smapArrCount; ++ i)
    {
        st_entry_t *ste = smapArr[i].src;
        debug_printf(DEBUG_LINKER, "%s\t%d\t%d\t%s\t%d\t%d\n",
            ste->st_name,
            ste->bind,
            ste->type,
            ste->st_shndx,
            ste->st_value,
            ste->st_size);
    }


}

static void merge_section(elf_t *srcs, int num_srcs, elf_t *dst,
    smap_t *smap_table, int smap_count){

    st_entry_t * ste = malloc(sizeof(st_entry_t) * dst->symt_count);    
    dst->symt = ste;
    
    //.txt
    sh_entry_t *dst_txt_sh_e = get_sh(dst->sht,dst->sh_entry_count,".text");
    assert(strcmp(dst_txt_sh_e->sh_name, ".text")==0);
    int dst_txt_offset =dst_txt_sh_e->sh_offset;

    for(int i=0;i<smap_count;i++){
        smap_t smap = smap_table[i];
        st_entry_t *sym = smap.src;
        
        if( strcmp(sym->st_shndx,".text") == 0 ){
            sh_entry_t *src_txt_sh_e = get_sh(smap.src_elf->sht,smap.src_elf->sh_entry_count,".text");
            int src_txt_offset = src_txt_sh_e->sh_offset;
            int src_size = src_txt_sh_e->sh_size;

            for(int j=0;j<src_size;j++){
                strcpy(dst->code[dst_txt_offset] ,smap.src_elf->code[src_txt_offset]);
                dst_txt_offset++;
                src_txt_offset++;
            }
        }
        
    }

    
    debug_print_elf(dst);
    //.rodata
    //.data
    //.symtab    
}


void link_elf(elf_t srcs[],int srcs_size,elf_t *dst){

    assert(srcs_size>0);
    smap_t smapArr[MAX_SYMBOL_MAP_LENGTH];
    int smapArr_count=0;
    process_symbol(smapArr,&smapArr_count, srcs,srcs_size);
    
    //process below done
    //all line count
    //sht line count
    //.sht
    compute_section_header(smapArr,smapArr_count,srcs,srcs_size,dst);    

    //.txt
    //.rodata
    //.data
    //.symtab   
    merge_section(srcs,srcs_size,dst,smapArr,smapArr_count);
   

}