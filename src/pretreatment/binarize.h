#ifndef BINARIZE_H

#define BINARIZE_H

#include <SDL2/SDL.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

void grayscale(GdkPixbuf* pixbuf);
void binarize(GdkPixbuf* pixbuf);
int is_black(guchar pixel);
void formatting_binarized(GdkPixbuf* pixbuf,
                            Uint8* res);
void binarize_letters(GdkPixbuf* pixbuf);
void print_formatted_bin(Uint8* arr,int width,int height);

#endif
