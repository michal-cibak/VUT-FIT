# Project: IMS - Epidemiologické modely - mikro-úroveň
# Authors: Michal Cibák - xcibak00
# 		   Mirka Kolaříková - xkolar76

NAME=simulation
EXTENSION=cpp
COMPILER=g++
CFLAGS=-Wall -Wextra -pedantic -lm
LFLAGS=-lsimlib

all: $(NAME).o $(NAME).cpp
	$(COMPILER) $(CFLAGS) $(NAME).$(EXTENSION) $(LFLAGS) -o $(NAME)

clean:
	-rm $(NAME) $(NAME).o

run:
	./simulation --pdiddis 0.5 --pwilldis 0.5
