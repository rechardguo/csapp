#ifndef SRAM_GUARD
#define SRAM_GUARD

#include <stdint.h>


#define SRAM_CACHE_TAG_LENGTH (40)
#define SRAM_CACHE_INDEX_LENGTH (6)
#define SRAM_CACHE_OFFSET_LENGTH (6)

// paddr
// 1. CT 2. CI 3. CO
typedef union 
{
    uint64_t paddr_value;
    union{
        uint64_t CO:SRAM_CACHE_OFFSET_LENGTH;
        uint64_t CI:SRAM_CACHE_INDEX_LENGTH;
        uint64_t CT:SRAM_CACHE_TAG_LENGTH;
    };
}address_t;


uint64_t read_sram(uint64_t paddr_value);
void write_sram(uint64_t paddr_value,uint64_t data);

#endif