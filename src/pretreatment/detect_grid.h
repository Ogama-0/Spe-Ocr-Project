#ifndef DETECT_GRID_H
#define DETECT_GRID_H

#include <SDL2/SDL.h>

void grayscale(SDL_Surface* surface);
void binarize(SDL_Surface* surface);
int is_black(Uint32 pixel);
void formatting_binarized(SDL_Surface* binarized_surface, Uint8* res);

void detect_blocks_surface(SDL_Surface* surface);

#endif
