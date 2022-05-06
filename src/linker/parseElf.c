#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <headers/common.h>
#include <headers/linker.h>

//从文件里加载
int parse_elf(char *file, elf_t *elf){
    FILE *fp;

    fp = fopen(file,"r");
    if( fp == NULL){
        //TODO
        printf("file %s does not exist \n" , file);
        exit(-1);
    }

    int lineCount = 0 ;
    char line[MAX_PERLINE_LENGTH_IN_ELF_FILE];
    char *addr=(char*)(elf->code);
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