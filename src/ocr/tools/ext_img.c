#include "ext_img.h"
#include "img.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void imgTo_print(ImgToPredict *img) {
  matrix_print(img->img_data);
  printf("Img Label: %c\n", img->label + 'a');
}

void imgTo_free(ImgToPredict *img) {
  matrix_free(img->img_data);
  free(img);
  img = NULL;
}

void imgsTo_free(ImgToPredict **imgs, int n) {
  for (int i = 0; i < n; i++) {
    imgTo_free(imgs[i]);
  }
  free(imgs);
  imgs = NULL;
}

int imgsTo_get_row(ImgToPredict **imgs, size_t count) {
  int res = 0;
  for (size_t i = 0; i < count; i++) {
    if (imgs[i]->grid_x > res) {
      res = imgs[i]->grid_x;
    }
  }
  return res + 1;
}
int imgsTo_get_col(ImgToPredict **imgs, size_t count) {
  int res = 0;
  for (size_t i = 0; i < count; i++) {
    if (imgs[i]->grid_y > res) {
      res = imgs[i]->grid_y;
    }
  }
  return res + 1;
}

int_matrix *Imgs_To_Matrix(ImgToPredict **imgs, size_t count) {
  int row = imgsTo_get_row(imgs, count);
  int col = imgsTo_get_col(imgs, count);
  int_matrix *res = int_matrix_create(row, col);
  for (size_t i = 0; i < count; i++) {
    int y = imgs[i]->grid_y;
    int x = imgs[i]->grid_x;
    res->entries[y][x] = imgs[i]->label;
  }
  return res;
}

void img_save_predict(char *path, ImgToPredict **imgs, size_t count) {
  int_matrix *mat = Imgs_To_Matrix(imgs, count);
  printf("0\n");
  int_matrix_print(mat);
  int_matrix_save(mat, path);
  printf("1\n");
  int_matrix_free(mat);
  printf("2\n");
}
