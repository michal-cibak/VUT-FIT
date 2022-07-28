// Project: IJC - DU2 2)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 17.4.2021


#ifndef GUARD_HTAB_H
#define GUARD_HTAB_H

#include <string.h> // size_t
#include <stdbool.h> // bool

// table
struct htab;
typedef struct htab htab_t;

// types
typedef const char * htab_key_t;
typedef int htab_value_t;

// data pair in table
typedef struct htab_pair
{
    htab_key_t key;
    htab_value_t value;
}
htab_pair_t;

// hash function
size_t htab_hash_function(htab_key_t str);

// table related functions
htab_t *htab_init(size_t n);
htab_t *htab_move(size_t n, htab_t *from);
size_t htab_size(const htab_t * t);
size_t htab_bucket_count(const htab_t * t);
htab_pair_t * htab_find(htab_t * t, htab_key_t key);
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key);
bool htab_erase(htab_t * t, htab_key_t key);
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data));
void htab_clear(htab_t * t);
void htab_free(htab_t * t);

#endif // GUARD_HTAB_H
