#ifndef ALGORITHM_GUARD
#define ALGORITHM_GUARD

#include<stdint.h>



/*======================================*/
/*      Dynamic Array                   */
/*======================================*/
typedef struct 
{
    uint32_t size;
    uint32_t index;
    uint64_t *table; 
}array_t;

array_t array_construct();

array_t *array_insert(array_t *arr, uint64_t value);

#endif