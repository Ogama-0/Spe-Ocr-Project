#include "grayscale.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

unsigned char* surface_to_grayscale(SDL_Surface* surface)
{
    int w = surface->w;
    int h = surface->h;
    unsigned char* gray = malloc(w * h);
    if (!gray) return NULL;

    SDL_LockSurface(surface);
    Uint32* pixels = (Uint32*)surface->pixels;
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            Uint32 pixel = pixels[y * (surface->pitch / 4) + x];
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
            gray[y * w + x] = (Uint8)(0.299*r + 0.587*g + 0.114*b);
        }
    }
    SDL_UnlockSurface(surface);
    return gray;
}
