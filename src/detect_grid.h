#ifndef DETECT_GRID_H

#define DETECT_GRID_H

void binarize(SDL_Surface* surface);
int is_black(Uint32 pixel);
void formatting_binarized(SDL_Surface* binarized_surface,Uint8* res);

void update_block(int x, int y, SDL_Rect** blocks,
                  size_t* size, size_t* capacity,
                  SDL_Surface* surface, int** visited, Uint8* image);
SDL_Rect get_block(SDL_Surface* surface,Uint8* image,
                   int x,int y,int** visited);
SDL_Rect* get_blocks(SDL_Surface* surface,Uint8* image,size_t* size);
void save_blocks(SDL_Surface* surface,SDL_Rect* blocks,size_t size);

void detect_blocks(char* file);

struct Point
{
  int x;
  int y;
};

#endif
