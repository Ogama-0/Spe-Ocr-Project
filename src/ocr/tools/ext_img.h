#pragma once

#include "img.h"
#include "int_matrix.h"

void img_save_predict(char *path, ImgToPredict **imgs, size_t count);
void img_save_words_predict(char *path, ImgToPredict **imgs, size_t count);
int_matrix *Imgs_To_Matrix(ImgToPredict **imgs, size_t count);

void imgTo_print(ImgToPredict* img);
void imgTo_free(ImgToPredict *img);
void imgsTo_free(ImgToPredict **imgs, int n);
