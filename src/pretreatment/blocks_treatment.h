#ifndef DETECT_BLOCKS_H

#define DETECT_BLOCKS_H

#include <stdint.h>

struct Rect {
  int x; int y;
  int w; int h;
};

void update_block(int x, int y, struct Rect** blocks,
                  size_t* size, size_t* capacity,
                  GdkPixbuf* pixbuf, int** visited, uint8_t* image);

struct Rect get_block(GdkPixbuf* pixbuf,uint8_t* image,
                   int x,int y,int** visited);

struct Rect* get_blocks(GdkPixbuf* pixbuf,uint8_t* image,size_t* size);

struct Rect* get_letter_blocks(struct Rect* blocks,size_t size,int* m_height,
                            uint8_t* image,int width,size_t* nb_letters);

int get_density(struct Rect block,uint8_t* image,int width);

struct Rect* save_blocks(struct Rect* blocks,int m_height,
                 size_t size,GdkPixbuf* pixbuf);

struct Point {
  int x;
  int y;
};

struct Image_zone {
  struct Rect* blocks;
  int x_min; int x_max;
  int y_min; int y_max;
  int nb_col;
  int nb_rows;
};

#endif
