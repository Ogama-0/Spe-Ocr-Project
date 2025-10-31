#include "manual_rotate.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>

GdkPixbuf* rotate_pixbuf(GdkPixbuf* src, double angle_degrees) {
    int src_w = gdk_pixbuf_get_width(src);
    int src_h = gdk_pixbuf_get_height(src);
    int channels = gdk_pixbuf_get_n_channels(src);
    int rowstride = gdk_pixbuf_get_rowstride(src);
    guchar *src_pixels = gdk_pixbuf_get_pixels(src);

    double angle = angle_degrees * M_PI / 180.0;
    double cos_a = cos(angle);
    double sin_a = sin(angle);

    // Dimensions of new pixbuf (conservatives)
    int dst_w = src_w;
    int dst_h = src_h;

    GdkPixbuf *dst = gdk_pixbuf_new(gdk_pixbuf_get_colorspace(src),
                                    gdk_pixbuf_get_has_alpha(src),
                                    gdk_pixbuf_get_bits_per_sample(src),
                                    dst_w, dst_h);

    int dst_rowstride = gdk_pixbuf_get_rowstride(dst);
    guchar *dst_pixels = gdk_pixbuf_get_pixels(dst);

    int cx = src_w / 2;
    int cy = src_h / 2;

    for (int y = 0; y < dst_h; y++) {
        for (int x = 0; x < dst_w; x++) {
            // Centered coords
            double dx = x - cx;
            double dy = y - cy;

            // Inverse rotation
            double src_x =  dx * cos_a + dy * sin_a + cx;
            double src_y = -dx * sin_a + dy * cos_a + cy;

            // bilinear Interpolation
            int x0 = (int)floor(src_x);
            int y0 = (int)floor(src_y);
            double fx = src_x - x0;
            double fy = src_y - y0;

            guchar *dst_pixel = dst_pixels + y * dst_rowstride + x * channels;

            if (x0 >= 0 && x0 + 1 < src_w && y0 >= 0 && y0 + 1 < src_h) {
                for (int c = 0; c < channels; c++) {
                    guchar p00 = src_pixels[(y0)     * rowstride + (x0)     * channels + c];
                    guchar p10 = src_pixels[(y0)     * rowstride + (x0 + 1) * channels + c];
                    guchar p01 = src_pixels[(y0 + 1) * rowstride + (x0)     * channels + c];
                    guchar p11 = src_pixels[(y0 + 1) * rowstride + (x0 + 1) * channels + c];

                    double val = (1 - fx) * (1 - fy) * p00 +
                                 fx       * (1 - fy) * p10 +
                                 (1 - fx) * fy       * p01 +
                                 fx       * fy       * p11;

                    dst_pixel[c] = (guchar)round(val);
                }
            } else {
                // PIxel out of place: black or transparent
                for (int c = 0; c < channels; c++) {
                    dst_pixel[c] = 0;
                }
            }
        }
    }

    return dst;
}
