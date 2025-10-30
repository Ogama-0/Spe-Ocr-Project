#pragma once
#include "../matrix/matrix.h"
typedef struct {
	double lr;
	Matrix* hidden_weights;
	Matrix* output_weights;
	Matrix *hidden_bias;
	Matrix *output_bias;    
} NeuralNetwork;

typedef struct {
  int a;
  int b;
  int answer;
}Ans;

NeuralNetwork* network_create(double lr);
void network_train(NeuralNetwork* net, Matrix* input_data, Matrix* output_data);
int network_predict_xor(NeuralNetwork* net, Ans* ans);
int* network_predict_xors(NeuralNetwork* net, Ans** lst_ans, int n);
void network_save(NeuralNetwork* net, char* file_string);
NeuralNetwork* network_load(char* file_string);
void network_print(NeuralNetwork* net);
void network_free(NeuralNetwork* net);
