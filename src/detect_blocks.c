#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"
#include "detect_grid.h"

// main function to call every other ones.
void detect_blocks(char* file)
{
  SDL_Surface* load = IMG_Load(file);

  if (load == NULL)
  {
    errx(EXIT_FAILURE,"error on importing file: %s\n",SDL_GetError());
  }

  // Convert the format to RGBA8888
  SDL_Surface* image_format = SDL_ConvertSurfaceFormat(load,
    SDL_PIXELFORMAT_RGBA8888,0);

  if (image_format == NULL)
  {
    errx(EXIT_FAILURE,"error on formatting image: %s\n",SDL_GetError());
  }

  if (SDL_LockSurface(image_format) < 0)
  {
    errx(EXIT_FAILURE,"error on locking surface: %s\n",SDL_GetError());
  }

  int height = image_format->h;
  int width = image_format->w;
  // Uint32* pixels = image_format->pixels;
  // SDL_PixelFormat* format = image_format->format;

  binarize(image_format);

  Uint8 res[height*width];
  formatting_binarized(image_format,res);

  size_t size_blocks = 0;
  SDL_Rect* blocks = get_blocks(image_format,res,&size_blocks);
  printf("%zu\n",size_blocks);

  save_blocks(image_format,blocks,size_blocks);

  free(blocks);

  SDL_UnlockSurface(image_format);

  SDL_SaveBMP(image_format, "binarized.bmp");
  SDL_FreeSurface(image_format);
  SDL_FreeSurface(load);
}

int main(int argc, char* argv[])
{
  detect_blocks(argv[1]);
}
