// Project: IJC - DU2 2)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 26.4.2021

// Notes:
// - I didn't manage to test everything and split it into libraries in time


#include <stdint.h> // uint32_t
#include <stddef.h> // size_t
#include <stdlib.h> // malloc()
#include <string.h> // strcmp(), strlen()
#include <stdbool.h> // strcmp(), strlen()

#include "htab.h"

// TODO - in its own private header file
typedef struct htab_item
{
    htab_pair_t;
    struct htab_item *next;
}
htab_item_t;

struct htab
{
    size_t size;
    size_t arr_size;
    htab_item_t *array[];
};

size_t htab_hash_function(const char *str)
{
    uint32_t h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}

htab_t *htab_init(size_t n)
{
    htab_t *table = malloc(sizeof(htab_t) + n * sizeof(htab_item_t *));
    if (table)
    {
        table->size = 0;
        table->arr_size = n;
        while (n--) // go through the whole array
        {
            table->array[n].key = NULL;
            table->array[n].value = 0;
            table->next = NULL;
        }
    }
    return table;
}

htab_t *htab_move(size_t n, htab_t *from)
{
    htab_t *to = htab_init(n);
    if (!to)
        return NULL;

    size_t num_items = from->size;
    for (size_t from_index = 0; num_items; from_index++) // go through the array of pointers to items until all items are visited
    {
        htab_item_t *from_item_p = from->array[from_index];
        while (from_item_p) // go through the list of items at current index
        {
            size_t to_index = htab_hash_function(from_item_p->key) % n;
            htab_item_t *to_item_p = to->array[to_index];
            if (!to_item_p) // there is no item on that index yet
                to->array[to_index] = from_item_p;
            else
            {
                while (to_item_p->next)
                    to_item_p = to_item_p->next;
                to_item_p->next = from_item_p;
            }

            from_item_p = from_item_p->next;
            to_item_p->next = NULL;
            --num_items;
        }
        from->array[from_index] = NULL;
    }
    to->size = from->size;
    from->size = 0;

    return to;
}

size_t htab_size(const htab_t * t)
{
    return t->size;
}

size_t htab_bucket_count(const htab_t * t)
{
    return t->arr_size;
}

htab_pair_t * htab_find(htab_t * t, htab_key_t key)
{
    size_t index = htab_hash_function(key) % n;
    htab_item_t *item_p = t->array[index];
    while (item_p && strcmp(item_p->value, key))
        item_p = item_p->next;

    return (htab_pair_t *)item_p;
}

htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key)
{
    size_t index = htab_hash_function(key) % t->arr_size;
    htab_item_t *item_p = t->array[index];
    while (item_p && strcmp(item_p->value, key))
        item_p = item_p->next;

    if (!item_p) // key wasn't found
    {
        // create new item for the table
        item_p = malloc(sizeof(htab_item_t));
        if (!item_p)
            return NULL;

        // copy the key
        size_t key_length = strlen(key);
        char *key_copy = malloc(key_length + 1);
        if (!key_copy)
        {
            free(item_p);
            return NULL;
        }
        strncpy(key_copy, key, key_length + 1); // + 1 so '\0' is added to the end

        // initialize the item
        item_p->key = (const char *)key_copy;
        item_p->value = 1;
        item_p->next = NULL;

        // add the item to the table
        htab_item_t *last_item_p = t->array[index];
        if (!last_item_p) // first in the item list for current index
            t->array[index] = item_p;
        else
        {
            while (last_item_p->next)
                last_item_p = last_item_p->next;
            last_item_p->next = item_p;
        }
        t->size++;
    }

    return (htab_pair_t *)item_p;
}

bool htab_erase(htab_t * t, htab_key_t key)
{
    // find the item and its predecessor
    size_t index = htab_hash_function(key) % t->arr_size;
    htab_item_t *prev_item_p = NULL;
    htab_item_t *item_p = t->array[index];
    while (item_p && strcmp(item_p->value, key))
    {
        prev_item_p = item_p;
        item_p = item_p->next;
    }
    if (!item_p) // item not found
        return false; // could return true, but then true would be returned always, so there would be no point for a return value

    // change the predecessor's successor
    if (prev_item_p)
        prev_item_p->next = item_p->next;
    else
        t->array[index] = item_p->next;

    // delete the desired item
    free(item_p->key);
    free(item_p);
    t->size--;

    return true;
}

void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data))
{
    size_t num_items = t->size;
    for (size_t index = 0; num_items; index++) // go through the array of pointers to items until all items are visited
    {
        for (htab_item_t *item_p = t->array[index]; item_p; item_p = item_p->next) // go through the list of items at current index
        {
            f((htab_pair_t *)item_p);
            --num_items;
        }
    }
}

void htab_clear(htab_t * t)
{
    size_t num_items = t->size;
    for (size_t index = 0; num_items; index++) // go through the array of pointers to items until all items are visited
    {
        htab_item_t *item_p = t->array[index];
        while (item_p) // go through the list of items at current index
        {
            htab_item_t *next_item_p = item_p->next;
            free(item_p->key);
            free(item_p);
            --num_items;
            item_p = next_item_p;
        }
    }
    t->size = 0;
}

void htab_free(htab_t * t)
{
    htab_clear(t);
    free(t);
}


// TODO
// split the file into separate modules with 1 function each - struct htab shouldn't be static?
// try #ifdef HASHTEST with own implementation of hash function
// struct htab_item should be in its own private header file
// try #ifdef MOVETEST with htab_move
// there are no checks for NULL passed as function argument
// maybe make the key a constant pointer? for_each could change the key otherwise
// if user decides to change the value of key, things could break
