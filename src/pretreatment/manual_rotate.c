#include <gdk-pixbuf/gdk-pixbuf.h>
#include <math.h>
#include "manual_rotate.h"
#include "blocks_treatment.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GdkPixbuf *rotate_pixbuf(GdkPixbuf *src, double angle_degrees)
{
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

    GdkPixbuf *dst = gdk_pixbuf_new(
        gdk_pixbuf_get_colorspace(src), gdk_pixbuf_get_has_alpha(src),
        gdk_pixbuf_get_bits_per_sample(src), dst_w, dst_h);

    int dst_rowstride = gdk_pixbuf_get_rowstride(dst);
    guchar *dst_pixels = gdk_pixbuf_get_pixels(dst);

    int cx = src_w / 2;
    int cy = src_h / 2;

    for (int y = 0; y < dst_h; y++)
    {
        for (int x = 0; x < dst_w; x++)
        {
            // Centered coords
            double dx = x - cx;
            double dy = y - cy;

            // Inverse rotation
            double src_x = dx * cos_a + dy * sin_a + cx;
            double src_y = -dx * sin_a + dy * cos_a + cy;

            // bilinear Interpolation
            int x0 = (int)floor(src_x);
            int y0 = (int)floor(src_y);
            double fx = src_x - x0;
            double fy = src_y - y0;

            guchar *dst_pixel = dst_pixels + y * dst_rowstride + x * channels;

            if (x0 >= 0 && x0 + 1 < src_w && y0 >= 0 && y0 + 1 < src_h)
            {
                for (int c = 0; c < channels; c++)
                {
                    guchar p00 = src_pixels[(y0)*rowstride + (x0)*channels + c];
                    guchar p10 =
                        src_pixels[(y0)*rowstride + (x0 + 1) * channels + c];
                    guchar p01 =
                        src_pixels[(y0 + 1) * rowstride + (x0)*channels + c];
                    guchar p11 = src_pixels[(y0 + 1) * rowstride +
                                            (x0 + 1) * channels + c];

                    double val = (1 - fx) * (1 - fy) * p00 +
                                 fx * (1 - fy) * p10 + (1 - fx) * fy * p01 +
                                 fx * fy * p11;

                    dst_pixel[c] = (guchar)round(val);
                }
            }
            else
            {
                // PIxel out of place: black or transparent
                for (int c = 0; c < channels; c++)
                {
                    dst_pixel[c] = 255;
                }
            }
        }
    }

    return dst;
}

void proj_horizontal(guchar *pixels, double *proj, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        double sum = 0;
        for (int j = 0; j < width; j++)
        {
            int line = pixels[i * width + j];
            if (line == 0)
                sum++;
        }
        proj[i] = sum;
    }
}

void proj_vertical(guchar *pixels, double *proj, int width, int height)
{
    for (int i = 0; i < width; i++)
    {
        double sum = 0;
        for (int j = 0; j < height; j++)
        {
            int col = pixels[j * width + i];
            if (col == 0)
                sum++;
        }
        proj[i] = sum;
    }
}

double score(const double *proj1, const double *proj2, int width, int height)
{
    size_t size1 = 0;
    size_t size2 = 0;

    double proj1_tmp[width];
    double proj2_tmp[height];

    for (int i = 0; i < width; i++)
    {
        if (proj1[i] > 10)
        {
            proj1_tmp[size1] = proj1[i];
            size1++;
        }
    }
    for (int j = 0; j < height; j++)
    {
        if (proj2[j] > 10)
        {
            proj2_tmp[size2] = proj2[j];
            size2++;
        }
    }

    double mean1 = 0;
    double mean2 = 0;
    for (size_t i = 0; i < size1; i++)
    {
        mean1 += proj1_tmp[i];
    }
    mean1 /= (size1 + 1);
    for (size_t j = 0; j < size2; j++)
    {
        mean2 += proj2_tmp[j];
    }
    mean2 /= (size2 + 1);

    return mean1 + mean2;
}

GdkPixbuf *automatic_rotate(GdkPixbuf *pixbuf)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);

    double max_score = -1;

    GdkPixbuf **max = malloc(sizeof(GdkPixbuf *));
    *max = NULL;

    const double min_thresh = -30.0;
    const double max_thresh = 30.0;

    for (int i = min_thresh; i <= max_thresh; i++)
    {
        GdkPixbuf *rotated = rotate_pixbuf(pixbuf, (double)i);
        guchar *pixels = gdk_pixbuf_get_pixels(rotated);

        double proj2[height];
        double proj1[width];
        proj_vertical(pixels, proj1, width, height);
        proj_horizontal(pixels, proj2, width, height);

        double s = score(proj1, proj2, width, height);

        if (s > max_score)
        {
            if (max_score > -1.0)
            {
                g_object_unref(*max);
            }
            max_score = s;
            *max = rotated;
        }
        else
        {
            g_object_unref(rotated);
        }
    }

    GdkPixbuf *res = *max;
    free(max);

    return res;
}
