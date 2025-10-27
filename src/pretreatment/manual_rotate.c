#include "manual_rotate.h"
#include <SDL2/SDL2_rotozoom.h>
#include <stdio.h>

SDL_Surface* rotate_image(SDL_Surface* src, double angle)
{
    SDL_Surface* rotated = rotozoomSurface(src, angle, 1.0, 1);
    if (!rotated)
    {
        fprintf(stderr, "Rotation failed: %s\n", SDL_GetError());
        return NULL;
    }
    return rotated;
}
