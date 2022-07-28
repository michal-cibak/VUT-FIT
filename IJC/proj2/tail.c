// Project: IJC - DU2 1)
// Author: Michal Cibák - xcibak00, FIT VUT
// Built: gcc (GCC) 7.5.0
// Date: 17.4.2021

// Notes:
// - free() isn't used because there is no need to, program ends immediately
//   after the output is printed or after an error appers


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


int main(int argc, char const *argv[])
{
    // look for "-n number" argument and/or filename
    unsigned long tail_size = 0;
    const char *filename = NULL;
    unsigned long tail_from_line = 0;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (!strcmp(argv[i], "-n"))
            {
                if (tail_size) // value already set
                {
                    fprintf(stderr, "More than one occurrence of argument -n.\n");
                    return 1;
                }
                if (argc == i + 1)
                {
                    fprintf(stderr, "Missing second argument after argument -n.\n");
                    return 1;
                }
                char *after_num = NULL;
                unsigned long num = strtoul(argv[++i], &after_num, 0); // ++i so the next arg isn't checked twice
                if (!after_num || *after_num || !num)
                {
                    fprintf(stderr, "Invalid value after argument -n.\n");
                    return 1;
                }
                if (argv[i][0] == '+')
                    tail_size = num;
                else
                    tail_from_line = num;
            }
            else
            {
                if (filename)
                {
                    fprintf(stderr, "More than one occurrance of a standalone argument (name of file).\n");
                    return 1;
                }
                filename = argv[i];
            }
        }
    }
    if (!tail_size && !tail_from_line) // tail length and starting line of tail weren't set
        tail_size = 10;

    // open file if name was provided
    FILE *file_p = NULL;
    if (filename)
    {
        file_p = fopen(filename, "r");
        if (!file_p)
        {
            fprintf(stderr, "File \"%s\" couldn't be opened.\n", filename);
            return 1;
        }
    }
    else
        file_p = stdin;

    // read and print
    if (tail_size) // last n lines should be printed (not lines from n onward)
    {
        // read the input line by line and store the last tail_size of them
        char **lines = malloc(tail_size * sizeof(char *)); // array of pointers to read lines
        if (!lines)
            goto allocation_error;
        const unsigned short line_buff_size = 512;
        unsigned long current_line = 0;
        bool needs_allocation = true;
        bool first_longer_line = true;
        for (;; current_line = (current_line + 1) % tail_size)
        {
            // allocate memory for the line (unless memory for all the lines is already allocated)
            if (needs_allocation)
            {
                lines[current_line] = malloc(line_buff_size);
                if (!lines[current_line])
                    goto allocation_error;
                lines[current_line][0] = '\0'; // empty sring marks situation where EOF comes before tail_size lines were read
                if (current_line == tail_size - 1)
                    needs_allocation = false;
            }

            // read the line
            if (!fgets(lines[current_line], line_buff_size, file_p))
            {
                if (feof(file_p))
                    break;
                else
                    goto read_error;
            }

            // check if the line was longer than the buffer size - 1, skip the rest if so, print error if it is the first time
            unsigned short line_length = strlen(lines[current_line]);

            if (line_length == line_buff_size - 1 && lines[current_line][line_buff_size - 2] != '\n')
            {
                if (first_longer_line)
                {
                    fprintf(stderr, "First line equal to or longer than %d encountered. Characters will be skipped until the end of line is reached.", line_buff_size - 1);
                    first_longer_line = false;
                }
                int c;
                do
                    c = getc(file_p); // one-liner
                while (c != '\n' || c != EOF);
                if (!feof(file_p))
                    goto read_error;
            }
        }

        // print the stored lines
        unsigned long last_line = current_line ? current_line - 1 : tail_size - 1;
        if (lines[current_line][0] == '\0') // empty string -> less than tail_size lines have been read
        {
            if (current_line == 0)
                last_line = 0;
            current_line = 0;
        }

        for (; current_line != last_line; current_line = (current_line + 1) % tail_size)
            fputs(lines[current_line], stdout); // one-liner
        fputs(lines[last_line], stdout);
        unsigned short line_length = strlen(lines[last_line]);
        if (line_length == 0 || lines[last_line][line_length - 1] != '\n')
            putchar('\n');
    }
    else // lines from line n onward should be printed
    {
        int c = 0, prev;
        for (;;) // go through input character by character
        {
            prev = c;
            c = getc(file_p);
            if (c == EOF)
            {
                if (feof(file_p))
                {
                    if (prev != '\n')
                        putchar('\n');
                    break;
                }
                else
                    goto read_error;
            }
            if (tail_from_line) // skipping first n-1 lines
            {
                if (c == '\n')
                    --tail_from_line;
            }
            else
                putchar(c);
        }
    }

    // close the file if it was opened
    if (file_p && fclose(file_p))
        fprintf(stderr, "File \"%s\" couldn't be closed.\n", filename);


    return 0;


read_error:
    fprintf(stderr, "Error occured when input was read.\n");
    if (file_p && fclose(file_p))
        fprintf(stderr, "File \"%s\" couldn't be closed.\n", filename);
    return 1;

allocation_error:
    fprintf(stderr, "Memory allocation failed.\n");
    if (filename && fclose(file_p))
        fprintf(stderr, "File \"%s\" couldn't be closed.\n", filename);
    return 1;
}
