#include <stdio.h>
// #include "bitset.h"
#include "ppm.h"


int main(void)
{
    struct ppm *ppm = ppm_read("du1-obrazek.ppm");
    printf("width: %u\nheight: %u\n", ppm->xsize, ppm->ysize);
    puts("OK");
    for (unsigned i = 0; i < 3 * ppm->xsize * ppm->ysize; i++)
        printf("%u%s", (unsigned char)ppm->data[i], i % 3 == 2 ? "  " : " ");
    printf("\n");
    ppm_free(ppm);

    // bitset_create(array, 64);

    // for (bitset_index_t i = 0; i < bitset_size(array); i++)
    // {
    //     bitset_setbit(array, i, 1);
    //     printf("%lu %s %lu\n", i, bitset_getbit(array, i) ? "OK" : "ERROR", bitset_getbit(array, i));
    //     // printf("%lu%s", array[1], i != 63 ? ", " : "\n");
    // }
    // puts("\n");

    // bitset_index_t n = 0;
    // name[1 + index / UL_BIT] = (name[1 + index / UL_BIT] & ~(1UL << index % UL_BIT)) | (expression ? 1 : 0) << index % UL_BIT;
    // printf("1 << 31 : %lu\n", 1 << 31);
    // printf("1UL << 31 : %lu\n", 1UL << 31);
    // printf("1 << 32 : %lu\n", 1 << 32);
    // printf("1UL << 32 : %lu\n", 1UL << 32);
    // printf("1 << 63 : %lu\n", 1 << 63);
    // printf("1UL << 63 : %lu\n", 1UL << 63);



    return 0;
}
