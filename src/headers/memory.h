// include guards to prevent double declaration of any identifiers 
// such as types, enums and static variables
#ifndef MEMORY_GUARD
#define MEMORY_GUARD

#include<stdint.h>
#include<headers/cpu.h>

/*======================================*/
/*      physical memory on dram chips   */
/*======================================*/

// physical memory space is decided by the physical address
// in this simulator, there are 4 + 6 + 6 = 16 bit physical adderss
// then the physical space is (1 << 16) = 65536
// total 16 physical memory

// 1<<16 就是2的16次方
#define PHYSICAL_MEMORY_SPACE   65536
// 最大的index=15,也就是共16页 ,每页是4096bit ,换算成byte=4096/8=512byte
// 16 physical memory pages
#define MAX_INDEX_PHYSICAL_PAGE 15

// physical memory
// 物理地址共65536bit, index from  0-65535
uint8_t pm[PHYSICAL_MEMORY_SPACE];

/*======================================*/
/*      memory R/W                      */
/*======================================*/

// used by instructions: read or write uint64_t to DRAM
uint64_t read64bits_dram(uint64_t paddr, core_t *cr);
void write64bits_dram(uint64_t paddr, uint64_t data, core_t *cr);
//write inst to dram
void writeinst_dram(uint64_t paddr, const char *str, core_t *cr);
void readinst_dram(uint64_t paddr, char *buf, core_t *cr);
#endif