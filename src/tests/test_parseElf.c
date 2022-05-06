#include <stdlib.h>
#include <stdio.h>
#include <headers/linker.h>
#include <assert.h>


int main()
{
    elf_t *elf = malloc(sizeof(elf_t));
    int lineCount = parse_elf("./files/main.elf.txt",elf);
    //为什么这样是50,为什么需要使用string2uint来转呢？
    char lineOne = *(char*)(elf->code[0]);
    printf("%d ,%s,%ld \n",lineCount,&lineOne,sizeof(int8_t *));
   // assert(lineCount == lineOne);
    for(int i=0;i<lineCount;i++){
       printf("%s \n",elf->code[i]);
    }
    return 0;
}