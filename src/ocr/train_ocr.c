#include "ocr.h"
#include "tools/img.h"
#include "train_ocr.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *PATH_DATASET = "../dataset";
int MAX_EPOCH = 150;
int HIDDEN = 200;
double LR = 0.01;

static void shuffle_dataset(Img **dataset, int count)
{
    for (int i = count - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Img *tmp = dataset[i];
        dataset[i] = dataset[j];
        dataset[j] = tmp;
    }
}

int train_ocr(void)
{
    srand(time(NULL));

    int count;
    Img **imgs = load_dataset(PATH_DATASET, &count);
    if (!imgs)
    {
        fprintf(stderr, "Failed to load dataset\n");
        return 1;
    }

    printf("dataset loaded count = %d\n", count);

    shuffle_dataset(imgs, count);

    int train_count = count * 0.8;
    int val_count = count - train_count;

    Img **train_imgs = imgs;
    Img **val_imgs = imgs + train_count;

    NeuralNetwork *net = network_create(27 * 27, HIDDEN, 26, LR);

    if (!net)
    {
        fprintf(stderr, "Failed to create network\n");
        imgs_free(imgs, count);
        return 1;
    }

    int epoch = MAX_EPOCH;
    network_train_imgs(net, train_imgs, train_count, val_imgs, val_count,
                       epoch);

    network_save(net, "network_saved");

    imgs_free(imgs, count);
    network_free(net);

    printf("OCR training finished.\n");
    return 0;
}
