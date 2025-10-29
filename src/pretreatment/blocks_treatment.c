#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include "detect_grid.h"
#include "err.h"
#include "blocks_treatment.h"

struct Point {
    int x;
    int y;
};

SDL_Rect get_block(SDL_Surface* surface, Uint8* image, int x, int y, int** visited);

// for a black pixel, check if it's an existing block.
// If not, create a new block.
void update_block(int x, int y, SDL_Rect** blocks,
                  size_t* size, size_t* capacity,
                  SDL_Surface* surface, int** visited, Uint8* image)
{
    int exists = 0;
    size_t i = 0;

    while (i < *size && !exists)
    {
        if (y >= (*blocks)[i].y && y < (*blocks)[i].y + (*blocks)[i].h &&
            x >= (*blocks)[i].x && x < (*blocks)[i].x + (*blocks)[i].w &&
            visited[y][x] == 1)
        {
            exists = 1;
        }
        i++;
    }

    if (!exists)
    {
        if (*size + 1 >= *capacity)
        {
            *blocks = realloc(*blocks, (*capacity) * 2 * sizeof(SDL_Rect));
            if (!(*blocks))
            {
                fprintf(stderr, "Error reallocing blocks\n");
                exit(EXIT_FAILURE);
            }
            *capacity *= 2;
        }

        SDL_Rect b = get_block(surface, image, x, y, visited);
        (*blocks)[*size] = b;
        (*size)++;
    }
}

// get a block using a propagation algorithm, from a black pixel belonging
// to the block.
SDL_Rect get_block(SDL_Surface* surface, Uint8* image, int x, int y, int** visited)
{
    int w = surface->w;
    int h = surface->h;

    struct Point* stack = malloc(w * h * sizeof(struct Point));
    if (!stack)
    {
        fprintf(stderr, "Error allocating stack\n");
        exit(EXIT_FAILURE);
    }

    int top = 0;
    stack[top++] = (struct Point){x, y};
    visited[y][x] = 1;

    int min_x = x, max_x = x;
    int min_y = y, max_y = y;

    while (top > 0)
    {
        struct Point p = stack[--top];

        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_y) min_y = p.y;
        if (p.y > max_y) max_y = p.y;

        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++)
        {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];

            if (nx >= 0 && nx < w && ny >= 0 && ny < h &&
                image[ny * w + nx] &&
                visited[ny][nx] == 0)
            {
                stack[top++] = (struct Point){nx, ny};
                visited[ny][nx] = 1;
            }
        }
    }

    free(stack);

    SDL_Rect rect;
    rect.x = min_x;
    rect.y = min_y;
    rect.w = max_x - min_x + 1;
    rect.h = max_y - min_y + 1;
    return rect;
}

// for every black pixel encountered, call the update_block function.
SDL_Rect* get_blocks(SDL_Surface* surface, Uint8* image, size_t* size)
{
    size_t capacity = 4;
    SDL_Rect* blocks = malloc(capacity * sizeof(SDL_Rect));
    if (!blocks)
    {
        fprintf(stderr, "Error allocating blocks array\n");
        exit(EXIT_FAILURE);
    }

    int height = surface->h;
    int width = surface->w;

    int** visited = calloc(height, sizeof(int*));
    if (!visited)
    {
        fprintf(stderr, "Error allocating visited array\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < height; i++)
    {
        visited[i] = calloc(width, sizeof(int));
        if (!visited[i])
        {
            fprintf(stderr, "Error allocating visited row %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (image[y * width + x])
            {
                update_block(x, y, &blocks, size, &capacity, surface, visited, image);
            }
        }
    }

    for (int i = 0; i < height; i++) free(visited[i]);
    free(visited);

    return blocks;
}

// save each block into an image with bmp format.
void save_blocks(SDL_Surface* surface, SDL_Rect* blocks, size_t size)
{
    SDL_UnlockSurface(surface);
    for (size_t i = 0; i < size; i++)
    {
        SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(
            0, blocks[i].w, blocks[i].h,
            surface->format->BitsPerPixel,
            SDL_PIXELFORMAT_RGBA8888
        );

        if (!surf)
        {
            fprintf(stderr, "Error creating surface for block %zu\n", i);
            continue;
        }

        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 0, 0));
        if (SDL_BlitSurface(surface, &blocks[i], surf, NULL) != 0)
            fprintf(stderr, "Error blitting surface: %s\n", SDL_GetError());

        char filename[64];
        snprintf(filename, sizeof(filename), "blocks/block_n%zu.bmp", i);
        SDL_SaveBMP(surf, filename);
        SDL_FreeSurface(surf);
    }
    SDL_LockSurface(surface);
}
