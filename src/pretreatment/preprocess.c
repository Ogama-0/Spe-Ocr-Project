#include "preprocess.h"
#include "blocks_treatment.h"
#include "manual_rotate.h"
#include "denoise.h"
#include "binarize.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h> // For chdir

int preprocess_image(const char *input_path, const char *output_path, double angle)
{
    GError* error = NULL;
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(input_path, &error);

    if (!pixbuf) {
        fprintf(stderr,"Error loading image: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // 1. Grayscale
    grayscale(pixbuf);

    // 2. Binarize
    binarize(pixbuf);

    // 3. Denoise
    GdkPixbuf* denoised_pixbuf = median_filter(pixbuf);
    g_object_unref(pixbuf); // free original pixbuf

    // 4. Rotate
    GdkPixbuf* rotated_pixbuf = rotate_pixbuf(denoised_pixbuf, angle);
    g_object_unref(denoised_pixbuf);

    // Save final processed image (optional, but good for debugging)
    if (!gdk_pixbuf_save(rotated_pixbuf, output_path, "png", &error, NULL)) {
        fprintf(stderr,"Error saving final processed image: %s\n", error->message);
        g_error_free(error);
    }

    // 5. Extract blocks
    char old_cwd[1024];
    if (getcwd(old_cwd, sizeof(old_cwd)) == NULL) {
        fprintf(stderr, "Error: Could not get current directory.\n");
        g_object_unref(rotated_pixbuf);
        return 1;
    }

    // The block saving functions assume a relative path, so we chdir
    if (chdir("../pretreatment") != 0) {
        fprintf(stderr, "Error: Could not switch to pretreatment directory.\n");
        g_object_unref(rotated_pixbuf);
        return 1;
    }
    
    struct stat st = {0};
    if (stat("blocks", &st) == -1) mkdir("blocks", 0700);
    if (stat("blocks/grid", &st) == -1) mkdir("blocks/grid", 0700);
    if (stat("blocks/words", &st) == -1) mkdir("blocks/words", 0700);

    int height = gdk_pixbuf_get_height(rotated_pixbuf);
    int width  = gdk_pixbuf_get_width(rotated_pixbuf);
    uint8_t* res = malloc(height * width * sizeof(uint8_t));
    if (!res) {
        fprintf(stderr, "Error: Block detection memory allocation failed.\n");
        g_object_unref(rotated_pixbuf);
        chdir(old_cwd);
        return 1;
    }
    
    formatting_binarized(rotated_pixbuf, res);

    size_t size_blocks = 0;
    size_t size_letters = 0;
    int m_height = 0;
    
    struct Rect* blocks = get_blocks(rotated_pixbuf, res, &size_blocks);
    struct Rect* letters = get_letter_blocks(blocks, size_blocks, &m_height, res, width, &size_letters);
    struct Rect* grid_blocks = save_blocks(letters, m_height, size_letters, rotated_pixbuf);
    
    if (grid_blocks)
    {
        // Do something with grid_blocks if needed, maybe return them?
        // For now, let's assume save_blocks did everything needed.
    }

    free(res);
    free(blocks);
    free(letters);
    // grid_blocks is a pointer into the 'letters' array, so it doesn't need separate freeing.

    g_object_unref(rotated_pixbuf);

    if (chdir(old_cwd) != 0) {
        fprintf(stderr, "Warning: Could not switch back to original directory.\n");
    }

    return 0;
}
