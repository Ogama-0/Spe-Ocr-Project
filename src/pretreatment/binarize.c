#include "binarize.h"
#include "err.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void grayscale(SDL_Surface *surface)
{
    Uint32 *pixels = surface->pixels;
    int height = surface->h;
    int width = surface->w;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;

            SDL_GetRGBA(pixels[i * width + j], surface->format, &r, &g, &b, &a);

            Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);

            pixels[i * width + j] =
                SDL_MapRGBA(surface->format, gray, gray, gray, a);
        }
    }
}

int comp(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

int get_gray_mean(SDL_Surface *surface)
{

    Uint32 *pixels = surface->pixels;
    const int height = surface->h;
    const int width = surface->w;
    const size_t size = (size_t)(height * width);

    // int gray_list[size];
    int sum = 0;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;

            SDL_GetRGBA(pixels[i * width + j], surface->format, &r, &g, &b, &a);

            sum += r;
        }
    }

    /*
    qsort(gray_list,size,sizeof(int),comp);

    if (size % 2 == 0) {
      return (gray_list[size/2]+gray_list[size/2-1])/2;
    } else {
      return gray_list[size/2];
    }
    */

    size_t res = (size_t)sum / size;
    return (int)res;
}

void binarize(SDL_Surface *surface)
{
    Uint32 *pixels = surface->pixels;
    int height = surface->h;
    int width = surface->w;

    int mean = get_gray_mean(surface);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;

            SDL_GetRGBA(pixels[i * width + j], surface->format, &r, &g, &b, &a);

            Uint8 gray = r;

            if (gray >= mean - 25 || a < 128)
            {
                pixels[i * width + j] =
                    SDL_MapRGBA(surface->format, 255, 255, 255, 255);
            }
            else
            {
                pixels[i * width + j] =
                    SDL_MapRGBA(surface->format, 0, 0, 0, 255);
            }
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
void formatting_binarized(SDL_Surface *binarized_surface, Uint8 *res)
{
    int w = binarized_surface->w;
    int h = binarized_surface->h;
    Uint32 *p = binarized_surface->pixels;

    for (int i = 0; i < w * h; i++)
    {
        if (is_black(p[i]))
        {
            res[i] = 1;
        }
        else
        {
            res[i] = 0;
        }
    }
}

void print_formatted_bin(Uint8 *arr, int width, int height)
{
    FILE *f = fopen("grid_format.txt", "w");
    int fd = fileno(f);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int n = arr[i * width + j];
            if (n == 1)
            {
                write(fd, "1", 1);
            }
            else
            {
                write(fd, " ", 1);
            }
        }
        write(fd, "\n", 1);
    }
}
