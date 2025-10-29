#ifndef BLOCKS_TREATMENT_H
#define BLOCKS_TREATMENT_H

#include <SDL2/SDL.h>
#include <stdlib.h>

SDL_Rect* get_blocks(SDL_Surface* surface, Uint8* image, size_t* size);
void save_blocks(SDL_Surface* surface, SDL_Rect* blocks, size_t size);

#endif
