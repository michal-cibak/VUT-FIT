# Project: IPK - Packet sniffer
# Author: Michal Cibák - xcibak00

EXE=ipk-sniffer
CC=gcc
CFLAGS=-pedantic -lm
LFLAGS=-lpcap

all: $(EXE).o $(EXE).c
	$(CC) $(CFLAGS) $(EXE).c $(LFLAGS) -o $(EXE)

.PHONY: clean
clean:
	-rm $(EXE) $(EXE).o
