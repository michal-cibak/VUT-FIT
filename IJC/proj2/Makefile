# Project: IJC - DU2 1) 2)
# Author: Michal Cibák - xcibak00, FIT VUT
# Built: gcc (GCC) 7.5.0
# Date: 17.4.2021


FLAGS = -g -std=c99 -pedantic -Wall -Wextra
PROGS = tail

.PHONY: all
all: $(PROGS)

tail: tail.c
	gcc $(FLAGS) tail.c -o tail

.PHONY: clean
clean:
	-rm $(PROGS)

.PHONY: run
run: all
	./tail

.PHONY: zip
zip:
	zip xcibak00.zip *.c *.h Makefile
