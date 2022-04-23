﻿#ifndef dram_guard
#define dram_guard
#include<stdint.h>

#define MM_LEN 1000

uint8_t mm[MM_LEN]; //physical address

void printStack();
void printRegister();
void write64bits_dram(uint64_t,uint64_t);
uint64_t read64bits_dram(uint64_t);
#endif
