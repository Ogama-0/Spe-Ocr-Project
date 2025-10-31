#include <SDL2/SDL.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "err.h"
#include "blocks_treatment.h"
#include "manual_rotate.h"
#include "denoise.h"
#include "binarize.h"

// main function to call every other ones.
int main(int argc, char* argv[])
{
  if (argc != 2) return 1;
  
  GError* error = NULL;
  GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(argv[1],&error);
  
  if (pixbuf == NULL) {
    fprintf(stderr,"Error on loading from png: %s\n",error->message);
    g_error_free(error);
    return 1;
  }

  if (!gdk_pixbuf_save(pixbuf,"images/base_image.bmp","bmp",&error,NULL)) {
    fprintf(stderr,"Error on saving base image to bmp: %s\n",error->message);
    g_error_free(error);
    g_object_unref(pixbuf);
    return 1;
  }

  g_object_unref(pixbuf);
  
  SDL_Surface* load = SDL_LoadBMP("images/base_image.bmp");
  
  if (load == NULL) {
    fprintf(stderr,"error on loading bmp: %s\n",SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_Surface* formatted = SDL_ConvertSurfaceFormat(load, SDL_PIXELFORMAT_RGBA8888, 0);
  SDL_FreeSurface(load);

  struct stat st = {0};
  if (stat("blocks", &st) == -1)
      mkdir("blocks", 0700);

  grayscale(formatted);
  SDL_SaveBMP(formatted,"images/grayscale.bmp");
  printf("grayscale.bmp\n");

  binarize(formatted);
  SDL_SaveBMP(formatted, "images/binarized.bmp");
  printf("binarized.bmp\n");

  int height = formatted->h;
  int width = formatted->w;

  Uint8 res[height*width];
  formatting_binarized(formatted,res);

  size_t size_blocks = 0;
  SDL_Rect* blocks = get_blocks(formatted,res,&size_blocks);

  save_blocks(formatted,blocks,size_blocks);
  printf("Blocks saved in 'blocks/' folder.\n");

  free(blocks);

  SDL_Surface* denoised = median_filter(formatted);
  SDL_SaveBMP(denoised, "images/denoised.bmp");
  printf("denoised.bmp\n");

  double angle;

  printf("Enter rotation angle (degrees): ");
  if (scanf("%lf", &angle) != 1)
  {
      fprintf(stderr, "Failed to read angle.\n");
      SDL_FreeSurface(formatted);
      SDL_FreeSurface(denoised);
      return 1;
  }

  SDL_Surface* rotated = rotate_image(denoised, angle);
  SDL_SaveBMP(rotated, "images/rotated_manual.bmp");
  printf("rotated_manual.bmp\n");

  SDL_FreeSurface(formatted);
  SDL_FreeSurface(denoised);
  SDL_FreeSurface(rotated);

  return 0;
  // if (SDL_LockSurface(image_format) < 0) {
    // errx(EXIT_FAILURE,"error on locking surface: %s\n",SDL_GetError());
  // }

  // SDL_UnlockSurface(image_format);
}
