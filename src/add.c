#include<stdio.h>
#include<stdint.h>
unsigned add(unsigned i1,unsigned i2){
    return i1+i2;
}

int main(){
    int i1 = 2;
    int i2 = 5;
    add(i1,i2);
    return 0;
}