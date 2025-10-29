#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "denoise.h"
#include "manual_rotate.h"
#include "detect_grid.h"

#define MAX_FILENAME 256

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error Init\n");
        return 1;
    }

    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        fprintf(stderr, "Error IMG_Init\n");
        SDL_Quit();
        return 1;
    }

    char filename[MAX_FILENAME];
    printf("Enter the name of the image to process (with extension): ");
    if (scanf("%255s", filename) != 1)
    {
        fprintf(stderr, "Failed to read filename.\n");
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* image = IMG_Load(filename);
    if (!image)
    {
        fprintf(stderr, "Failed to load image\n");
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(image);

    struct stat st = {0};
    if (stat("blocks", &st) == -1)
        mkdir("blocks", 0700);

    binarize(formatted);
    SDL_SaveBMP(formatted, "binarized.bmp");
    printf("binarized.bmp\n");

    detect_blocks_surface(formatted);
    printf("Blocks saved in 'blocks/' folder.\n");

    SDL_Surface* denoised = median_filter(formatted);
    SDL_SaveBMP(denoised, "denoised.bmp");
    printf("denoised.bmp\n");

    double angle;
    printf("Enter rotation angle (degrees): ");
    if (scanf("%lf", &angle) != 1)
    {
        fprintf(stderr, "Failed to read angle.\n");
        SDL_FreeSurface(formatted);
        SDL_FreeSurface(denoised);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* rotated = rotate_image(denoised, angle);
    SDL_SaveBMP(rotated, "rotated_manual.bmp");
    printf("rotated_manual.bmp\n");

    SDL_FreeSurface(formatted);
    SDL_FreeSurface(denoised);
    SDL_FreeSurface(rotated);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
