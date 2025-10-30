#include "matrix.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXCHAR 100

Matrix *matrix_create(int row, int col) {
  Matrix *matrix = malloc(sizeof(Matrix));
  matrix->rows = row;
  matrix->cols = col;
  matrix->entries = malloc(row * sizeof(double *));
  for (int i = 0; i < row; i++) {
    matrix->entries[i] = malloc(col * sizeof(double));
  }
  return matrix;
}

void matrix_fill(Matrix *m, int n) {
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      m->entries[i][j] = n;
    }
  }
}

void matrix_free(Matrix *m) {
  for (int i = 0; i < m->rows; i++) {
    free(m->entries[i]);
  }
  free(m->entries);
  free(m);
  m = NULL;
}

void matrix_print(Matrix *m) {
  printf("Rows: %d Columns: %d\n", m->rows, m->cols);
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      printf("%1.3f ", m->entries[i][j]);
    }
    printf("\n");
  }
}

Matrix *matrix_copy(Matrix *m) {
  Matrix *mat = matrix_create(m->rows, m->cols);
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      mat->entries[i][j] = m->entries[i][j];
    }
  }
  return mat;
}

void matrix_save(Matrix *m, char *file_string) {
  FILE *file = fopen(file_string, "w");
  fprintf(file, "%d\n", m->rows);
  fprintf(file, "%d\n", m->cols);
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      fprintf(file, "%.6f\n", m->entries[i][j]);
    }
  }
  printf("Successfully saved matrix to %s\n", file_string);
  fclose(file);
}

Matrix *matrix_load(char *file_string) {
  FILE *file = fopen(file_string, "r");
  char entry[MAXCHAR];
  fgets(entry, MAXCHAR, file);
  int rows = atoi(entry);
  fgets(entry, MAXCHAR, file);
  int cols = atoi(entry);
  Matrix *m = matrix_create(rows, cols);
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      fgets(entry, MAXCHAR, file);
      m->entries[i][j] = strtod(entry, NULL);
    }
  }
  printf("Sucessfully loaded matrix from %s\n", file_string);
  fclose(file);
  return m;
}

double uniform_distribution(double low, double high) {
  double r = (double)rand() / (double)RAND_MAX; // valeur entre 0 et 1
  return low + r * (high - low);
}

void matrix_randomize(Matrix *m, int n) {
  double range = 1.0 / sqrt(n); // Xavier-like
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      m->entries[i][j] = uniform_distribution(-range, range);
    }
  }
}
int matrix_argmax(Matrix *m) {
  // Expects a Mx1 matrix
  double max_score = 0;
  int max_idx = 0;
  for (int i = 0; i < m->rows; i++) {
    if (m->entries[i][0] > max_score) {
      max_score = m->entries[i][0];
      max_idx = i;
    }
  }
  return max_idx;
}

Matrix *matrix_flatten(Matrix *m, int axis) {
  // Axis = 0 -> Column Vector, Axis = 1 -> Row Vector
  Matrix *mat;
  if (axis == 0) {
    mat = matrix_create(m->rows * m->cols, 1);
  } else if (axis == 1) {
    mat = matrix_create(1, m->rows * m->cols);
  } else {
    printf("Argument to matrix_flatten must be 0 or 1");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < m->rows; i++) {
    for (int j = 0; j < m->cols; j++) {
      if (axis == 0)
        mat->entries[i * m->cols + j][0] = m->entries[i][j];
      else if (axis == 1)
        mat->entries[0][i * m->cols + j] = m->entries[i][j];
    }
  }
  return mat;
}
