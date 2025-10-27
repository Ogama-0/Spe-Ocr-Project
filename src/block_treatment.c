#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"
#include "detect_grid.h"

// for a black pixel, check if it's an existing block.
// If not, create a new block.
void update_block(int x, int y, SDL_Rect** blocks,
                  size_t* size, size_t* capacity,
                  SDL_Surface* surface, int** visited, Uint8* image)
{
  int exists = 0;
  size_t i = 0;

  while (i < *size && exists == 0)
  {
    // check the dimensions to see if it is an existing block.
    if (y >= (*blocks)[i].y && y < (*blocks)[i].y + (*blocks)[i].h
    && x >= (*blocks)[i].x && x < (*blocks)[i].x + (*blocks)[i].w
    && visited[y][x] == 1)
    {
      exists = 1;
    }
    i++;
  }

  if (exists == 0)
  {
    if (*size+1 >= *capacity)
    {
      // modify the size of the blocks array if we exceed the
      // capacity.
      *blocks = realloc(*blocks, (*capacity)*2*sizeof(SDL_Rect));
      if (*blocks == NULL)
      {
        printf("error on reallocing\n");
      }
      (*capacity)*=2;
    }

    SDL_Rect b = get_block(surface,image,x,y,visited);
    
    (*blocks)[*size] = b;
    (*size)++;
  }
}

// get a block using a propagation algorithm, from a black pixel belonging
// to the block.
SDL_Rect get_block(SDL_Surface* surface,Uint8* image,int x,int y,int** visited)
{
  int w = surface->w;
  int h = surface->h;
  
  struct Point* stack = malloc(w*h*sizeof(struct Point));
  int top = 0;

  stack[top++] = (struct Point){x,y};
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

      if (nx >= 0 && nx < w && ny >= 0 && ny < h
      && image[ny*w+nx]
      && visited[ny][nx] == 0)
      {
        stack[top++] = (struct Point){nx,ny};
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
SDL_Rect* get_blocks(SDL_Surface* surface,Uint8* image,size_t* size)
{
  size_t capacity = 4;
  // create the array of blocks.
  SDL_Rect* blocks = malloc(capacity*sizeof(SDL_Rect));
  if (blocks == NULL)
  {
    printf("error on mallocing\n");
  }

  int height = surface->h;
  int width = surface->w;
  Uint32* pixels = surface->pixels;
  // create the array with the visited pixels.
  int** visited = calloc(height,sizeof(int*));
  if (visited == NULL)
  {
    printf("error on calloc\n");
  }
  for (int i = 0; i < height; i++)
  {
    visited[i] = calloc(width,sizeof(int));
    if (visited[i] == NULL)
    {
      printf("error on calloc for %i\n",i);
    }
  }

  for (int i = 0; i<height; i++)
  {
    for (int j = 0; j<width; j++)
    {
      if (image[i*width+j])
      {
        update_block(j,i,&blocks,size,&capacity,surface,visited,image);
      }
    }
  }

  for (int i = 0; i < height; i++)
  {
    free(visited[i]);
  }
  free(visited);

  return blocks;
}

// save each block into an image with bmp format.
void save_blocks(SDL_Surface* surface,SDL_Rect* blocks,size_t size)
{
  SDL_UnlockSurface(surface);
  for (size_t i = 0; i < size; i++)
  {
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(
      0,
      blocks[i].w,
      blocks[i].h,
      surface->format->BitsPerPixel,
      SDL_PIXELFORMAT_RGBA8888
    );

    printf("Bloc %zu: x=%d y=%d w=%d h=%d\n",i,blocks[i].x,blocks[i].y,
           blocks[i].w,blocks[i].h);

    if (!surf)
    {
      printf("error on surfacing block n°%zu: %s\n",i,SDL_GetError());
      return;
    }

    // fills the image with red if it doesn't work
    SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 0, 0));

    // fills the surface with a portion of the original surface.
    if (SDL_BlitSurface(surface,&blocks[i],surf,NULL) != 0)
    {
      printf("error on blitting surface: %s\n",SDL_GetError());
    }

    char filename[64];
    // save a name with a different index every time.
    snprintf(filename,sizeof(filename),"blocks/block_n%zu.bmp",i);
    SDL_SaveBMP(surf,filename);
    SDL_FreeSurface(surf);
  }
  SDL_LockSurface(surface);
}
