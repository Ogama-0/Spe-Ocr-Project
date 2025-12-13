#include "ocr.h"
#include "tools/ext_img.h"
#include "tools/img.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *IMAGE_PREDICT_TEST = "./dataset_test";
char *IMAGE_PREDICT = "../pretreatment/blocks/grid";
char *NETWORK = "./best_model";
char *INPUT_SOLVER = "input_solver";

int main() {
  srand(time(NULL));

  int count;
  ImgToPredict **imgs_to_test = load_imgs(IMAGE_PREDICT_TEST, &count);
  printf("dataset loaded succesfully :\ncount = %i\n", count);

  NeuralNetwork *net = network_load(NETWORK);
  printf("network load successfuly\n");

  network_predict_imgs_runtime(net, imgs_to_test, count);

  printf("prediction done.\n");

  img_save_predict(INPUT_SOLVER, imgs_to_test, count);
  printf("save imgs succsessfully :%s:", INPUT_SOLVER);

  imgsTo_free(imgs_to_test, count);
  network_free(net);
  return 0;
}
