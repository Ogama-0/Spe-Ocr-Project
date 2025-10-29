#ifndef BINARIZE_H
#define BINARIZE_H

#include <SDL2/SDL.h>

void grayscale(SDL_Surface* surface);
void binarize(SDL_Surface* surface);
void formatting_binarized(SDL_Surface* binarized_surface, Uint8* res);

#endif
