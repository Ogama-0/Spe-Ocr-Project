#ifndef MANUAL_ROTATE_H
#define MANUAL_ROTATE_H

#include <gdk-pixbuf/gdk-pixbuf.h>

GdkPixbuf* manual_rotate(GdkPixbuf* src, double angle_degrees);
GdkPixbuf* automatic_rotate(GdkPixbuf* pixbuf);

#endif
