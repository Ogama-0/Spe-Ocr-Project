#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solver.h"

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

int main(int argc, char *argv[])
{
    int count = 0;
    char **words = read_words("fichier_mots.txt", &count);
    if (!words)
    {
        fprintf(stderr, "Error on reading words\n");
        return 1;
    }

    for (int i = 0; i < count; i++)
    {
        solve_word("grid.txt",words[i]);
        free(words[i]);
    }
    free(words);

    return 0;
}
