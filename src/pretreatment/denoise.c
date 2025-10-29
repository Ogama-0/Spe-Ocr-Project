#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include "denoise.h"

#define WINDOW_SIZE 3

int compare_uint8(const void* a, const void* b)
{
    return (*(Uint8*)a - *(Uint8*)b);
}

SDL_Surface* median_filter(SDL_Surface* src)
{
    int w = src->w;
    int h = src->h;

    SDL_Surface* dst = SDL_CreateRGBSurface(0, w, h, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    Uint32* src_pixels = (Uint32*)src->pixels;
    Uint32* dst_pixels = (Uint32*)dst->pixels;

    int offset = WINDOW_SIZE / 2;
    Uint8 window[WINDOW_SIZE * WINDOW_SIZE];

    for (int y = offset; y < h - offset; y++)
    {
        for (int x = offset; x < w - offset; x++)
        {
            int k = 0;

            for (int j = -offset; j <= offset; j++)
            {
                for (int i = -offset; i <= offset; i++)
                {
                    Uint8 r, g, b;
                    SDL_GetRGB(src_pixels[(y + j) * w + (x + i)], src->format, &r, &g, &b);
                    window[k++] = r;
                }
            }

            qsort(window, WINDOW_SIZE * WINDOW_SIZE, sizeof(Uint8), compare_uint8);
            Uint8 median = window[(WINDOW_SIZE * WINDOW_SIZE) / 2];

            dst_pixels[y * w + x] = SDL_MapRGB(dst->format, median, median, median);
        }
    }

    return dst;
}
