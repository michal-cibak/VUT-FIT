// Project: IJC - DU1 a)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 18.3.2021


#include "bitset.h"
#include "eratosthenes.h"
#include <stdio.h>
#include <time.h>


int main(void)
{
    // note the time of start of program
    clock_t start = clock();

    // search for primes before 200 000 000
    bitset_create(bitarray, 200000000);
    Eratosthenes(bitarray);

    // pick last 10
    bitset_index_t primes[10];
    bitset_index_t index;
    int i;
    for (index = 199999999, i = 9; i >= 0; index--)
        if (bitset_getbit(bitarray, index) == 0)
            primes[i--] = index;

    // print them
    for (i = 0; i < 10; i++)
        printf("%lu\n", primes[i]);

    // print run time of program
    fprintf(stderr, "Time=%.3g\n", (double)(clock() - start) / CLOCKS_PER_SEC);


    return 0;
}
