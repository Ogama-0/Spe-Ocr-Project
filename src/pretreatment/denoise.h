#ifndef DENOISE_H
#define DENOISE_H

#include <SDL2/SDL.h>

#define WINDOW_SIZE 3

int compare_uint8(const void* a, const void* b);
SDL_Surface* median_filter(SDL_Surface* src);

#endif
