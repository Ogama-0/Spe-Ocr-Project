#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <stdlib.h>
#include "denoise.h"

#define WINDOW_SIZE 3

int compare_guchar(const void *a, const void *b)
{
    return (int)(*(guchar*)a - *(guchar*)b);
}

/**
 * @brief Removes noise from a binary image using morphological operations
 * 
 * @param input Binary input image
 * @return GdkPixbuf* Denoised image, NULL on failure
 */
GdkPixbuf *median_filter(GdkPixbuf* pixbuf)
{
    int w = gdk_pixbuf_get_width(pixbuf);
    int h = gdk_pixbuf_get_height(pixbuf);

    GdkPixbuf* pix2 = gdk_pixbuf_new(
                          GDK_COLORSPACE_RGB,
                          gdk_pixbuf_get_has_alpha(pixbuf),
                          8,
                          w, h
                      );

    if (pix2 == NULL)
    {
        printf("pix2 empty\n");
    }

    guchar *src_pixels = gdk_pixbuf_get_pixels(pixbuf);
    guchar *dst_pixels = gdk_pixbuf_get_pixels(pix2);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    int offset = WINDOW_SIZE / 2;
    guchar window[WINDOW_SIZE * WINDOW_SIZE];

    for (int y = offset; y < h - offset; y++)
    {
        for (int x = offset; x < w - offset; x++)
        {
            guchar* p_dst = dst_pixels + y*rowstride +
                            x*n_channels;

            int k = 0;

            for (int j = -offset; j <= offset; j++)
            {
                guchar* p_src = src_pixels + (y+j)*rowstride +
                                (x-offset)*n_channels;
                
                for (int i = -offset; i <= offset; i++)
                {
                    guchar r = p_src[0];

                    if (n_channels == 4)
                    {
                        p_dst[3] = p_src[3];
                    }

                    window[k++] = r;

                    p_src += n_channels;
                }
            }

            qsort(window, WINDOW_SIZE * WINDOW_SIZE, sizeof(guchar),
                  compare_guchar);

            guchar median = window[(WINDOW_SIZE * WINDOW_SIZE) / 2];

            p_dst[0] = median;
            p_dst[1] = median;
            p_dst[2] = median;
        }
    }
    
    return pix2;
}
