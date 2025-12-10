#include "ocr.h"
#include "tools/img.h"
#include "tools/resize_dataset.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
  srand(time(NULL));

  int count;
  Img **imgs = load_dataset("../dataset_processed", &count);

  printf("dataset loaded count = %i\n", count);
  NeuralNetwork *net = network_create(27 * 27, 300, 26, 0.1);

  int epoch = 200;
  network_train_imgs(net, imgs, count, epoch);

  network_save(net, "net_200-1.00-10_12");

  imgs_free(imgs, count);

  network_free(net);
  return 0;
}
