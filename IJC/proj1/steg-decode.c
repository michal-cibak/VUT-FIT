// Project: IJC - DU1 b)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 21.3.2021

// Notes:
// - number of bits in a character is assumed to be 8 as support of UTF-8 is
//   expected, also because the values stored in character array should be
//   in range 0..255 according to the assignment


#include "error.h"
#include "ppm.h"
#include "bitset.h"
#include "eratosthenes.h"
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>


int main(int argc, char const *argv[])
{
    if (argc != 2)
        error_exit("wrong number of arguments\n");

    // load the image
    struct ppm *ppm = ppm_read(argv[1]);
    if (!ppm)
        error_exit("ppm_read error\n");

    // prepare prime numbers in bitfield array
    unsigned ppmsize = 3 * ppm->xsize * ppm->ysize;
    bitset_alloc(bitarray, ppmsize);
    Eratosthenes(bitarray);

    // go through image color data on prime number positions to find a message
    unsigned char charbits[8]; // individual bits of a character
    unsigned char character; // the character itself
    bitset_index_t bindex = 23;
    signed char cindex = 0;
    for (bindex = 23; bindex < ppmsize; bindex++)
    {
        if (!bitset_getbit(bitarray, bindex)) // a prime number
        {
            // store the LSb of color data
            charbits[cindex++] = ppm->data[bindex] & 1;
            if (cindex == 8)
            {
                // assemble a character from the stored bits
                character = 0;
                while (cindex != 0)
                    character = character << 1 | charbits[--cindex]; // one-liner
                // print the character or stop searching if the message ended
                if (!character)
                    break;
                else if (putchar(character) == EOF)
                    warning_msg("putchar failed, output may not be correct\n");
            }
        }
    }
    putchar('\n');
    bitset_free(bitarray);

    // check if the message was found whole
    if (cindex || character) // search ended prematurely
        error_exit("decoding ended before a final '\0' was reached\n");


    return 0;
}
