#include <stdio.h>
#include <stdlib.h>
#include "ocr/ocr.h"
#include "pretreatment/preprocess.h"
#include "solver/solver.h"

/**
 * @brief Main entry point of the OCR application
 * 
 * This function orchestrates the complete OCR pipeline:
 * 1. Preprocesses the input image
 * 2. Performs character recognition
 * 3. Solves the word puzzle grid
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return int Exit status (0 for success, 1 for error)
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image_path>\n", argv[0]);
        return 1;
    }

    char *image_path = argv[1];

    if (preprocess_image(image_path) != 0) {
        fprintf(stderr, "Error: Preprocessing failed\n");
        return 1;
    }

    if (ocr_process() != 0) {
        fprintf(stderr, "Error: OCR processing failed\n");
        return 1;
    }

    if (solve_grid() != 0) {
        fprintf(stderr, "Error: Grid solving failed\n");
        return 1;
    }

    return 0;
}
