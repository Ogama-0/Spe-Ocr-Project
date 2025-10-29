#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "denoise.h"
#include "manual_rotate.h"
#include "binarize.h"

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
        fprintf(stderr, "Error IMG_Ini\n");
        SDL_Quit();
        return 1;
    }

    SDL_Surface* image = IMG_Load("Mots-Mélés.png");
    if (!image)
    {
        fprintf(stderr, "Erreur chargement images\n");
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(image);

    binarize(formatted);
    SDL_SaveBMP(formatted, "binarized.bmp");
    printf("binarized.bmp\n");

    SDL_Surface* denoised = median_filter(formatted);
    SDL_SaveBMP(denoised, "denoised.bmp");
    printf("denoised.bmp\n");

    double angle;
    printf("Entrez un angle de rotation : ");
    scanf("%lf", &angle);

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
