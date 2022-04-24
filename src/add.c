#include<stdio.h>
#include<stdint.h>
uint64_t add(uint64_t i1,uint64_t i2){
    return i1+i2;
}

int main(){
    uint64_t i1 = 2;
    uint64_t i2 = 5;
    add(i1,i2);
    return 0;
}