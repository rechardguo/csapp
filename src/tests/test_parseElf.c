#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "headers/linker.h"

int main()
{
    // char buf[100];
    // char *path;
    // path = getcwd(buf,100);
    // printf("current dir=%s \n",path);
    elf_t src[2];

    //char *elfFile1 = strcat(path ,"/files/exe/sum.elf.txt");
    //printf("elf file1 path=%s \n",elfFile1);
    parse_elf("./files/exe/sum.elf.txt" , &src[0]);
    
    printf("-----------------------------\n");
    
   // path = getcwd(buf,100);
   // char *elfFile2 = strcat(path ,"/files/exe/main.elf.txt");
   // printf("elf file2 path=%s \n",elfFile2);
    parse_elf("./files/exe/main.elf.txt",&src[1]);
    
    elf_t dst;
    link_elf(src,2,&dst); 

    free_elf(&src[0]);
    free_elf(&src[1]);
    free_elf(&dst);
    return 0;
}