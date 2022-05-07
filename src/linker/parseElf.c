#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <headers/common.h>
#include <headers/linker.h>

/**
 * @brief 打印出elf sht
 * 
 * @param elf 
 */
void debug_print_sht(elf_t *elf){
     int entryCount = elf->sh_entry_count;
     for(int i=0;i<entryCount;i++){
        sh_entry_t *sh_e = &elf->sht[i];
        printf("sh_addr=%s\t sh_addr=%ld\t sh_offset=%ld\t sh_size=%ld\n",
        sh_e->sh_name,sh_e->sh_addr,
        sh_e->sh_offset,sh_e->sh_size);
     }
}

void debug_print_syt(elf_t *elf){
     int entryCount = elf->symt_count;
     for(int i=0;i<entryCount;i++){
        st_entry_t *st_e = &elf->symt[i];
        printf("st_name=%s\t bind=%d\t sh_offset=%d\t st_shndx=%s st_size=%ld\t st_value=%ld\t \n",
        st_e->st_name,st_e->bind,st_e->type,st_e->st_shndx,
        st_e->st_size,st_e->st_value);
     }
}
void debug_print_elf(elf_t *elf){
   for(int i=0;i<elf->lineCount;i++){
       printf("%s \n",elf->code[i]);
    }
}

//test_elf.c   
//header/linker.h   read_elf(char *file)  global, undefied, .text
//linker/parseElf.c
int read_elf(char *file, uint64_t addr){
   FILE *fp;

    fp = fopen(file,"r");
    assert( fp!=NULL );
    if(fp == NULL){
        printf("file %s does not exist \n" , file);
        exit(0);
    }

    int lineCount = 0 ;
    char line[MAX_PERLINE_LENGTH_IN_ELF_FILE];
    
    while( fgets(line, MAX_PERLINE_LENGTH_IN_ELF_FILE, fp) !=NULL ){ // NULL means eof or error, see man fgets
        // int len = strlen(line);
        // if( len==0||
        //    (len>=1  && (line[0]=='\r' || line[0]=='\n' || line[0]=='\t'))||
        //    (len>2 && line[0]=='/' && line[1]=='/')){
        //       continue;
        // }

        int len = strlen(line);
        if ((len == 0) || 
            (len >= 1 && (line[0] == '\n' || line[0] == '\r')) ||
            (len >= 2 && (line[0] == '/' && line[1] == '/')))
        {
            continue;
        }


        // check if is empty or white line
        //最后一位是'\0',需要排除掉，所以是len-1
        int iswhite = 1;
        for (int i = 0; i < len-1; ++ i)
        {
            iswhite = iswhite && (line[i] == ' ' || line[i] == '\t' || line[i] == '\r');
        }
        if (iswhite == 1)
        {
            continue;
        }

      
        // 加入到elf.code
        for (int j = 0; j < len; ++ j)
        {
              if( line[j] == '\r' || line[j] == '\n' || 
                 (j+1<len && line[j]=='/' && line[j+1]=='/' )){
                     break;
              }
              
               char* p= (char*)(addr + (lineCount*(MAX_PERLINE_LENGTH_IN_ELF_FILE)+ j)*sizeof(char)) ;
               *p= line[j];
        }
        lineCount++;

        if(lineCount > MAX_ELF_FILE_LINES)
        {
            debug_printf(DEBUG_LINKER, "elf file %s is too long (>%d)\n", file, MAX_ELF_FILE_LINES);
            fclose(fp);
            exit(1);
        }

    }
    fclose(fp);
    
    assert(string2uint((char *)addr) == lineCount);
    return lineCount; 
 
}

void process_sh(char *sh, sh_entry_t *sh_e){
     // .text,0x0,6,10
     //error: ‘cols’ is used uninitialized in this functio
     //在process_entry里初始化
     char **cols;
     int col_num = process_entry(sh, &cols);
     assert( col_num == 4);   
     //sh_e->sh_name = cols[0];
     //为什么这里要用strcpy
     strcpy(sh_e->sh_name, cols[0]);
     sh_e->sh_addr = string2uint(cols[1]);
     sh_e->sh_offset = string2uint(cols[2]);
     sh_e->sh_size = string2uint(cols[3]);
}


