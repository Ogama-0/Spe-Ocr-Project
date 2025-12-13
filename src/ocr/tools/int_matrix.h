
#pragma once
typedef struct {
  int row;
  int cols;
  int **entries;
} int_matrix;


int_matrix *int_matrix_create(int row, int col);
void int_matrix_fill(int_matrix *m, int n);
void int_matrix_free(int_matrix *m);
void int_matrix_print(int_matrix *m);
int_matrix *int_matrix_copy(int_matrix *m);

void int_matrix_save(int_matrix *mat, char *path);
int_matrix *int_matrix_load(int_matrix *file_string);
