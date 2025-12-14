#include "matrix.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXCHAR 100

/**
 * @brief Creates a new matrix with specified dimensions
 * 
 * @param rows Number of rows in the matrix
 * @param cols Number of columns in the matrix
 * @return Matrix* Pointer to the newly created matrix, NULL on failure
 */
Matrix *matrix_create(int row, int col)
{
    Matrix *matrix = malloc(sizeof(Matrix));
    matrix->rows = row;
    matrix->cols = col;
    matrix->entries = malloc(row * sizeof(double *));
    for (int i = 0; i < row; i++)
    {
        matrix->entries[i] = malloc(col * sizeof(double));
    }
    return matrix;
}

/**
 * @brief Fills the matrix with a constant value
 * 
 * @param m Pointer to the matrix
 * @param n The value to fill the matrix with
 */
void matrix_fill(Matrix *m, int n)
{
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            m->entries[i][j] = n;
        }
    }
}

/**
 * @brief Frees the memory allocated for a matrix
 * 
 * @param matrix Pointer to the matrix to be freed
 */
void matrix_free(Matrix *m)
{
    for (int i = 0; i < m->rows; i++)
    {
        free(m->entries[i]);
    }
    free(m->entries);
    free(m);
    m = NULL;
}

/**
 * @brief Prints the matrix to the standard output
 * 
 * @param m Pointer to the matrix
 */
void matrix_print(Matrix *m)
{
    printf("Rows: %d Columns: %d\n", m->rows, m->cols);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            printf("%1.3f ", m->entries[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Creates a copy of the matrix
 * 
 * @param m Pointer to the matrix to be copied
 * @return Matrix* Pointer to the newly created matrix, NULL on failure
 */
Matrix *matrix_copy(Matrix *m)
{
    Matrix *mat = matrix_create(m->rows, m->cols);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            mat->entries[i][j] = m->entries[i][j];
        }
    }
    return mat;
}

/**
 * @brief Saves the matrix to a file
 * 
 * @param m Pointer to the matrix
 * @param file_string The name of the file to save the matrix to
 */
void matrix_save(Matrix *m, char *file_string)
{
    FILE *file = fopen(file_string, "w");
    fprintf(file, "%d\n", m->rows);
    fprintf(file, "%d\n", m->cols);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            fprintf(file, "%.6f\n", m->entries[i][j]);
        }
    }
    printf("Successfully saved matrix to %s\n", file_string);
    fclose(file);
}

/**
 * @brief Loads a matrix from a file
 * 
 * @param file_string The name of the file to load the matrix from
 * @return Matrix* Pointer to the newly created matrix, NULL on failure
 */
Matrix *matrix_load(char *file_string)
{
    FILE *file = fopen(file_string, "r");
    char entry[MAXCHAR];
    fgets(entry, MAXCHAR, file);
    int rows = atoi(entry);
    fgets(entry, MAXCHAR, file);
    int cols = atoi(entry);
    Matrix *m = matrix_create(rows, cols);
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            fgets(entry, MAXCHAR, file);
            m->entries[i][j] = strtod(entry, NULL);
        }
    }
    printf("Sucessfully loaded matrix from %s\n", file_string);
    fclose(file);
    return m;
}

double uniform_distribution(double low, double high)
{
    double r = (double)rand() / (double)RAND_MAX; // valeur entre 0 et 1
    return low + r * (high - low);
}

/**
 * @brief Randomizes the entries of the matrix using a uniform distribution
 * 
 * @param m Pointer to the matrix
 * @param n The range of the distribution
 */
void matrix_randomize(Matrix *m, int n)
{
    double range = 1.0 / sqrt(n); // Xavier-like
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            m->entries[i][j] = uniform_distribution(-range, range);
        }
    }
}
int matrix_argmax(Matrix *m)
{
    // Expects a Mx1 matrix
    double max_score = 0;
    int max_idx = 0;
    for (int i = 0; i < m->rows; i++)
    {
        if (m->entries[i][0] > max_score)
        {
            max_score = m->entries[i][0];
            max_idx = i;
        }
    }
    return max_idx;
}

/**
 * @brief Flattens the matrix into a vector
 * 
 * @param m Pointer to the matrix
 * @param axis The axis to flatten the matrix along (0 = column vector, 1 = row vector)
 * @return Matrix* Pointer to the newly created vector, NULL on failure
 */
Matrix *matrix_flatten(Matrix *m, int axis)
{
    // Axis = 0 -> Column Vector, Axis = 1 -> Row Vector
    Matrix *mat;
    if (axis == 0)
    {
        mat = matrix_create(m->rows * m->cols, 1);
    }
    else if (axis == 1)
    {
        mat = matrix_create(1, m->rows * m->cols);
    }
    else
    {
        printf("Argument to matrix_flatten must be 0 or 1");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < m->rows; i++)
    {
        for (int j = 0; j < m->cols; j++)
        {
            if (axis == 0)
                mat->entries[i * m->cols + j][0] = m->entries[i][j];
            else if (axis == 1)
                mat->entries[0][i * m->cols + j] = m->entries[i][j];
        }
    }
    return mat;
}

/**
 * @brief Performs matrix multiplication
 * 
 * @param a First matrix (m x n)
 * @param b Second matrix (n x p)
 * @return Matrix* Result matrix (m x p), NULL on failure
 */
Matrix *matrix_multiply(Matrix *a, Matrix *b)
{
    if (a->cols != b->rows)
    {
        printf("Error: Incompatible matrix dimensions for multiplication\n");
        return NULL;
    }
    Matrix *result = matrix_create(a->rows, b->cols);
    for (int i = 0; i < a->rows; i++)
    {
        for (int j = 0; j < b->cols; j++)
        {
            result->entries[i][j] = 0;
            for (int k = 0; k < a->cols; k++)
            {
                result->entries[i][j] += a->entries[i][k] * b->entries[k][j];
            }
        }
    }
    return result;
}
