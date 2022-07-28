# Vypracoval: Michal Cibak
# Login: xcibak00
# IOS Projekt 2: The senate bus problem

CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -lrt
SOURCE=proj2.c
BIN=proj2
LFLAGS=-lpthread

all:
	$(CC) $(CFLAGS) $(SOURCE) -o $(BIN) $(LFLAGS)

run: all
	./$(BIN)

clean:
	rm $(BIN)
