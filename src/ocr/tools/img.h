
#pragma once
#include "../../matrix/matrix.h"
#include <stddef.h>

typedef struct {
	Matrix* img_data;
	int label;
} Img;

typedef struct {
	Matrix* img_data;
	char label;
	int grid_y;
	int grid_x;
} ImgToPredict;


Img** load_dataset(char* file_string, int *nb_img);
ImgToPredict **load_imgs(const char *path, int *out_count);

void img_print(Img* img);
void img_free(Img *img);
void imgs_free(Img **imgs, int n);

