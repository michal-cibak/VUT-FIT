// Project: IJC - DU1 b)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 20.3.2021


#ifndef GUARD_PPM_H
#define GUARD_PPM_H


struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];
};


struct ppm * ppm_read(const char * filename);
void ppm_free(struct ppm *p);


#endif // GUARD_PPM_H
