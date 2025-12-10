
#pragma once
#include "../../matrix/matrix.h"

typedef struct {
	Matrix* img_data;
	char label;
} Img;

Img** load_dataset(char* file_string, int *nb_img);
// Img **load_imgs(const char *path, int *out_count);
void img_print(Img* img);
void img_free(Img *img);
void imgs_free(Img **imgs, int n);
