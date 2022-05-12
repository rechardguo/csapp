#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include "headers/linker.h"

int main()
{
   // elf_t *elf = malloc(sizeof(elf_t));
    elf_t src[2];
    parse_elf("./files/exe/sum.elf.txt",&src[0]);
    printf("-----------------------------\n");
    parse_elf("./files/exe/main.elf.txt",&src[1]);
    
    elf_t dst;
    link_elf(src,2,&dst); 

    free_elf(&src[0]);
    free_elf(&src[1]);
    return 0;
}