#include<stdio.h>
unsigned add(unsigned i1,unsigned i2){
    return i1+i2;
}

int main(){
    int i1 = 2;
    int i2 = 5;
    printf("%d + % d =%d",i1,i2, add(i1,i2));
}