// Project: IJC - DU1 a)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 4.3.2021

// Notes:
// - use of "bitset_index_t" (an unsigned type) for indexes when using macros
//   is expected from a user as the value is NOT checked to be ">= 0" nor
//   converted to said type, improper use will result in an unexpected behavior


#ifndef GUARD_BITSET_H
#define GUARD_BITSET_H


#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include <stdbool.h>


typedef unsigned long *bitset_t;
typedef unsigned long bitset_index_t;


#define UL_BIT (CHAR_BIT * sizeof(unsigned long))

#define bitset_create(name, size) \
    static_assert(size > 0, "Invalid size of bit array"); \
    unsigned long name[1 + (size - 1) / UL_BIT + 1] = {size, }

#define bitset_alloc(name, size) \
    assert(size > 0 && "Invalid size of bit array"); /* ("text", condition) causes warning... */ \
    bitset_t name = calloc(1 + (size - 1) / UL_BIT + 1, sizeof(unsigned long)); \
    if (name) name[0] = size; \
    else error_exit("bitset_alloc: Chyba alokace paměti")

#ifndef USE_INLINE

#define bitset_free(name) \
    free(name)

#define bitset_size(name) \
    name[0]

#define bitset_setbit(name, index, expression) \
    if ((index) >= name[0]) error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu", (unsigned long)(index), (unsigned long)name[0] - 1); \
    else name[1 + (index) / UL_BIT] = (name[1 + (index) / UL_BIT] & ~(1UL << (index) % UL_BIT)) | ((expression) ? 1UL : 0UL) << (index) % UL_BIT

#define bitset_getbit(name, index) \
    (((index) >= name[0]) ? error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu", (unsigned long)(index), (unsigned long)name[0] - 1), 0UL : (name[1 + (index) / UL_BIT] >> (index) % UL_BIT & 1UL))

#endif

#ifdef USE_INLINE

inline void bitset_free(bitset_t name)
{
    free(name);
}

inline unsigned long bitset_size(bitset_t name)
{
    return name[0];
}

inline void bitset_setbit(bitset_t name, bitset_index_t index, bool expression)
{
    if (index >= name[0])
        error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)name[0] - 1);
    else
        name[1 + index / UL_BIT] = (name[1 + index / UL_BIT] & ~(1UL << index % UL_BIT)) | (expression ? 1UL : 0UL) << index % UL_BIT;
}

inline unsigned long bitset_getbit(bitset_t name, bitset_index_t index)
{
    if (index >= name[0])
        error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu", (unsigned long)index, (unsigned long)name[0] - 1);
    return name[1 + index / UL_BIT] >> index % UL_BIT & 1UL;
}

#endif // USE_INLINE


#endif // GUARD_BITSET_H
