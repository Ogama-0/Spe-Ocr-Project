#ifndef MANUAL_ROTATE_H
#define MANUAL_ROTATE_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <SDL2/SDL.h>

GdkPixbuf* rotate_pixbuf(GdkPixbuf* src, double angle_degrees);

#endif
