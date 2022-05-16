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
    uint64_t sh_addr; // 0x0
    uint64_t sh_offset; //lineoffset
    uint64_t sh_size;  //line count start from lineoffset
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

/*======================================*/
/*      relocation information          */
/*======================================*/

typedef enum
{
    R_X86_64_32,
    R_X86_64_PC32,
    R_X86_64_PLT32,
} reltype_t;

// relocation entry type
typedef struct
{
    /*  this is what's different in our implementation. instead of byte offset, 
        we use line index + char offset to locate the symbol */
    uint64_t    r_row;      // line index of the symbol in buffer section
                            // for .rel.text, that's the line index in .text section
                            // for .rel.data, that's the line index in .data section
    uint64_t    r_col;      // char offset in the buffer line
    reltype_t   type;       // relocation type
    uint32_t    sym;        // symbol table index
    int64_t     r_addend;   // constant part of relocation expression
} rl_entry_t;



typedef struct 
{
    char code[MAX_ELF_FILE_LINES][MAX_PERLINE_LENGTH_IN_ELF_FILE];
    uint64_t lineCount;
    uint64_t sh_entry_count;
    sh_entry_t *sht;

    uint64_t symt_count;
    st_entry_t *symt;

    uint64_t reltext_count;
    rl_entry_t *reltext;

    uint64_t reldata_count;
    rl_entry_t *reldata;

}elf_t;

void debug_print_sht(elf_t *elf);
void debug_print_elf(elf_t *elf);
void debug_print_syt(elf_t *elf);

void parse_elf(char *filename, elf_t *elf);
int read_elf(char *file, uint64_t addr);

void process_sh(char *sh, sh_entry_t *sh_e);
int process_entry(char *sh, char ***cols);

void free_elf(elf_t *elf);
void free_table_entry(char **ent, int n);

void link_elf(elf_t src[],int src_size,elf_t *dst);
#endif