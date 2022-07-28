# Project: IJC - DU1 a) b)
# Author: Michal Cibák - xcibak00, FIT VUT
# Built: gcc (GCC) 7.5.0
# Date: 4.3.2021


FLAGS = -g -std=c11 -pedantic -Wall -Wextra -O2 -lm
PROGS = primes primes-i steg-decode

all: $(PROGS)

primes: primes.c bitset.h eratosthenes.h eratosthenes.c error.h error.c
	gcc $(FLAGS) primes.c eratosthenes.c error.c -o primes

primes-i: primes.c bitset.h bitset.c eratosthenes.h eratosthenes.c error.h error.c
	gcc -DUSE_INLINE $(FLAGS) primes.c bitset.c eratosthenes.c error.c -o primes-i

steg-decode: steg-decode.c ppm.h ppm.c bitset.h eratosthenes.h eratosthenes.c error.h error.c
	gcc $(FLAGS) steg-decode.c ppm.c eratosthenes.c error.c -o steg-decode

clean:
	-rm $(PROGS)

run: all
	./primes
	./primes-i
	./steg-decode du1-obrazek.ppm

zip:
	zip xcibak00.zip *.c *.h Makefile
