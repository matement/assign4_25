
###################################################
#
# file: Makefile
#
# @Author:   Iacovos G. Kolokasis
# @Version:  10-02-2022
# @email:    kolokasis@csd.uoc.gr
#
# Makefile
#
####################################################

CC = gcc
CFLAGS = -Wall -ansi -pedantic
SRC_DIR = src
OBJ = $(SRC_DIR)/sudoku.o $(SRC_DIR)/grid.o
EXEC = sudoku

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(EXEC)

