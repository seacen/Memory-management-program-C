# 
# Sample Makefile for project 1 (COMP30023)
#
# This example is very basic
#
# The program used for illustration purposes is
# a simple program using prime numbers
#


## CC  = Compiler.
## CFLAGS = Compiler flags.
CC	= gcc
CFLAGS 	= -Wall -ansi


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC =		memswap.c
OBJ =		memswap.o
EXE = 		memswap

## Top level target is executable.
$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -lm


## Clean: Remove object files and core dump files.
clean:
		/bin/rm $(OBJ) 

## Clobber: Performs Clean and removes executable file.

clobber: clean
		/bin/rm $(EXE) 

## Dependencies

memswap.o:	listops.h listops.c


