#include "ocr-xor.h"
#include "../matrix/matrix.h"
#include "../matrix/ops.h"
#include "tools/activations.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXCHAR 1000

NeuralNetwork *network_create(double lr)
{
    int input = 2;
    int hidden = 2;
    int output = 1;
    NeuralNetwork *net = malloc(sizeof(NeuralNetwork));
    net->lr = lr;
    Matrix *hidden_layer = matrix_create(input, hidden);
    Matrix *output_layer = matrix_create(hidden, output);
    matrix_randomize(hidden_layer, hidden);
    matrix_randomize(output_layer, output);
    net->hidden_weights = hidden_layer;
    net->output_weights = output_layer;
    return net;
}
void network_train(NeuralNetwork *net, Matrix *input_data, Matrix *output_data)
{
    int epochs = 100000; // how many times the net will learn the dataset

    for (int epoch = 0; epoch < epochs; epoch++)
    {
        double total_error = 0.0;
        for (int i = 0; i < input_data->rows; i++)
        {
            // extrat data from the dataset
            Matrix *input = matrix_create(1, 2);
            input->entries[0][0] = input_data->entries[i][0];
            input->entries[0][1] = input_data->entries[i][1];

            Matrix *target = matrix_create(1, 1);
            target->entries[0][0] = output_data->entries[i][0];

            // forward pass
            Matrix *hidden = dot(input, net->hidden_weights);  // 1x2
            Matrix *hidden_activated = apply(sigmoid, hidden); // 1x2

            Matrix *output = dot(hidden_activated, net->output_weights); // 1x1
            Matrix *output_activated = apply(sigmoid, output);           // 1x1

            // errorr
            Matrix *error = subtract(target, output_activated); // 1x1
            total_error += fabs(error->entries[0][0]);

            Matrix *d_output =
                multiply(error, sigmoidPrime(output_activated)); // 1x1
            Matrix *output_weights_delta =
                dot(transpose(hidden_activated), d_output); // 2x1
            Matrix *scaled_output_delta = scale(net->lr, output_weights_delta);

            net->output_weights = add(net->output_weights, scaled_output_delta);

            Matrix *hidden_error =
                dot(d_output, transpose(net->output_weights)); // 1x2
            Matrix *d_hidden =
                multiply(hidden_error, sigmoidPrime(hidden_activated)); // 1x2
            Matrix *hidden_weights_delta =
                dot(transpose(input), d_hidden); // 2x2
            Matrix *scaled_hidden_delta = scale(net->lr, hidden_weights_delta);

            net->hidden_weights = add(net->hidden_weights, scaled_hidden_delta);

            matrix_free(input);
            matrix_free(target);
            matrix_free(hidden);
            matrix_free(hidden_activated);
            matrix_free(output);
            matrix_free(output_activated);
            matrix_free(error);
            matrix_free(d_output);
            matrix_free(output_weights_delta);
            matrix_free(scaled_output_delta);
            matrix_free(hidden_error);
            matrix_free(d_hidden);
            matrix_free(hidden_weights_delta);
            matrix_free(scaled_hidden_delta);
        }
        if (epoch % 10000 == 0)
        {
            printf("Epoch %d | Mean error: %.4f\n", epoch,
                   total_error / input_data->rows);
        }
    }
}

Matrix *network_predict(NeuralNetwork *net, Matrix *input_data)
{
    Matrix *hidden_raw = dot(input_data, net->hidden_weights);
    Matrix *hidden_output = apply(sigmoid, hidden_raw);
    Matrix *output_raw = dot(hidden_output, net->output_weights);
    Matrix *output = apply(sigmoid, output_raw);

    matrix_free(hidden_raw);
    matrix_free(hidden_output);
    matrix_free(output_raw);

    return output; // 1x1
}

int network_predict_xor(NeuralNetwork *net, Ans *ans)
{
    Matrix *input_data = matrix_create(1, 2);
    input_data->entries[0][0] = ans->a;
    input_data->entries[0][1] = ans->b;

    Matrix *res = network_predict(net, input_data);
    matrix_free(input_data);
    int prediction = res->entries[0][0] > 0.5 ? 1 : 0;
    matrix_free(res);
    return prediction;
}
int *network_predict_xors(NeuralNetwork *net, Ans **lst_ans, int n)
{
    int *res = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++)
    {
        res[i] = network_predict_xor(net, lst_ans[i]);
    }
    return res;
}

void network_save(NeuralNetwork *net, char *file_string)
{
    mkdir(file_string, 0777);
    // Write the descriptor file
    chdir(file_string);
    matrix_save(net->hidden_weights, "hidden");
    matrix_save(net->output_weights, "output");
    printf("Successfully written to '%s'\n", file_string);
    chdir("-"); // Go back to the orignal directory
}

NeuralNetwork *network_load(char *file_string)
{
    NeuralNetwork *net = malloc(sizeof(NeuralNetwork));
    char entry[MAXCHAR];
    chdir(file_string);

    net->hidden_weights = matrix_load("hidden");
    net->output_weights = matrix_load("output");
    printf("Successfully loaded network from '%s'\n", file_string);
    chdir("-"); // Go back to the original directory
    return net;
}

void network_print(NeuralNetwork *net)
{
    printf("Hidden Weights: \n");
    matrix_print(net->hidden_weights);
    printf("Output Weights: \n");
    matrix_print(net->output_weights);
}

void network_free(NeuralNetwork *net)
{
    matrix_free(net->hidden_weights);
    matrix_free(net->output_weights);
    free(net);
    net = NULL;
}
