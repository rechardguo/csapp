#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include "headers/linker.h"

int main()
{
   // elf_t *elf = malloc(sizeof(elf_t));
    elf_t elf;
    parse_elf("./files/exe/sum.elf.txt",&elf);
    debug_print_elf(&elf);
    printf("-----------------------\n");
    debug_print_sht(&elf);
    printf("-----------------------\n");
    debug_print_syt(&elf);

    free_elf(&elf);
    return 0;
}