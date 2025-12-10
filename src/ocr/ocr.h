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

NeuralNetwork* network_create(int input, int hidden, int output, double lr);

void network_train(NeuralNetwork* net, Matrix* input_data, Matrix* output_data);

double network_predict_imgs(NeuralNetwork *net, Img **imgs, int n);
Matrix* network_predict(NeuralNetwork* net, Matrix* input_data);
int* network_predicts(NeuralNetwork* net, Img** lst_img, int n);
void network_train_imgs(NeuralNetwork* net, Img** imgs, int number_imgs, int epoch);

void network_save(NeuralNetwork* net, char* file_string);
NeuralNetwork* network_load(char* file_string);
void network_print(NeuralNetwork* net);
void network_free(NeuralNetwork* net);
