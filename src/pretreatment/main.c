#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdint.h>
#include "manual_rotate.h"
#include "denoise.h"
#include "blocks_treatment.h"
#include "err.h"
#include "binarize.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../solver/solver.h"

// main function to call every other ones.
int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(argv[1], &error);

    if (pixbuf == NULL)
    {
        fprintf(stderr, "Error on loading from png: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    if (!gdk_pixbuf_save(pixbuf, "images/base_image.bmp", "bmp", &error, NULL))
    {
        fprintf(stderr, "Error on saving base image to bmp: %s\n",
                error->message);
        g_error_free(error);
        g_object_unref(pixbuf);
        return 1;
    }

    struct stat st = {0};
    if (stat("blocks", &st) == -1)
        mkdir("blocks", 0700);

    // grayscale

    grayscale(pixbuf);
    if (!gdk_pixbuf_save(pixbuf, "images/grayscale.bmp", "bmp", &error, NULL))
    {
        fprintf(stderr, "Error on saving base image to bmp: %s\n",
                error->message);
        g_error_free(error);
        g_object_unref(pixbuf);
        return 1;
    }
    printf("grayscale.bmp\n");

    // denoise

    GError* error2 = NULL;
    GdkPixbuf* denoised = median_filter(pixbuf);
    if (!gdk_pixbuf_save(denoised, "images/denoised.bmp", "bmp", &error2, NULL))
    {
        fprintf(stderr, "Error on saving base image to bmp: %s\n",
                error2->message);
        g_error_free(error2);
        g_object_unref(pixbuf);
        g_object_unref(denoised);
        return 1;
    }
    printf("denoised.bmp\n");

    // binarize

    GError* error3 = NULL;
    binarize(denoised);
    if (!gdk_pixbuf_save(denoised, "images/binarized.bmp", "bmp", &error3, NULL))
    {
        fprintf(stderr, "Error on saving base image to bmp: %s\n",
                error3->message);
        g_error_free(error3);
        g_object_unref(pixbuf);
        g_object_unref(denoised);
        return 1;
    }
    printf("binarized.bmp\n");

    // rotate image

    /*
    double angle= 0;

    printf("Enter rotation angle (degrees): ");
    if (scanf("%lf", &angle) != 1)
    {
        fprintf(stderr, "Failed to read angle.\n");
        SDL_FreeSurface(formatted);
        SDL_FreeSurface(denoised);
        return 1;
    }
    */

    GError* error4 = NULL;
    GdkPixbuf *rotated = automatic_rotate(denoised);
    binarize(rotated);

    if (!gdk_pixbuf_save(rotated, "images/rotated.bmp", "bmp", &error4, NULL))
    {
        fprintf(stderr, "Error on saving base image to bmp: %s\n",
                error4->message);
        g_error_free(error4);
        g_object_unref(pixbuf);
        g_object_unref(denoised);
        g_object_unref(rotated);
        return 1;
    }

    printf("rotated.bmp\n");

    // get blocks

    int height = gdk_pixbuf_get_height(rotated);
    int width = gdk_pixbuf_get_width(rotated);

    uint8_t res[height * width];
    formatting_binarized(rotated, res);

    // print_formatted_bin(res,width,height);

    /*
    GdkPixbuf *cpy_of_rot = gdk_pixbuf_new_from_data(
        gdk_pixbuf_get_pixels(rotated),
        GDK_COLORSPACE_RGB,
        TRUE,
        8,
        width, height,
        gdk_pixbuf_get_rowstride(rotated),
        destroy_notify, rotated
    );
    */

    size_t size_blocks = 0;
    size_t size_letters = 0;
    int m_height = 0;
    
    struct Rect *blocks = get_blocks(rotated, res, &size_blocks);
    struct Rect *letters = get_letter_blocks(blocks, size_blocks, &m_height, res,
                                          width, &size_letters);

    // printf("nb of letters in main: %zu\n", size_letters);

    struct Rect* grid_blocks = save_blocks(letters, m_height, size_letters,
                rotated);

    line_pos(grid_blocks);

    printf("Blocks saved in 'blocks/' folder.\n");

    free(blocks);
    free(letters);

    g_object_unref(rotated);
    g_object_unref(denoised);
    g_object_unref(pixbuf);
    
    return 0;
}
