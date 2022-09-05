#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "headers/algorithm.h"

int main(int argc, char const *argv[])
{
    
    array_t *arr = array_construct();
    for(int i=0;i<100;i++){
       assert(array_insert(&arr,i)==1);
    }

    print_array(arr);

    for(int i=1;i<80;i++){
       assert(array_delete(arr,0)==1);
    }
    assert(arr->index-1==20);
    print_array(arr);

    //error
    //这个指针是空的
    //uint64_t *val;
    //不同于uint64_t val 这个是有值的，在于栈上分配
    //assert(array_get(arr,arr->index-1,val)==1);
    //assert(val == 99);

    uint64_t *val = malloc(sizeof(uint64_t));
    assert(array_get(arr,arr->index-1,val)==1);
    assert(*val == 99);

    // uint64_t val;
    // assert(array_get(arr,arr->index-1,&val)==1);
    // assert(val == 99);
     
    array_free(arr); 
    return 0;
}
