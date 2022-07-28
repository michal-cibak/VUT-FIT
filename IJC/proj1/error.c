// Project: IJC - DU1 b)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 14.3.2021

#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


void warning_msg(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, argp);
    va_end(argp);
}

void error_exit(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    exit(1);
}
