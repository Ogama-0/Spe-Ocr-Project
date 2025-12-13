#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <SDL2/SDL.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

int preprocess_image(const char *input_path, const char *output_path, double angle);

#endif
