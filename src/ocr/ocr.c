#include "../matrix/ops.h"
#include "glib.h"
#include "img.h"
#include "ocr.h"
#include "tools/activations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXCHAR 1000

// 729, 300, 10
NeuralNetwork *network_create(int input, int hidden, int output, double lr) {
  NeuralNetwork *net = malloc(sizeof(NeuralNetwork));
  net->input = input;
  net->hidden = hidden;
  net->output = output;
  net->lr = lr;
  Matrix *hidden_layer = matrix_create(hidden, input);
  Matrix *output_layer = matrix_create(output, hidden);
  matrix_randomize(hidden_layer, hidden);
  matrix_randomize(output_layer, output);
  net->hidden_weights = hidden_layer;
  net->output_weights = output_layer;
  return net;
}

void network_train(NeuralNetwork *net, Matrix *input, Matrix *output) {
  // Feed forward
  Matrix *hidden_inputs = dot(net->hidden_weights, input);
  Matrix *hidden_outputs = apply(sigmoid, hidden_inputs);
  Matrix *final_inputs = dot(net->output_weights, hidden_outputs);
  Matrix *final_outputs = apply(sigmoid, final_inputs);

  // Find errors
  Matrix *output_errors = subtract(output, final_outputs);
  Matrix *transposed_mat = transpose(net->output_weights);
  Matrix *hidden_errors = dot(transposed_mat, output_errors);
  matrix_free(transposed_mat);

  // Backpropogate
  Matrix *sigmoid_primed_mat = sigmoidPrime(final_outputs);
  Matrix *multiplied_mat = multiply(output_errors, sigmoid_primed_mat);
  transposed_mat = transpose(hidden_outputs);
  Matrix *dot_mat = dot(multiplied_mat, transposed_mat);
  Matrix *scaled_mat = scale(net->lr, dot_mat);
  Matrix *added_mat = add(net->output_weights, scaled_mat);

  matrix_free(net->output_weights); // Free the old weights before replacing
  net->output_weights = added_mat;

  matrix_free(sigmoid_primed_mat);
  matrix_free(multiplied_mat);
  matrix_free(transposed_mat);
  matrix_free(dot_mat);
  matrix_free(scaled_mat);

  // Reusing variables after freeing memory
  sigmoid_primed_mat = sigmoidPrime(hidden_outputs);
  multiplied_mat = multiply(hidden_errors, sigmoid_primed_mat);
  transposed_mat = transpose(input);
  dot_mat = dot(multiplied_mat, transposed_mat);
  scaled_mat = scale(net->lr, dot_mat);
  added_mat = add(net->hidden_weights, scaled_mat);
  matrix_free(
      net->hidden_weights); // Free the old hidden_weights before replacement
  net->hidden_weights = added_mat;

  matrix_free(sigmoid_primed_mat);
  matrix_free(multiplied_mat);
  matrix_free(transposed_mat);
  matrix_free(dot_mat);
  matrix_free(scaled_mat);

  matrix_free(hidden_inputs);
  matrix_free(hidden_outputs);
  matrix_free(final_inputs);
  matrix_free(final_outputs);
  matrix_free(output_errors);
  matrix_free(hidden_errors);
}
Matrix *network_predict(NeuralNetwork *net, Matrix *input_data) {
  Matrix *hidden_inputs = dot(net->hidden_weights, input_data);
  Matrix *hidden_outputs = apply(sigmoid, hidden_inputs);
  Matrix *final_inputs = dot(net->output_weights, hidden_outputs);
  Matrix *final_outputs = apply(sigmoid, final_inputs);

  matrix_free(hidden_inputs);
  matrix_free(hidden_outputs);
  matrix_free(final_inputs);

  return final_outputs;
}

void network_train_imgs(NeuralNetwork *net, Img **train_imgs, int train_count,
                        Img **val_imgs, int val_count, int epoch) {
  double best_accuracy = 0.0;
  int stagnant_epochs = 0;
  int max_stagnant = 10; // Stop after 10 epochs with no improvement

  for (int curr_epoch = 0; curr_epoch < epoch; curr_epoch++) {
    // Train on the training set
    for (int i = 0; i < train_count; i++) {
      Img *cur_img = train_imgs[i];
      Matrix *img_data = matrix_flatten(cur_img->img_data, 0);
      Matrix *output = matrix_create(net->output, 1);

      output->entries[cur_img->label][0] = 1; // Target output
      network_train(net, img_data, output);

      matrix_free(output);
      matrix_free(img_data);
    }

    // Evaluate on the validation set
    double val_accuracy = network_predict_imgs(net, val_imgs, val_count);
    double train_accuracy = network_predict_imgs(net, train_imgs, train_count);
    printf("Epoch %d - Train Accuracy: %.4f, Validation Accuracy: %.4f\n",
           curr_epoch + 1, train_accuracy, val_accuracy);

    // Early stopping logic based on validation accuracy
    if (val_accuracy > best_accuracy) {
      best_accuracy = val_accuracy;
      stagnant_epochs = 0;
      printf("New best validation accuracy! Saving network to 'best_model'.\n");
      network_save(net, "best_model"); // Save the best performing model
    } else {
      stagnant_epochs++;
    }

    if (stagnant_epochs >= max_stagnant) {
      printf("Validation accuracy has not improved for %d epochs. "
             "Stopping training.\n",
             max_stagnant);
      break;
    }
  }

  printf("Training finished. Best validation accuracy: %f\n", best_accuracy);
}

