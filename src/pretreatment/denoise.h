#ifndef DENOISE_H
#define DENOISE_H

#define WINDOW_SIZE 3

int compare_guchar(const void* a, const void* b);
GdkPixbuf* median_filter(GdkPixbuf* pixbuf);

#endif