void process_symtab(char *sh, st_entry_t *st_e){
     // sum,STB_GLOBAL,STT_FUNC,.text,0,22 
     //在process_entry里初始化
     char **cols;
     int col_num = process_entry(sh, &cols);
     assert( col_num == 6);   
     strcpy(st_e->st_name, cols[0]);
     //strcpy(st_e->bind , cols[1]);
     //strcpy(st_e->type , cols[2]);
     
      // select symbol bind
    if (strcmp(cols[1], "STB_LOCAL") == 0)
    {
        st_e->bind = STB_LOCAL;
    }
    else if (strcmp(cols[1], "STB_GLOBAL") == 0)
    {
        st_e->bind = STB_GLOBAL;
    }
    else if (strcmp(cols[1], "STB_WEAK") == 0)
    {
        st_e->bind = STB_WEAK;
    }
    else
    {
        printf("symbol bind is neiter LOCAL, GLOBAL, nor WEAK\n");
        exit(0);
    }
    
    // select symbol type 
    if (strcmp(cols[2], "STT_NOTYPE") == 0)
    {
        st_e->type = STT_NOTYPE;
    }
    else if (strcmp(cols[2], "STT_OBJECT") == 0)
    {
        st_e->type = STT_OBJECT;
    }
    else if (strcmp(cols[2], "STT_FUNC") == 0)
    {
        st_e->type = STT_FUNC;
    }
    else
    {
        printf("symbol type is neiter NOTYPE, OBJECT, nor FUNC\n");
        exit(0);
    }

     strcpy(st_e->st_shndx , cols[3]);   
     st_e->st_size = string2uint(cols[4]);
     st_e->st_value = string2uint(cols[5]);
}

/**
 * @brief 
 * 
 * @param sh 
 * @param cols 
 * @return int : total cols number 
 */
int process_entry(char *sh, char ***cols){
    assert(sh!=NULL);
    int len= strlen(sh);
    int num_cols=1;
    for (int i = 0; i < len; i++)
    {
        if(sh[i] == ','){
            num_cols++;
        }
    }

    //分配一个string数组吗？
   // char **arr= malloc(num_cols*sizeof(char));
    char **arr = malloc(num_cols * sizeof(char *));
    *cols = arr;
    
    char col[32];
    int colIndex = 0;
    int colWidth = 0;
    //strlen不包含\0,所以这里<=len
    for (int i = 0; i <= len; i++){
        //i==len 也就是到了\0和','一起处理即可
        if(sh[i] == ',' || i==len ){
            assert(colIndex < num_cols);
            //直接这样写的话会变，col只是一个地址指针，所指向的内容会变化，所以需要再次进行拷贝】
            //arr[colIndex] = col;
            char *txt =malloc( (colWidth+1)*sizeof(char) );
            for( int j=0;j<colWidth;j++){
                 txt[j] = col[j];   
            }
            txt[colWidth] ='\0';
            arr[colIndex] = txt;
            colIndex++;
            colWidth=0;
            if(i==len-1)
              printf("i==len-1 , %s \n" ,txt);
        }else{
            //不能超过32个字符
            assert(colWidth<32);
            col[colWidth] = sh[i];
            colWidth++;
        }
    }

    return colIndex;
}

//从文件里加载
void parse_elf(char *filename, elf_t *elf){
    uint64_t addr = (uint64_t)elf->code;
    int lineCount = read_elf(filename,addr);
    elf->lineCount = lineCount;

    //process section header
    uint64_t st_count= string2uint(elf->code[1]);
    assert(st_count>0);
    elf->sh_entry_count = st_count;

    sh_entry_t *sh_e = malloc(st_count * sizeof(sh_entry_t));

    elf->sht =sh_e;
    sh_entry_t *sym_sh_e = NULL;  
    for(int i=0;i<st_count;i++){
        process_sh(elf->code[i+2],&sh_e[i]);
        
        if( strcmp(sh_e[i].sh_name,".symtab") == 0 ){
            sym_sh_e=&sh_e[i];
        }
    }

    assert( sym_sh_e!=NULL );
    //process symtab
    int sym_count  = sym_sh_e->sh_size;
    assert(sym_count>0);
    st_entry_t *st_e = malloc( sym_count * sizeof(st_entry_t) );
    elf->symt_count = sym_count;
    elf->symt = st_e;

    for(int i=0;i<sym_count;i++){
       process_symtab(elf->code[sym_sh_e->sh_offset+i],&st_e[i]);
    }
    


}

