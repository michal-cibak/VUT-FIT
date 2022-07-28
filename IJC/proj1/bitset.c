// Project: IJC - DU1 a)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 15.3.2021


#include "bitset.h"
#include <stdbool.h>


#ifdef USE_INLINE

extern void bitset_free(bitset_t name);

extern unsigned long bitset_size(bitset_t name);

extern void bitset_setbit(bitset_t name, bitset_index_t index, bool expression);

extern unsigned long bitset_getbit(bitset_t name, bitset_index_t index);

#endif
