#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include "err.h"
#include "detect_grid.h"

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

int is_black(Uint32 pixel)
{
  if (pixel <= 255)
  {
    return 1;
  }
  return 0;
}

// transform the binarized image into an array of 0 and 1
void formatting_binarized(SDL_Surface* binarized_surface,
                            Uint8* res)
{
  int w = binarized_surface->w;
  int h = binarized_surface->h;
  Uint32* p = binarized_surface->pixels;

  for (int i = 0; i < w*h; i++)
  {
    if (is_black(p[i]))
    {
      res[i] = 1;
    }
    else
    {
      res[i] = 0;
    }
  }
}
