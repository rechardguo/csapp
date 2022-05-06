#ifndef LINKER_GUARD
#define LINKER_GUARD

#include <stdint.h>
#include <stdlib.h>

#define MAX_ELF_FILE_LINES (120)
#define MAX_PERLINE_LENGTH_IN_ELF_FILE (80)
#define MAX_SH_NAME_LENGTH (10)

#define MAX_CHAR_SECTION_NAME (50)
//section header table
typedef struct 
{
    char sh_name[MAX_SH_NAME_LENGTH];    //.text  or  .data or .symbol
    uint16_t sh_addr; // 0x0
    uint16_t sh_offset; //lineoffset
    uint16_t sh_size;  //line count start from lineoffset
}sh_entry_t;

typedef enum
{
    STB_LOCAL,
    STB_GLOBAL,
    STB_WEAK
} st_bind_t;

typedef enum
{
    STT_NOTYPE,
    STT_OBJECT,
    STT_FUNC
} st_type_t;

typedef struct linker
{
    char st_name[MAX_CHAR_SECTION_NAME];
    st_bind_t bind;
    st_type_t type;
    char st_shndx[MAX_CHAR_SECTION_NAME];
    uint64_t st_value;      // in-section offset
    uint64_t st_size;       // count of lines of symbol
} st_entry_t; // symbol table entry


typedef struct 
{
    char code[MAX_ELF_FILE_LINES][MAX_PERLINE_LENGTH_IN_ELF_FILE];
    uint16_t lineCount;
    uint16_t sh_entry_count;
    sh_entry_t *sht;

    uint64_t symt_count;
    st_entry_t *symt;

}elf_t;

int parse_elf(char *file, elf_t *elf);

#endif