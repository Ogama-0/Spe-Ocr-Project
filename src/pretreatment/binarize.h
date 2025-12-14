#ifndef BINARIZE_H

#define BINARIZE_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdint.h>

void grayscale(GdkPixbuf* pixbuf);
void binarize(GdkPixbuf* pixbuf);
int is_black(guchar pixel);
void formatting_binarized(GdkPixbuf* pixbuf,
                            uint8_t* res);
void binarize_letters(GdkPixbuf* pixbuf);
void print_formatted_bin(uint8_t* arr,int width,int height);

#endif
