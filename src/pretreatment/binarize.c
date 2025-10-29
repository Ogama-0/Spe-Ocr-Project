#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "detect_grid.h"

void grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int height = surface->h;
    int width = surface->w;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixels[i * width + j], surface->format, &r, &g, &b, &a);
            Uint8 gray = (Uint8)(0.3*r + 0.59*g + 0.11*b);
            pixels[i * width + j] = SDL_MapRGBA(surface->format, gray, gray, gray, a);
        }
    }
}

void binarize(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int height = surface->h;
    int width = surface->w;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixels[i * width + j], surface->format, &r, &g, &b, &a);
            Uint8 gray = r;

            if (gray > 200)
                pixels[i * width + j] = SDL_MapRGBA(surface->format, 255, 255, 255, a);
            else
                pixels[i * width + j] = SDL_MapRGBA(surface->format, 0, 0, 0, a);
        }
    }
}

int is_black(Uint32 pixel)
{
    if (pixel <= 255)
    {
        return 1;
    }

    return 0;
}

// transform the binarized image into an array of 0 and 1
void formatting_binarized(SDL_Surface* binarized_surface, Uint8* res)
{
    int w = binarized_surface->w;
    int h = binarized_surface->h;
    Uint32* p = binarized_surface->pixels;

    for (int i = 0; i < w*h; i++)
        res[i] = is_black(p[i]);
}
