#pragma once
#include "../matrix/matrix.h"
#include "tools/img.h"
typedef struct {
	int input;
	int hidden;
	int output;
	double lr;
	Matrix* hidden_weights;
	Matrix* output_weights;
} NeuralNetwork;

// typedef struct {
//   int a;
//   int b;
//   int answer;
// }Ans;

// TRAIN TIME
NeuralNetwork* network_create(int input, int hidden, int output, double lr);
void network_train(NeuralNetwork* net, Matrix* input_data, Matrix* output_data);
double network_predict_imgs(NeuralNetwork *net, Img **imgs, int n);
Matrix* network_predict(NeuralNetwork* net, Matrix* input_data);
int* network_predicts(NeuralNetwork* net, Img** lst_img, int n);
void network_train_imgs(NeuralNetwork* net, Img** train_imgs, int train_count, Img** val_imgs, int val_count, int epoch);

// RUN TIME
void network_predict_imgs_runtime(NeuralNetwork *net, ImgToPredict **imgs, int n);
void save_predict(Matrix* mat);

// OTHERS
void network_save(NeuralNetwork* net, char* file_string);
NeuralNetwork* network_load(char* file_string);
void network_print(NeuralNetwork* net);
void network_free(NeuralNetwork* net);

