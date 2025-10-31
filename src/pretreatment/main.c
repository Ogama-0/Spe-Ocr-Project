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
    return 1;
  }

  SDL_Surface* formatted = SDL_ConvertSurfaceFormat(load,
	SDL_PIXELFORMAT_RGBA8888, 0);
  SDL_FreeSurface(load);

  struct stat st = {0};
  if (stat("blocks", &st) == -1)
      mkdir("blocks", 0700);

  // grayscale

  grayscale(formatted);
  SDL_SaveBMP(formatted,"images/grayscale.bmp");
  printf("grayscale.bmp\n");

  // binarize

  binarize(formatted);
  SDL_SaveBMP(formatted, "images/binarized.bmp");
  printf("binarized.bmp\n");

  // denoise

  SDL_Surface* denoised = median_filter(formatted);
  SDL_SaveBMP(denoised, "images/denoised.bmp");
  printf("denoised.bmp\n");

  // rotate image

  double angle;

  printf("Enter rotation angle (degrees): ");
  if (scanf("%lf", &angle) != 1)
  {
      fprintf(stderr, "Failed to read angle.\n");
      SDL_FreeSurface(formatted);
      SDL_FreeSurface(denoised);
      return 1;
  }

  GError* error2 = NULL;
  GdkPixbuf* pixbuf2 = gdk_pixbuf_new_from_file("images/denoised.bmp",&error);

  if (pixbuf2 == NULL) {
    fprintf(stderr,"Error on loading from png: %s\n",error2->message);
    g_error_free(error2);
    return 1;
  }

  pixbuf2 = rotate_pixbuf(pixbuf2, angle);
  
  if (!gdk_pixbuf_save(pixbuf2,"images/rotated_manual.bmp","bmp",&error2,NULL)) {
    fprintf(stderr,"Error on saving base image to bmp: %s\n",error2->message);
    g_error_free(error2);
    g_object_unref(pixbuf2);
    return 1;
  }

  g_object_unref(pixbuf2);

  printf("rotated_manual.bmp\n");

  // get blocks

  SDL_Surface* rotated = SDL_LoadBMP("images/rotated_manual.bmp");

  if (rotated == NULL) {
    fprintf(stderr,"Error on loading rotated image: %s\n",SDL_GetError());
    return 1;
  }

  SDL_Surface* rotated_formatted = SDL_ConvertSurfaceFormat(rotated,
                                      SDL_PIXELFORMAT_RGBA8888,0);
  SDL_FreeSurface(rotated);

  int height = rotated_formatted->h;
  int width = rotated_formatted->w;

  Uint8 res[height*width];
  formatting_binarized(rotated_formatted,res);

  size_t size_blocks = 0;
  SDL_Rect* blocks = get_blocks(rotated_formatted,res,&size_blocks);

  save_blocks(rotated_formatted,blocks,size_blocks);
  printf("Blocks saved in 'blocks/' folder.\n");

  free(blocks);

  SDL_FreeSurface(formatted);
  SDL_FreeSurface(denoised);

  return 0;
  // if (SDL_LockSurface(image_format) < 0) {
    // errx(EXIT_FAILURE,"error on locking surface: %s\n",SDL_GetError());
  // }

  // SDL_UnlockSurface(image_format);
}
