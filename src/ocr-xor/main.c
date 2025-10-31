#include "ocr-xor.h"
#include <stdio.h>
int main() {
  // we will creat a network that have 2 input (a and b)(0 or 1 eatch) and 1
  // output, (0 or 1) for false and true
  NeuralNetwork *net = network_create(0.1);

  Ans *dataset[4] = {&(Ans){0, 0, 0}, &(Ans){0, 1, 1}, &(Ans){1, 0, 1},
                     &(Ans){1, 1, 0}};

  Matrix *input_data = matrix_create(4, 2);
  Matrix *output_data = matrix_create(4, 1);

  for (int i = 0; i < 4; i++) {
    input_data->entries[i][0] = dataset[i]->a;
    input_data->entries[i][1] = dataset[i]->b;
    output_data->entries[i][0] = dataset[i]->answer;
  }

  printf("Training...\n");
  network_train(net, input_data, output_data);
  printf("Training complete.\n");

  for (int i = 0; i < 4; i++) {
    int prediction = network_predict_xor(net, dataset[i]);
    printf("Input: %d XOR %d → Predicted: %d | Expected: %d\n", dataset[i]->a,
           dataset[i]->b, prediction, dataset[i]->answer);
  }

  network_save(net, "trained_xor");
  network_free(net);
  matrix_free(input_data);
  matrix_free(output_data);

  return 0;
}
