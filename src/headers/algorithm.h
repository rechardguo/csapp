// include guards to prevent double declaration of any identifiers 
// such as types, enums and static variables
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

array_t *array_construct();
void array_free(array_t *arr);
int array_insert(array_t **address, uint64_t value);
int array_delete(array_t *arr, int index);
int array_get(array_t *arr, int index, uint64_t *valptr);
void print_array(array_t *arr);

#endif