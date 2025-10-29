#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"
#include "detect_grid.h"
#include "blocks_treatment.h"

void detect_blocks_surface(SDL_Surface* image_format)
{
    int height = image_format->h;
    int width = image_format->w;

    Uint8* res = malloc(height * width * sizeof(Uint8));
    if (!res)
        errx(EXIT_FAILURE, "Erreur malloc pour res");

    formatting_binarized(image_format, res);

    size_t size_blocks = 0;
    SDL_Rect* blocks = get_blocks(image_format, res, &size_blocks);

    save_blocks(image_format, blocks, size_blocks);

    free(res);
    free(blocks);
}
