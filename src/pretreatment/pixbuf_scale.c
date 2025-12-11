#include <gdk-pixbuf/gdk-pixbuf.h>
#include <SDL2/SDL.h>
#include "pixbuf_scale.h"
#include "binarize.h"

void save_pixbuf_scale(GdkPixbuf* pixbuf,SDL_Rect* rect,size_t i1, size_t i2,
                       int is_grid)
{
  int size = rect->w > rect->h ? rect->w : rect->h;
  GdkPixbuf* new = gdk_pixbuf_new (
    GDK_COLORSPACE_RGB,
    TRUE,
    gdk_pixbuf_get_bits_per_sample(pixbuf),
    size, size
  );

  gdk_pixbuf_fill(new,0xFFFFFFFF);

  int dx = (size - rect->w) / 2;
  int dy = (size - rect->h) / 2;

  gdk_pixbuf_copy_area(pixbuf,
                       rect->x,rect->y,rect->w,rect->h,
                       new,
                       dx,dy);

  GdkPixbuf* scaled = gdk_pixbuf_scale_simple(
    new,
    27,27,
    GDK_INTERP_BILINEAR
  );

  binarize_letters(scaled);

  char* grid = "blocks/grid/";
  char* words = "blocks/words/";
  char* dir;
  if (is_grid)
  {
    dir = grid;
  }
  else
  {
    dir = words;
  }

  char filename[64];
  snprintf(filename,sizeof(filename),
           "%sletter %zu %zu.bmp",dir,i1,i2);

  GError *error = NULL;
  gboolean ok = gdk_pixbuf_save(
    scaled,
    filename,
    "png",
    &error,
    NULL
  );

  if (!ok) {
    g_printerr("Error on saving: %s\n", error->message);
    g_error_free(error);
  }

  g_object_unref(new);
  g_object_unref(scaled);
}
