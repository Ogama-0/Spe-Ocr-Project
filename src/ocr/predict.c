
#include "ocr.h"
#include "tools/ext_img.h"
#include "tools/img.h"
#include <stdio.h>

char *GRID_IMAGE_PREDICT =
    "../pretreatment/blocks/grid"; //   ou il vas chercher la grid
char *WORDS_IMAGE_PREDICT =
    "../pretreatment/blocks/words"; // ou il vas chercher les mots (en mode sur
                                    // le cote)

char *NETWORK = "./network_saved"; // ou il vas chercher le nw
char *GRID_INPUT_SOLVER =
    "input_solver/grid"; // ou il vas mettre le fichier grid input du solver
char *WORDS_INPUT_SOLVER =
    "input_solver/words"; // ou il vas mettre le fichier words input du solver

void predict()
{

    int grid_count;
    ImgToPredict **grid_imgs_to_test =
        load_imgs(GRID_IMAGE_PREDICT, &grid_count);
    printf("dataset loaded succesfully :\ncount = %i\n", grid_count);

    int words_count;
    ImgToPredict **words_imgs_to_test =
        load_imgs(WORDS_IMAGE_PREDICT, &words_count);

    NeuralNetwork *net = network_load(NETWORK);
    printf("network load successfuly\n");

    network_predict_imgs_runtime(net, grid_imgs_to_test, grid_count);
    network_predict_imgs_runtime(net, words_imgs_to_test, words_count);

    printf("prediction done.\n");

    printf("\tgrid :\n\n");
    for (int i = 0; i < grid_count; i++)
    {
        printf("img (%i,%i):%c\n", grid_imgs_to_test[i]->grid_y,
               grid_imgs_to_test[i]->grid_x, grid_imgs_to_test[i]->label);
    }
    printf("\twords :\n\n");
    for (int i = 0; i < words_count; i++)
    {
        printf("img (%i,%i):%c\n", words_imgs_to_test[i]->grid_y,
               words_imgs_to_test[i]->grid_x, words_imgs_to_test[i]->label);
    }

    img_save_predict(GRID_INPUT_SOLVER, grid_imgs_to_test, grid_count);
    printf("save imgs succsessfully : %s\n", GRID_INPUT_SOLVER);

    img_save_words_predict(WORDS_INPUT_SOLVER, words_imgs_to_test, words_count);
    printf("save imgs succsessfully :%s\n", WORDS_INPUT_SOLVER);

    imgsTo_free(grid_imgs_to_test, grid_count);
    imgsTo_free(words_imgs_to_test, words_count);
    network_free(net);
}

// int main()
// {
//     srand(time(NULL));
//     predict();

//     return 0;
// }
