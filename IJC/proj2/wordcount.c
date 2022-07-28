// Project: IJC - DU2 2)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 17.4.2021

// Notes:
// - untested, I didnt manage to do everything on time


#include "htab.h"
#include "io.h"
#include <stdio.h>


void f(htab_pair_t *data)
{
    // cout << mi.first << "\t" << mi.second << "\n";
    printf("%s\t%d\n", data->key, data->value);
}

int main(int argc, char const *argv[])
{
    htab_t *m = htab_init(32768); // individuals use 20000 to 30000 english words, this should be enough for all
    const int WORD_BUFF_SIZE = 128;
    char word[WORD_BUFF_SIZE];
    int num_read_chars;
    int err_printed = 0;
    while ((num_read_chars = read_word(word, WORD_BUFF_SIZE, stdin)) != EOF)
    {
        htab_lookup_add(m, word);
        if (num_read_chars >= WORD_BUFF_SIZE && !err_printed)
        {
            fprintf(stderr, "First word longer than or equal to %d\n", WORD_BUFF_SIZE);
            err_printed = 1;
        }
    }
    htab_for_each(m, f);

    htab_free(m);

    return 0;
}

// TODO - V programu zvolte vhodnou velikost pole a v komentáři zdůvodněte vaše rozhodnutí.
