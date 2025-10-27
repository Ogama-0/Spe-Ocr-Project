#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "grayscale.h"
#include "manual_rotate.h"

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "%s\n", SDL_GetError()); return 1; }
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    SDL_Surface* image = IMG_Load("Mots-Mélés.jpg");
    if (!image) { fprintf(stderr, "IMG_Load error: %s\n", IMG_GetError()); return 1; }

    //Grayscale
    unsigned char* gray = surface_to_grayscale(image);

    SDL_Surface* gray_surface = SDL_CreateRGBSurfaceFrom(
        gray,
        image->w,
        image->h,
        8,
        image->w,
        0,0,0,0
    );

    SDL_SaveBMP(gray_surface, "grayscale.bmp");
    printf("Saved grayscale image as grayscale.bmp\n");

    //Rotation
    double angle;
    printf("Enter rotation angle (degrees): ");
    scanf("%lf", &angle);

    SDL_Surface* rotated = rotate_image(gray_surface, angle);
    if (rotated)
    {
        SDL_SaveBMP(rotated, "rotated.bmp");
        printf("Saved rotated image as rotated.bmp\n");
        SDL_FreeSurface(rotated);
    }

    SDL_FreeSurface(gray_surface);
    SDL_FreeSurface(image);
    free(gray);

    IMG_Quit();
    SDL_Quit();
    return 0;
}
