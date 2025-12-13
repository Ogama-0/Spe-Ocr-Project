#include "int_matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int_matrix *int_matrix_create(int row, int col) {
  int_matrix *m = malloc(sizeof(int_matrix));
  if (!m)
    return NULL;

  m->row = row;
  m->cols = col;

  m->entries = calloc(row, sizeof(int *));
  if (!m->entries) {
    free(m);
    return NULL;
  }

  for (int i = 0; i < row; i++) {
    m->entries[i] = calloc(col, sizeof(int));
    if (!m->entries[i]) {
      while (--i >= 0)
        free(m->entries[i]);
      free(m->entries);
      free(m);
      return NULL;
    }
  }

  return m;
}

void int_matrix_fill(int_matrix *m, int n) {
  for (int i = 0; i < m->row; i++) {
    for (int j = 0; j < m->cols; j++) {
      m->entries[i][j] = n;
    }
  }
}

void int_matrix_free(int_matrix *m) {
  if (!m)
    return;

  for (int i = 0; i < m->row; i++) {
    free(m->entries[i]);
  }

  free(m->entries);
  free(m);
}

void int_matrix_print(int_matrix *m) {
  for (int i = 0; i < m->row; i++) {
    for (int j = 0; j < m->cols; j++) {

      printf("%c ", m->entries[i][j]);
    }
    printf("\n");
  }
}

int_matrix *int_matrix_copy(int_matrix *m) {
  if (!m)
    return NULL;

  int_matrix *copy = int_matrix_create(m->row, m->cols);
  if (!copy)
    return NULL;

  for (int i = 0; i < m->row; i++) {
    memcpy(copy->entries[i], m->entries[i], m->cols * sizeof(int));
  }

  return copy;
}

void int_matrix_save(int_matrix *mat, char *path) {
  FILE *f = fopen(path, "w");
  if (!f) {
    fprintf(stderr, "Error: cannot open file %s\n", path);
    return;
  }

  for (int i = 0; i < mat->row; i++) {
    for (int j = 0; j < mat->cols; j++) {
      char c = mat->entries[i][j]; // chaque entrée est un char
      fputc(c, f);
    }
    fputc('\n', f);
  }

  fclose(f);
}
