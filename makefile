CC=gcc

CFLAG= -Wall -g -o2 -Werror -std=gnu99

EXE=program


SRC = ./src

CODE = $(SRC)/memory/instruction.c $(SRC)/disk/code.c $(SRC)/memory/dram.c $(SRC)/cpu/mmu.c  $(SRC)/main.c


.PHONY: program

main:
	$(CC) $(CFLAG) -I$(SRC) $(CODE) -o $(EXE)

run:
	./$(EXE) 