#include "ext_img.h"
#include "img.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void imgTo_print(ImgToPredict *img)
{
    matrix_print(img->img_data);
    printf("Img Label: %c\n", img->label + 'a');
}

void imgTo_free(ImgToPredict *img)
{
    matrix_free(img->img_data);
    free(img);
    img = NULL;
}

void imgsTo_free(ImgToPredict **imgs, int n)
{
    for (int i = 0; i < n; i++)
    {
        imgTo_free(imgs[i]);
    }
    free(imgs);
    imgs = NULL;
}

int imgsTo_get_row(ImgToPredict **imgs, size_t count)
{
    int res = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (imgs[i]->grid_x > res)
        {
            res = imgs[i]->grid_x;
        }
    }
    return res + 1;
}
int imgsTo_get_col(ImgToPredict **imgs, size_t count)
{
    int res = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (imgs[i]->grid_y > res)
        {
            res = imgs[i]->grid_y;
        }
    }
    return res + 1;
}

int_matrix *Imgs_To_Matrix(ImgToPredict **imgs, size_t count)
{
    int row = imgsTo_get_row(imgs, count);
    int col = imgsTo_get_col(imgs, count);
    int_matrix *res = int_matrix_create(row, col);
    for (size_t i = 0; i < count; i++)
    {
        int y = imgs[i]->grid_y;
        int x = imgs[i]->grid_x;
        res->entries[y][x] = imgs[i]->label;
    }
    return res;
}

void img_save_predict(char *path, ImgToPredict **imgs, size_t count)
{
    int_matrix *mat = Imgs_To_Matrix(imgs, count);
    int_matrix_print(mat);
    int_matrix_save(mat, path);
    int_matrix_free(mat);
}

/**
 * @brief Converts an array of ImgToPredict structures to a list of words.
 *
 * This function processes an array of ImgToPredict structures, organizing them
 * into words based on their grid_y (word index) and grid_x (character index)
 * properties.
 *
 * @param imgs An array of pointers to ImgToPredict structures.
 * @param count The number of elements in the imgs array.
 * @param num_words_out A pointer to an integer where the number of words will
 * be stored.
 * @param word_lengths_out A pointer to a size_t pointer where the array of word
 * lengths will be stored.
 * @return A dynamically allocated array of strings (char **), where each string
 * is a word. The strings may contain null characters ('\0') for missing
 * letters. Returns NULL on allocation failure or if count is 0. The caller is
 * responsible for freeing the returned array of strings, each string within it,
 * and the word_lengths array.
 */
char **imgs_to_words_list(ImgToPredict **imgs, size_t count, int *num_words_out,
                          size_t **word_lengths_out)
{
    if (count == 0)
    {
        *num_words_out = 0;
        *word_lengths_out = NULL;
        return NULL;
    }

    int num_words = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (imgs[i]->grid_y > num_words)
        {
            num_words = imgs[i]->grid_y;
        }
    }
    num_words++;
    printf("nb_words :%i\n", num_words);

    size_t *word_lengths = calloc(num_words, sizeof(size_t));
    if (!word_lengths)
    {
        *num_words_out = 0;
        *word_lengths_out = NULL;
        return NULL;
    }

    for (size_t i = 0; i < count; i++)
    {
        int word_idx = imgs[i]->grid_y;
        if ((size_t)imgs[i]->grid_x + 1 > word_lengths[word_idx])
        {
            word_lengths[word_idx] = imgs[i]->grid_x + 1;
        }
    }

    char **words = malloc(num_words * sizeof(char *));
    if (!words)
    {
        free(word_lengths);
        *num_words_out = 0;
        *word_lengths_out = NULL;
        return NULL;
    }

    for (int i = 0; i < num_words; i++)
    {
        words[i] = calloc(word_lengths[i] + 1, sizeof(char));
        if (!words[i])
        {
            for (int j = 0; j < i; j++)
            {
                free(words[j]);
            }
            free(words);
            free(word_lengths);
            *num_words_out = 0;
            *word_lengths_out = NULL;
            return NULL;
        }
    }

    for (size_t i = 0; i < count; i++)
    {
        int word_idx = imgs[i]->grid_y;
        int char_idx = imgs[i]->grid_x;
        words[word_idx][char_idx] = imgs[i]->label;
    }

    *num_words_out = num_words;
    *word_lengths_out = word_lengths;
    return words;
}

/**
 * @brief Saves the recognized words to a file.
 *
 * This function takes an array of ImgToPredict, converts them to words,
 * and saves them as a single line of space-separated text in the specified
 * file. Unrecognized characters within words are replaced with '?'.
 *
 * @param path The path to the output file.
 * @param imgs An array of pointers to ImgToPredict structures.
 * @param count The number of elements in the imgs array.
 */
void img_save_words_predict(char *path, ImgToPredict **imgs, size_t count)
{
    printf("in img_save_words_predicts\n");
    int num_words = 0;
    size_t *word_lengths = NULL;
    char **words = imgs_to_words_list(imgs, count, &num_words, &word_lengths);
    printf("len words : %i\n", num_words);

    for (int i = 0; i < num_words; i++)
    {
        printf("%s\n", words[i]);
    }

    FILE *file = fopen(path, "w");
    if (!file)
    {
        if (words)
        {
            for (int i = 0; i < num_words; i++)
            {
                free(words[i]);
            }
            free(words);
        }
        free(word_lengths);
        return;
    }

    if (words)
    {
        int first_word = 1;
        for (int i = 0; i < num_words; i++)
        {
            if (word_lengths[i] == 0)
                continue;

            if (!first_word)
            {
                fprintf(file, "\n");
            }
            first_word = 0;

            for (size_t j = 0; j < word_lengths[i]; j++)
            {
                if (words[i][j] == '\0')
                {
                    words[i][j] = '?';
                }
            }
            fprintf(file, "%s", words[i]);
        }
        fprintf(file, "\n");

        for (int i = 0; i < num_words; i++)
        {
            free(words[i]);
        }
        free(words);
        free(word_lengths);
    }

    fclose(file);
    printf("out img_save_words_predicts\n");
}
