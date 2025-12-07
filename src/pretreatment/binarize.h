#ifndef BINARIZE_H

#define BINARIZE_H

#include <SDL2/SDL.h>

void grayscale(SDL_Surface* surface);
void binarize(SDL_Surface* surface);
int is_black(Uint32 pixel);
void formatting_binarized(SDL_Surface* binarized_surface,
                            Uint8* res);
void print_formatted_bin(Uint8* arr,int width,int height);

#endif
