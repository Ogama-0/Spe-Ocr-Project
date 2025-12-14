#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void grayscale(GdkPixbuf *pixbuf)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    for (int i = 0; i < height; i++)
    {
        guchar *p = pixels + i * rowstride;
        for (int j = 0; j < width; j++)
        {
            guchar r = p[0];
            guchar g = p[1];
            guchar b = p[2];

            guchar gray = (guchar)(0.3 * r + 0.59 * g + 0.11 * b);

            p[0] = gray;
            p[1] = gray;
            p[2] = gray;

            p += n_channels;
        }
    }
}

int comp(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

int get_gray_mean(GdkPixbuf *pixbuf)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    // int gray_list[size];
    int sum = 0;

    for (int i = 0; i < height; i++)
    {
        guchar *p = pixels + i * rowstride;
        for (int j = 0; j < width; j++)
        {
            guchar r = p[0];
            // guchar g = p[1];
            // guchar b = p[2];

            sum += (size_t)r;

            p += n_channels;
        }
    }

    size_t res = (size_t)sum / (height * width);
    return (int)res;
}

void binarize(GdkPixbuf *pixbuf)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    int mean = get_gray_mean(pixbuf);

    for (int i = 0; i < height; i++)
    {
        guchar *p = pixels + i * rowstride;
        for (int j = 0; j < width; j++)
        {
            guchar a = 255;
            if (n_channels == 4)
            {
                a = p[3];
            }

            guchar gray = p[0];

            if ((int)gray >= mean - 25 || a < 128)
            {
                p[0] = 255;
                p[1] = 255;
                p[2] = 255;
            }
            else
            {
                p[0] = 0;
                p[1] = 0;
                p[2] = 0;
            }

            if (n_channels == 4)
            {
                p[3] = 255;
            }

            p += n_channels;
        }
    }
}

void binarize_letters(GdkPixbuf *pixbuf)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    for (int y = 0; y < height; y++)
    {
        guchar *p = pixels + y * rowstride;
        for (int x = 0; x < width; x++)
        {
            guchar r = p[0];
            guchar g = p[1];
            guchar b = p[2];

            if (r > 200 && g > 200 && b > 200)
            {
                p[0] = 0;
                p[1] = 0;
                p[2] = 0;
            }

            else
            {
                p[0] = 255;
                p[1] = 255;
                p[2] = 255;
            }

            if (n_channels == 4)
            {
                p[3] = 255;
            }

            p += n_channels;
        }
    }
}

int is_black(guchar pixel)
{
    if (pixel == 0)
    {
        return 1;
    }
    return 0;
}

// transform the binarized image into an array of 0 and 1
void formatting_binarized(GdkPixbuf *pixbuf, uint8_t *res)
{
    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

    for (int i = 0; i < height; i++)
    {
        guchar *p = pixels + i * rowstride;
        for (int j = 0; j < width; j++)
        {
            if (is_black(p[0]))
            {
                res[i * width + j] = 1;
            }
            else
            {
                res[i * width + j] = 0;
            }

            p += n_channels;
        }
    }
}

// void print_formatted_bin(uint8_t *arr, int width, int height)
// {
//     FILE *f = fopen("grid_format.txt", "w");
//     int fd = fileno(f);

//     for (int i = 0; i < height; i++)
//     {
//         for (int j = 0; j < width; j++)
//         {
//             int n = arr[i * width + j];
//             if (n == 1)
//             {
//                 write(fd, "1", 1);
//             }
//             else
//             {
//                 write(fd, " ", 1);
//             }
//         }
//         write(fd, "\n", 1);
//     }
// }

/**
 * @brief Converts a grayscale image to binary using adaptive thresholding
 * 
 * @param input Input grayscale image
 * @return GdkPixbuf* Binary image, NULL on failure
 */
