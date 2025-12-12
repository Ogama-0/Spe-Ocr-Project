#ifndef DETECT_BLOCKS_H

#define DETECT_BLOCKS_H

struct Rect {
  int x; int y;
  int w; int h;
};

void update_block(int x, int y, SDL_Rect** blocks,
                  size_t* size, size_t* capacity,
                  SDL_Surface* surface, int** visited, Uint8* image);

SDL_Rect get_block(SDL_Surface* surface,Uint8* image,
                   int x,int y,int** visited);

SDL_Rect* get_blocks(SDL_Surface* surface,Uint8* image,size_t* size);

SDL_Rect* get_letter_blocks(SDL_Rect* blocks,size_t size,int* m_height,
                            Uint8* image,int width,size_t* nb_letters);

int get_density(SDL_Rect block,Uint8* image,int width);

void save_blocks(SDL_Surface* surface,SDL_Rect* blocks,int m_height,
                 size_t size,GdkPixbuf* pixbuf);

struct Point {
  int x;
  int y;
};

struct Image_zone {
  SDL_Rect* blocks;
  int x_min; int x_max;
  int y_min; int y_max;
  int nb_col;
  int nb_rows;
};

#endif
