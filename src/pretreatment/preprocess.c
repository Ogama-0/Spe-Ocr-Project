#include "preprocess.h"
#include "blocks_treatment.h"
#include "manual_rotate.h"
#include "denoise.h"
#include "binarize.h"
#include <SDL2/SDL.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

int preprocess_image(const char *input_path, const char *output_path, double angle)
{
    GError* error = NULL;
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(input_path, &error);

    if (!pixbuf) {
        fprintf(stderr,"Error loading image: %s\n", error->message);
        g_error_free(error);
        return 1;
    }

    // Crée le dossier images si nécessaire
    struct stat st = {0};
    if (stat("images", &st) == -1)
        mkdir("images", 0700);

    if (!gdk_pixbuf_save(pixbuf, "images/base_image.bmp", "bmp", &error, NULL)) {
        fprintf(stderr,"Error saving base BMP: %s\n", error->message);
        g_error_free(error);
        g_object_unref(pixbuf);
        return 1;
    }

    g_object_unref(pixbuf);

    SDL_Surface* load = SDL_LoadBMP("images/base_image.bmp");
    if (!load) {
        fprintf(stderr,"SDL_LoadBMP failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(load, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(load);

    // Grayscale
    grayscale(formatted);
    SDL_SaveBMP(formatted,"images/grayscale.bmp");

    // Binarize
    binarize(formatted);
    SDL_SaveBMP(formatted,"images/binarized.bmp");

    // Denoise
    SDL_Surface* denoised = median_filter(formatted);
    SDL_SaveBMP(denoised,"images/denoised.bmp");

    // Rotate
    GError* error2 = NULL;
    GdkPixbuf* pixbuf2 = gdk_pixbuf_new_from_file("images/denoised.bmp",&error2);
    if (!pixbuf2) {
        fprintf(stderr,"Error loading denoised BMP: %s\n", error2->message);
        g_error_free(error2);
        SDL_FreeSurface(formatted);
        SDL_FreeSurface(denoised);
        return 1;
    }

    pixbuf2 = rotate_pixbuf(pixbuf2, angle);

    if (!gdk_pixbuf_save(pixbuf2, output_path, "bmp", &error2, NULL)) {
        fprintf(stderr,"Error saving rotated BMP: %s\n", error2->message);
        g_error_free(error2);
        g_object_unref(pixbuf2);
        SDL_FreeSurface(formatted);
        SDL_FreeSurface(denoised);
        return 1;
    }

    g_object_unref(pixbuf2);

    // Extract blocks
    SDL_Surface* rotated = SDL_LoadBMP(output_path);
    if (!rotated) {
        fprintf(stderr,"SDL_LoadBMP failed for rotated BMP: %s\n", SDL_GetError());
        SDL_FreeSurface(formatted);
        SDL_FreeSurface(denoised);
        return 1;
    }

    SDL_Surface* rotated_formatted = SDL_ConvertSurfaceFormat(rotated, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(rotated);

    int height = rotated_formatted->h;
    int width  = rotated_formatted->w;
    Uint8 res[height*width];
    formatting_binarized(rotated_formatted,res);

    size_t size_blocks = 0;
    SDL_Rect* blocks = get_blocks(rotated_formatted,res,&size_blocks);

    save_blocks(rotated_formatted,blocks,size_blocks);

    free(blocks);
    SDL_FreeSurface(formatted);
    SDL_FreeSurface(denoised);
    SDL_FreeSurface(rotated_formatted);

    return 0;
}
