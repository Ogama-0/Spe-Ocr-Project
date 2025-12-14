// #include "ocr.h"
// #include "tools/img.h"
#include "train_ocr.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to shuffle the dataset to ensure random distribution for training
// and validation
// void shuffle_dataset(Img **dataset, int count)
// {
//     for (int i = count - 1; i > 0; i--)
//     {
//         int j = rand() % (i + 1);
//         Img *temp = dataset[i];
//         dataset[i] = dataset[j];
//         dataset[j] = temp;
//     }
// }

int main()
{
    srand(time(NULL));

    train_ocr();

    // int count;
    // Img **imgs = load_dataset("../dataset_processed", &count);
    // printf("dataset loaded count = %i\n", count);

    // // Shuffle the dataset for randomness
    // shuffle_dataset(imgs, count);
    // printf("Dataset shuffled.\n");

    // // Split the dataset into training (80%) and validation (20%) sets
    // int train_count = count * 0.8;
    // int val_count = count - train_count;
    // Img **train_imgs = imgs;
    // Img **val_imgs = imgs + train_count;

    // printf("Training set size: %d\n", train_count);
    // printf("Validation set size: %d\n", val_count);

    // NeuralNetwork *net = network_create(27 * 27, 300, 26, 0.01);

    // int epoch = 150;
    // // Train the network using the training set and validate it against the
    // // validation set
    // network_train_imgs(net, train_imgs, train_count, val_imgs, val_count,
    //                    epoch);

    // network_save(net, "run_time_net");

    // // Free the entire original dataset
    // imgs_free(imgs, count);

    // network_free(net);
    // return 0;
}
