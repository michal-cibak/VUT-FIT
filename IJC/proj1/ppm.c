// Project: IJC - DU1 b)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 20.3.2021


#include "ppm.h"
#include <stdlib.h>
#include "error.h"
#include <stdio.h>
#include <string.h>


struct ppm * ppm_read(const char * filename)
{
    // open ppm file
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        warning_msg("file %s can't be opened\n", filename);
        return NULL;
    }

    // read the file header
    char ppmtype[3] = {0};
    if (fgets(ppmtype, 3, fp) == NULL || strcmp(ppmtype, "P6") != 0)
        goto format_error;

    int c = getc(fp);
    if (c == EOF || (c != ' ' && c != '\t' && c != '\n'))
        goto format_error;

    unsigned maxval, xsize, ysize;
    if (fscanf(fp, "%u %u %u", &xsize, &ysize, &maxval) != 3 || maxval != 255)
        goto format_error;

    if (xsize > 8000 || ysize > 8000) // no check for 0 size as it is valid
    {
        warning_msg("image is too big, maximum size is 8000 by 8000\n");
        goto error;
    }

    c = getc(fp);
    if (c == EOF || (c != ' ' && c != '\t' && c != '\n'))
        goto format_error;

    // allocate memory for ppm structure and fill it
    struct ppm *ppm = malloc(sizeof(struct ppm) + 3 * xsize * ysize);
    if (!ppm)
    {
        warning_msg("memory allocation failed\n");
        goto error;
    }
    ppm->xsize = xsize;
    ppm->ysize = ysize;
    if (!fread(&ppm->data, 3 * xsize * ysize, 1, fp) || !(getc(fp), feof(fp)))
    {
        free(ppm);
        goto format_error;
    }

    // close the file
    if (fclose(fp))
        warning_msg("file %s failed to close\n", filename);

    return ppm;


format_error:
    warning_msg("reading of file failed, wrong format\n");
error:
    if (fclose(fp))
        warning_msg("file %s failed to close\n", filename);
    return NULL;
}


void ppm_free(struct ppm *p)
{
    free(p);
}
