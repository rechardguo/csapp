#include <stdint.h>
#include <headers/address.h>


//paddr

// sram 是 cpu cache 
//1 BLOCK=8byte
#define BLOCK_BIT_LENGTH (6)
#define BLOCK_BYTE_COUNT (1<<BLOCK_BIT_LENGTH)
#define CACHE_SET_COUNT (6)
#define CACHE_LINE_COUNT (6)

typedef enum
{
  INVALID,
  VALID,
  DIRTY  
}cache_line_status_t;

typedef struct
{
    //time for lru
    uint64_t time;
    // status
    cache_line_status_t status;
    // tag
    uint64_t tag;
    // block
    uint8_t block[BLOCK_BYTE_COUNT];
} cache_line_t;

typedef struct 
{
    cache_line_t line[CACHE_LINE_COUNT];
}cache_set_t;


typedef struct 
{
   cache_set_t set[CACHE_SET_COUNT];
}sram_t;

static sram_t sram;

uint64_t bus_read(uint64_t paddr);
void bus_write(uint64_t paddr,uint64_t data);

/**
 * @brief 
 * 读cache
 * 1.如果cache_line的tag是相同的,只要状态是非invalid就直接读返回
 * 2.如果没有找到1,则如果有状态是 invalid的cacheline就读取dram到sram后返回
 * 3.如果2也没有，则找出victim,如果victim状态是dirty就要写回去。
 * 4.读取dram到sram后返回
 * 
 * @param paddr_value 
 * @return uint64_t 
 */
uint64_t read_sram(uint64_t paddr_value)
{
    address_t paddr = {
       .paddr_value = paddr_value
    };

    assert(paddr.CI < CACHE_SET_COUNT);

    cache_set_t *cache_set = &sram.set[paddr.CI];
  
    cache_line_t *victim_line = NULL;
    cache_line_t *invalid_line = NULL;
    for (int j = 0; j < CACHE_LINE_COUNT; j++)
    {
        cache_line_t *line = &cache_set->line[j];
        if(line->status == INVALID ){
           invalid_line = line;
        }

        line->time++;
        if(victim_line == NULL || (line->time > victim_line->time)){
            victim_line = line;
        }
    }

    for (int j = 0; j < CACHE_LINE_COUNT; j++)
    {
      cache_line_t *line = &cache_set->line[j];
      if(line->status != INVALID && line->tag == paddr.CT){
            //cache hit
            return  *(uint64_t*)&line->block[paddr.CO*sizeof(uint64_t)];
        }
    }

    if(invalid_line != NULL){
        read_dram_to_cache(paddr_value,invalid_line);
            
        invalid_line->status = VALID;
         invalid_line->tag=paddr.CT;
        invalid_line->time = 0;  
        return  *(uint64_t*)&invalid_line->block[paddr.CO];
    }

    assert(victim_line != NULL);
    
    if(victim_line->status == DIRTY){
        //victim_paddr = tag(40)ci(6)co(6)
        uint64_t victim_paddr=
          victim_line->tag<<(SRAM_CACHE_OFFSET_LENGTH+SRAM_CACHE_INDEX_LENGTH) 
          + paddr.CI<<SRAM_CACHE_INDEX_LENGTH;
        write_cache_to_dram(victim_paddr,victim_line);
    }
   
    read_dram_to_cache(paddr_value,victim_line);
    victim_line->status=VALID;
    victim_line->tag=paddr.CT;
    victim_line->time=0;

    return *(uint64_t*)&victim_line->block[paddr.CO];
}


void write_sram(uint64_t paddr_value,uint64_t data)
{
    address_t paddr = {
       .paddr_value = paddr_value
    };

    assert(paddr.CI < CACHE_SET_COUNT);

    cache_set_t *cache_set = &sram.set[paddr.CI];
  
    cache_line_t *victim_line = NULL;
    cache_line_t *invalid_line = NULL;
    for (int j = 0; j < CACHE_LINE_COUNT; j++)
    {
        cache_line_t *line = &cache_set->line[j];
        if(line->status == INVALID ){
          invalid_line = line;
        } 

        line->time++;
        if(victim_line == NULL || (line->time > victim_line->time)){
            victim_line = line;
        }
    }

    for (int j = 0; j < CACHE_LINE_COUNT; j++)
    { 
         cache_line_t *line = &cache_set->line[j];
        //when ct equals , status always change to dirty 
        //valid -> dirty
        //invalid -> dirty
        //dirty -> dirty
        if(line->tag == paddr.CT){
            //cache hit
            *(uint64_t*)&line->block[paddr.CO] = data;
            line->time=0;
            line->status = DIRTY;
            return ;
        }
    }
    if(invalid_line != NULL){
        *(uint64_t*)&invalid_line->block[paddr.CO] = data;
            
        invalid_line->status = DIRTY;
        invalid_line->tag=paddr.CT;
        invalid_line->time = 0;  
        return;
    }


    assert(victim_line != NULL);
    
    if(victim_line->status == DIRTY){
        uint64_t victim_paddr=
           victim_line->tag<<(SRAM_CACHE_OFFSET_LENGTH+SRAM_CACHE_INDEX_LENGTH) & paddr.CI;
        write_cache_to_dram(victim_paddr,victim_line);
    }
   
    read_dram_to_cache(paddr_value,victim_line);
    victim_line->status=VALID;
    victim_line->tag=paddr.CT;
    victim_line->time=0;
}

void read_dram_to_cache(uint64_t paddr_value,cache_line_t *line){
    uint64_t base_paddr = (paddr_value) >> BLOCK_BIT_LENGTH <<BLOCK_BIT_LENGTH;
    int num = BLOCK_BYTE_COUNT/sizeof(uint64_t) ;
    for(int i=0;i<num;i++){
      *(uint64_t*)&line->block[i] = bus_read(base_paddr+i*sizeof(uint64_t));
    }
}

//从line 的block写回到paddr
void write_cache_to_dram(uint64_t paddr_value,cache_line_t *line)
{
   uint64_t base_paddr = (paddr_value) >> BLOCK_BIT_LENGTH <<BLOCK_BIT_LENGTH;
   int num = BLOCK_BYTE_COUNT/sizeof(uint64_t) ;
    for(int i=0;i<num;i++){
      bus_write(base_paddr+i*sizeof(uint64_t),*(uint64_t*)&line->block[i*sizeof(uint64_t)]);
    }
}




