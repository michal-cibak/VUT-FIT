// Project: IJC - DU1 a)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 17.3.2021

// Notes:
// - bitset_index_t is used here for size of the array as (without looking at
//   the implementation) there is no proper type for it - it is assumed to be
//   of equal type because size is just 1 bigger than the last (biggest) index


#include "eratosthenes.h"
#include "bitset.h"
#include <math.h>


void Eratosthenes(bitset_t array)
{
    bitset_index_t size = bitset_size(array); // index type used to hold size, not index

    // p[0] = 1
    bitset_setbit(array, 0, 1);
    if (size == 1)
        return;

    // p[1] = 1
    bitset_setbit(array, 1, 1);
    if (size == 2)
        return;


    // p[2]..p[N-1] = 0
    bitset_index_t index;
    for (index = 2; index < size; index += 1)
        bitset_setbit(array, index, 0);

    // find first index with 0 and set 1 to all its multiples, repeat until index with sqrt(N) is reached
    index = 2;
    bitset_index_t jindex;
    do
    {
        if (bitset_getbit(array, index) == 0)
        {
            for (jindex = index * 2; jindex < size; jindex += index)
                bitset_setbit(array, jindex, 1);
        }
        index += 1;
    } while (index <= sqrt(size));
}
