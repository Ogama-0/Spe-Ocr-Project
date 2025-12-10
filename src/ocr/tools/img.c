#include "../../matrix/matrix.h"
#include "img.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Img **load_dataset(char *file_string, int *nb_imgs) {
  const int expected_rows = 27;
  const int expected_cols = 27;

  *nb_imgs = 0;
  int capacity = 32;
  Img **dataset = malloc(capacity * sizeof(Img *));

  if (!dataset) {
    fprintf(stderr, "Erreur malloc dataset\n");
    exit(EXIT_FAILURE);
  }

  for (int label = 0; label < 26; label++) {
    char letter = 'A' + label;

    // path vers dataset/A
    char folder_path[512];
    snprintf(folder_path, sizeof(folder_path), "%s/%c", file_string, letter);

    DIR *dir = opendir(folder_path);
    if (!dir) {
      fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", folder_path);
      exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    while ((entry = readdir(dir))) {
      if (!strstr(entry->d_name, ".png"))
        continue;

      // fichier complet
      char img_path[512];
      snprintf(img_path, sizeof(img_path), "%s/%s", folder_path, entry->d_name);

      GError *error = NULL;
      GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(img_path, &error);

      if (!pixbuf) {
        fprintf(stderr, "Erreur chargement image %s : %s\n", img_path,
                error->message);
        g_error_free(error);
        exit(EXIT_FAILURE);
      }

      int width = gdk_pixbuf_get_width(pixbuf);
      int height = gdk_pixbuf_get_height(pixbuf);
      int channels = gdk_pixbuf_get_n_channels(pixbuf);
      int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
      guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);

      if (width != expected_cols || height != expected_rows) {
        fprintf(stderr, "Erreur : %s fait %dx%d au lieu de 27x27\n", img_path,
                width, height);
        g_object_unref(pixbuf);
        exit(EXIT_FAILURE);
      }

      if (channels < 3) {
        fprintf(stderr, "Erreur : %s n'a pas au moins 3 canaux RGB\n",
                img_path);
        g_object_unref(pixbuf);
        exit(EXIT_FAILURE);
      }

      Matrix *mat = matrix_create(expected_rows, expected_cols);

      for (int r = 0; r < expected_rows; r++) {
        guchar *row = pixels + r * rowstride;
        for (int c = 0; c < expected_cols; c++) {
          guchar *px = row + c * channels;

          guchar R = px[0];
          guchar G = px[1];
          guchar B = px[2];

          int is_black = (R == 0 && G == 0 && B == 0);
          int is_white = (R == 255 && G == 255 && B == 255);

          if (!is_black && !is_white) {
            fprintf(stderr,
                    "Erreur : pixel (%d,%d,%d) non binaire dans %s (%d,%d)\n",
                    R, G, B, img_path, r, c);
            g_object_unref(pixbuf);
            exit(EXIT_FAILURE);
          }

          mat->entries[r][c] = is_black ? 1.0 : 0.0;
        }
      }

      g_object_unref(pixbuf);

      Img *img = malloc(sizeof(Img));
      img->img_data = mat;
      img->label = label;

      // agrandir le tableau si nécessaire
      if (*nb_imgs >= capacity) {
        capacity *= 2;
        dataset = realloc(dataset, capacity * sizeof(Img *));
        if (!dataset) {
          fprintf(stderr, "Erreur realloc dataset\n");
          exit(EXIT_FAILURE);
        }
      }

      dataset[*nb_imgs] = img;
      (*nb_imgs)++;
    }

    closedir(dir);
  }

  return dataset;
}

// Img **load_imgs(const char *path, int *out_count) {
//   DIR *dir = opendir(path);
//   if (!dir) {
//     fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", path);
//     *out_count = 0;
//     return NULL;
//   }

//   struct dirent *entry;
//   Img **imgs = NULL;
//   int capacity = 10;
//   int count = 0;

//   imgs = malloc(capacity * sizeof(Img *));
//   if (!imgs) {
//     fprintf(stderr, "Erreur allocation mémoire\n");
//     closedir(dir);
//     *out_count = 0;
//     return NULL;
//   }

//   while ((entry = readdir(dir))) {
//     if (!strstr(entry->d_name, ".png"))
//       continue;

//     // Construire le chemin complet du fichier
//     char img_path[512];
//     snprintf(img_path, sizeof(img_path), "%s/%s", path, entry->d_name);

//     GError *err = NULL;
//     Img *img = gdk_pixbuf_new_from_file(img_path, &err);
//     if (!img) {
//       fprintf(stderr, "Erreur chargement image %s : %s\n", img_path,
//               err->message);
//       g_error_free(err);
//       continue;
//     }

//     // Vérifier taille 27x27
//     int w = gdk_pixbuf_get_width(img);
//     int h = gdk_pixbuf_get_height(img);
//     if (w != 27 || h != 27) {
//       fprintf(stderr, "Ignorée (taille non conforme) : %s (%dx%d)\n",
//       img_path,
//               w, h);
//       g_object_unref(img);
//       continue;
//     }

//     // Binariser : pixels proches du noir/blanc deviennent noir ou blanc
//     guchar *pixels = gdk_pixbuf_get_pixels(img);
//     int rowstride = gdk_pixbuf_get_rowstride(img);
//     int n_channels = gdk_pixbuf_get_n_channels(img);

//     for (int y = 0; y < h; y++) {
//       guchar *row = pixels + y * rowstride;
//       for (int x = 0; x < w; x++) {
//         guchar *p = row + x * n_channels;

//         // Ici on suppose RGB sans alpha
//         // Valeur moyenne de la couleur
//         int avg = (p[0] + p[1] + p[2]) / 3;

//         // Seuil pour noir ou blanc
//         if (avg < 128) {
//           p[0] = 0;
//           p[1] = 0;
//           p[2] = 0;
//         } else {
//           p[0] = 255;
//           p[1] = 255;
//           p[2] = 255;
//         }
//       }
//     }

//     // Ajouter dans le tableau
//     if (count >= capacity) {
//       capacity *= 2;
//       Img **tmp = realloc(imgs, capacity * sizeof(Img *));
//       if (!tmp) {
//         fprintf(stderr, "Erreur realloc\n");
//         g_object_unref(img);
//         break;
//       }
//       imgs = tmp;
//     }

//     imgs[count++] = img;
//   }

//   closedir(dir);
//   *out_count = count;
//   return imgs;
// }

void img_print(Img *img) {
  matrix_print(img->img_data);
  printf("Img Label: %c\n", img->label + 'a');
}

void img_free(Img *img) {
  matrix_free(img->img_data);
  free(img);
  img = NULL;
}

void imgs_free(Img **imgs, int n) {
  for (int i = 0; i < n; i++) {
    img_free(imgs[i]);
  }
  free(imgs);
  imgs = NULL;
}
