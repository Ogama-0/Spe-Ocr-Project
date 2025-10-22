#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"

// binarize the image (black and white)
void binarize(SDL_Surface* surface)
{
  // grab the surface's attributes
  Uint32* pixels = surface->pixels;
  int height = surface->h;
  int width = surface->w;

  // iterate through entire image
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      Uint8 r;
      Uint8 g;
      Uint8 b;
      Uint8 a;

      // get pixel components
      SDL_GetRGBA(pixels[i*width+j],
                 surface->format,&r,&g,&b,&a);

      Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);

      if (gray > 200)
      {
        // put pixel in white
        pixels[i*width+j] =
          SDL_MapRGBA(surface->format,255,255,255,a);
      }
      else
      {
        // put pixel in black
        pixels[i*width+j] =
          SDL_MapRGBA(surface->format,0,0,0,a);
      }
    }
  }
}

// main function to crate the surface
// and call every functions
void detect_grid(char* file, int n)
{
  SDL_Surface* load = IMG_Load(file);

  if (load == NULL)
  {
    errx(EXIT_FAILURE,"error on importing file: %s\n",SDL_GetError());
  }

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

  // int height = image_format->h;
  // int width = image_format->w;
  // Uint32* pixels = image_format->pixels;
  // SDL_PixelFormat* format = image_format->format;

  binarize(image_format);

  SDL_UnlockSurface(image_format);

  SDL_SaveBMP(image_format, "binarized.bmp");
  SDL_FreeSurface(image_format);
  SDL_FreeSurface(load);
}

// test function
int main() { }
