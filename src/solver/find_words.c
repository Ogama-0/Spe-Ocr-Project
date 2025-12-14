#include "../pretreatment/blocks_treatment.h"
#include "solver.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 256

// Reads every word (one per line) of a file into an array of strings.
// Returns the array and puts the number of words read in *count.
char **read_words(const char *filename, int *count)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        perror("fopen");
        return NULL;
    }

    int capacity = 10;
    int n = 0;
    char **words = malloc(capacity * sizeof(char *));
    if (!words)
    {
        fclose(f);
        return NULL;
    }

    char buffer[MAX_LINE_LEN];
    while (fgets(buffer, sizeof(buffer), f) != NULL)
    {
        // take out the '\n' if it exists.
        buffer[strcspn(buffer, "\r\n")] = '\0';

        // ignore the possibly empty lines
        if (buffer[0] == '\0')
            continue;

        // grow the array if needed.
        if (n == capacity)
        {
            capacity *= 2;
            char **tmp = realloc(words, capacity * sizeof(char *));
            if (!tmp)
            {
                // if error, free what has been allocated.
                for (int i = 0; i < n; i++)
                    free(words[i]);
                free(words);
                fclose(f);
                return NULL;
            }
            words = tmp;
        }

        // allocate just the necessary length and copy the word.
        words[n] = malloc(strlen(buffer) + 1);
        if (!words[n])
        {
            // if error, free all.
            for (int i = 0; i < n; i++)
                free(words[i]);
            free(words);
            fclose(f);
            return NULL;
        }
        strcpy(words[n], buffer);
        n++;
    }

    fclose(f);
    *count = n;
    return words;
}

int **line_pos(struct Rect *grid_blocks, int *words_count)
{
    char **words = read_words("input_solver/words", words_count);
    if (!words || *words_count == -1)
    {
        fprintf(stderr, "Error on reading words\n");
        return NULL;
    }

    int **globalsresults = calloc(*words_count, sizeof(int *));

    for (int i = 0; i < *words_count; i++)
    {
        // int results[4];
        int *results = calloc(4, sizeof(int));
        solve_word("input_solver/grid", words[i], grid_blocks, results);
        for (int i = 0; i < 4; i++)
        {
            printf("%i ", results[i]);
        }
        printf("\n");
        globalsresults[i] = results;
        free(words[i]);
    }
    free(words);
    free(grid_blocks);

    return globalsresults;
}
