// Project: IJC - DU2 2)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 25.4.2021


#include <stdio.h> // getc()
#include <ctype.h> // isspace()


int read_word(char *s, int max, FILE *f)
{
    if (max < 0)
        max = 0;
    int c; // current character
    int i = 0; // read characters

    // skip spaces
    while ((c = getc(f)) != EOF && isspace(c))
        ; // do nothing
    if (c == EOF)
        return EOF;
    else
        ungetc(c, f);

    // read word
    while ((c = getc(f)) != EOF && !isspace(c))
    {
        if (i < max - 1)
            s[i] = c;
        i++;
    }
    if (max)
        s[max - 1] = '\0';

    return i;
}
