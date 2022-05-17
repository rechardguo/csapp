#include <headers/algorithm.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#define MIN_SIZE (8)

array_t *array_construct()
{
    array_t *array = malloc(sizeof(array_t));
    array->index = 0;
    array->size = MIN_SIZE;
    uint64_t *table = malloc(array->size * sizeof(uint64_t));
    array->table = table;
    return array;
}


void array_free(array_t *arr)
{
    free(arr->table);
    free(arr);
}
/**
 * @brief 
 * 
 * @param address 
 * @param value 
 * @return int 0: fail 1:success 
 */
int array_insert(array_t **address, uint64_t value)
{
    array_t *arr = *address;
    if(arr==NULL){
        return 0;
    }
    
    uint64_t *oldTab = arr->table;
    if( arr->index == arr->size ){
         arr->table = malloc(sizeof(uint64_t) * (arr->size * 2) );
         for(int i=0;i<arr->index;i++){
              arr->table[i] = oldTab[i];
         }
         arr->size = arr->size * 2;
         free(oldTab);
    }
    
    arr->table[arr->index] = value;
    arr->index++;
    return 1;
}


int array_delete(array_t *arr, int index)
{
    if(arr==NULL){
        return 0;
    }
    if( (index > arr->index) || (arr->index == 0) )
        return 0;
    
    for(int i=index;i<arr->index-1;i++){
        arr->table[i] = arr->table[i+1];
    }
    arr->index -- ;

    uint64_t *oldTab = arr->table;
    //shrink table when index quater of size
    if( (arr->size/4 >= MIN_SIZE) && ( (arr->index-1) <= arr->size/4) ){
        arr->table = malloc(sizeof(uint64_t) * arr->size /4 );
        for(int i=0;i<arr->index;i++){
            arr->table[i] = oldTab[i];
        }
        arr->size = arr->size/4;
        free(oldTab);
    }

    return 1;
}
int array_get(array_t *arr, int index, uint64_t *valptr)
{
    //assert(index <= arr->index);
    //assert(arr->index>0);
    if(index >arr->index || arr->index==0 ){
        return 0;
    }
    *valptr = arr->table[index];
    return 1;
}
void print_array(array_t *arr)
{
    printf("table size:%d \t table current index:%d \n", arr->size , (arr->index-1));
     for(int i=0;i<arr->index;i++){
        printf("\t[%d] : %16lx \n", i, arr->table[i]);
    } 
}