Matrix *network_predict_img(NeuralNetwork *net, Img *img) {
  Matrix *img_data = matrix_flatten(img->img_data, 0);
  Matrix *res = network_predict(net, img_data);
  matrix_free(img_data);
  return res;
}

Matrix *network_predict_img_runtime(NeuralNetwork *net, ImgToPredict *img) {
  Matrix *img_data = matrix_flatten(img->img_data, 0);
  Matrix *res = network_predict(net, img_data);
  matrix_free(img_data);
  return res;
}

void network_predict_imgs_runtime(NeuralNetwork *net, ImgToPredict **imgs,
                                  int n) {

  for (int i = 0; i < n; i++) {

    Matrix *prediction = network_predict_img_runtime(net, imgs[i]);
    printf("image (%i, %i) : \n", imgs[i]->grid_y, imgs[i]->grid_x);
    matrix_print(prediction);
    imgs[i]->label = matrix_argmax(prediction) + 'A';
  };
}

double network_predict_imgs(NeuralNetwork *net, Img **imgs, int n) {
  int n_correct = 0;
  for (int i = 0; i < n; i++) {
    Matrix *prediction = network_predict_img(net, imgs[i]);
    if (matrix_argmax(prediction) == imgs[i]->label) {
      n_correct++;
    }
    matrix_free(prediction);
  }
  return 1.0 * n_correct / n;
}

void network_save(NeuralNetwork *net, char *file_string) {
  char *curr_path = g_get_current_dir();
  mkdir(file_string, 0777);
  // Write the descriptor file
  chdir(file_string);
  FILE *descriptor = fopen("descriptor", "w");
  fprintf(descriptor, "%d\n", net->input);
  fprintf(descriptor, "%d\n", net->hidden);
  fprintf(descriptor, "%d\n", net->output);
  fclose(descriptor);
  matrix_save(net->hidden_weights, "hidden");
  matrix_save(net->output_weights, "output");
  printf("Successfully written to '%s'\n", file_string);
  chdir(curr_path); // Go back to the orignal directory
}

NeuralNetwork *network_load(char *file_string) {
  char *curr_path = g_get_current_dir();
  printf("curr path = %s\n", curr_path);
  printf("path = %s\n", file_string);
  NeuralNetwork *net = malloc(sizeof(NeuralNetwork));
  char entry[MAXCHAR];
  chdir(file_string);

  FILE *descriptor = fopen("descriptor", "r");
  fgets(entry, MAXCHAR, descriptor);
  net->input = atoi(entry);
  fgets(entry, MAXCHAR, descriptor);
  net->hidden = atoi(entry);
  fgets(entry, MAXCHAR, descriptor);
  net->output = atoi(entry);
  fclose(descriptor);
  net->hidden_weights = matrix_load("hidden");
  net->output_weights = matrix_load("output");
  printf("Successfully loaded network from '%s'\n", file_string);
  chdir(curr_path); // Go back to the original directory
  return net;
}

void network_print(NeuralNetwork *net) {
  printf("# of Inputs: %d\n", net->input);
  printf("# of Hidden: %d\n", net->hidden);
  printf("# of Output: %d\n", net->output);
  printf("Hidden Weights: \n");
  matrix_print(net->hidden_weights);
  printf("Output Weights: \n");
  matrix_print(net->output_weights);
}

void network_free(NeuralNetwork *net) {
  matrix_free(net->hidden_weights);
  matrix_free(net->output_weights);
  free(net);
  net = NULL;
}
/*

  */
void save_predict(Matrix *mat) {}

/**
 * @brief Initializes the OCR neural network
 * 
 * Loads the trained model weights and prepares the network for prediction
 * 
 * @return int 0 on success, -1 on failure
 */

/**
 * @brief Processes characters from preprocessed images
 * 
 * Iterates through all character images in the blocks directory,
 * performs character recognition, and builds the output grid
 * 
 * @return int 0 on success, -1 on failure
 */

/**
 * @brief Predicts a single character from an image
 * 
 * @param image_path Path to the character image file
 * @return char Predicted character, '\0' on failure
 */

/**
 * @brief Cleans up OCR resources
 * 
 * Frees all memory allocated for the neural network
 */
