#include "blocks_treatment.h"
#include "err.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// for a black pixel, check if it's an existing block.
// If not, create a new block.
void update_block(int x, int y, SDL_Rect **blocks, size_t *size,
                  size_t *capacity, SDL_Surface *surface, int **visited,
                  Uint8 *image)
{
    int exists = 0;
    size_t i = 0;

    while (i < *size && exists == 0)
    {
        // check the dimensions to see if it is an existing block.
        if (y >= (*blocks)[i].y && y < (*blocks)[i].y + (*blocks)[i].h &&
            x >= (*blocks)[i].x && x < (*blocks)[i].x + (*blocks)[i].w &&
            visited[y][x] == 1)
        {
            exists = 1;
        }
        i++;
    }

    if (exists == 0)
    {
        if (*size + 1 >= *capacity)
        {
            // modify the size of the blocks array if we exceed the
            // capacity.
            *blocks = realloc(*blocks, (*capacity) * 2 * sizeof(SDL_Rect));
            if (*blocks == NULL)
            {
                printf("error on reallocing\n");
            }
            (*capacity) *= 2;
        }

        SDL_Rect b = get_block(surface, image, x, y, visited);

        if (b.w >= 3 && b.h > 3)
        {
            // printf("density: x:%i y:%i\n",b.x,b.y);
            (*blocks)[*size] = b;
            (*size)++;
        }
    }
}

// get a block using a propagation algorithm, from a black pixel belonging
// to the block.
SDL_Rect get_block(SDL_Surface *surface, Uint8 *image, int x, int y,
                   int **visited)
{
    int w = surface->w;
    int h = surface->h;

    struct Point *stack = malloc(w * h * sizeof(struct Point));
    int top = 0;

    stack[top++] = (struct Point){x, y};
    visited[y][x] = 1;

    int min_x = x, max_x = x;
    int min_y = y, max_y = y;

    while (top > 0)
    {
        struct Point p = stack[--top];

        if (p.x < min_x)
            min_x = p.x;
        if (p.x > max_x)
            max_x = p.x;
        if (p.y < min_y)
            min_y = p.y;
        if (p.y > max_y)
            max_y = p.y;

        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};

        for (int i = 0; i < 4; i++)
        {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];

            if (nx >= 0 && nx < w && ny >= 0 && ny < h && image[ny * w + nx] &&
                visited[ny][nx] == 0)
            {
                stack[top++] = (struct Point){nx, ny};
                visited[ny][nx] = 1;
            }
        }
    }

    free(stack);

    SDL_Rect rect;
    rect.x = min_x;
    rect.y = min_y;
    rect.w = max_x - min_x + 1;
    rect.h = max_y - min_y + 1;
    return rect;
}

// for every black pixel encountered, call the update_block function.
SDL_Rect *get_blocks(SDL_Surface *surface, Uint8 *image, size_t *size)
{
    size_t capacity = 4;
    // create the array of blocks.
    SDL_Rect *blocks = malloc(capacity * sizeof(SDL_Rect));
    if (blocks == NULL)
    {
        printf("error on mallocing\n");
    }

    int height = surface->h;
    int width = surface->w;
    // Uint32* pixels = surface->pixels;

    // create the array with the visited pixels.
    int **visited = calloc(height, sizeof(int *));
    if (visited == NULL)
    {
        printf("error on calloc\n");
    }
    for (int i = 0; i < height; i++)
    {
        visited[i] = calloc(width, sizeof(int));
        if (visited[i] == NULL)
        {
            printf("error on calloc for %i\n", i);
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i * width + j])
            {
                update_block(j, i, &blocks, size, &capacity, surface, visited,
                             image);
            }
        }
    }

    for (int i = 0; i < height; i++)
    {
        free(visited[i]);
    }
    free(visited);

    return blocks;
}

// get the amount of black pixels in a block.
int get_density(SDL_Rect block, Uint8 *image, int width)
{
    int i = block.x;
    int density = 0;

    while (i < block.x + block.w)
    {
        int j = block.y;
        while (j < block.y + block.h)
        {
            if (image[j * width + i])
            {
                density++;
            }
            j++;
        }
        i++;
    }

    return density;
}

// get the density of every block in the image.
int *density_map(SDL_Rect *blocks, size_t size, Uint8 *image, int width)
{
    int *map = malloc(size * sizeof(int));
    for (size_t i = 0; i < size; i++)
    {
        map[i] = get_density(blocks[i], image, width);
    }
    return map;
}

int compare(const void *a, const void *b) { return (*(int *)a - *(int *)b); }

// get the median of the density. To do that, do a quick sort
// with the density_map array and return the middle element or
// the mean of the two middles if the number of elements is even.
int get_density_median(SDL_Rect *blocks, size_t size, Uint8 *image, int width)
{
    int *map = density_map(blocks, size, image, width);

    qsort(map, (int)size, sizeof(int), compare);
    printf("median density: ");

    if (size % 2 == 0)
    {
        printf("%i\n", (map[size / 2 - 1] + map[size / 2]) / 2);
        int m = (map[size / 2 - 1] + map[size / 2]) / 2;
        free(map);
        return m;
    }
    else
    {
        printf("%i\n", map[size / 2]);
        int m2 = map[size / 2];
        free(map);
        return m2;
    }
}

int get_median_height(SDL_Rect *blocks, size_t size)
{
    int height_map[size];
    for (size_t i = 0; i < size; i++)
    {
        height_map[i] = blocks[i].h;
        // printf("height num %zu: %i\n",i,blocks[i].h);
    }

    qsort(height_map, (int)size, sizeof(int), compare);
    printf("median height: ");

    if (size % 2 == 0)
    {
        int m1 = (height_map[size / 2 - 1] + height_map[size / 2]) / 2;
        printf("%i\n", m1);
        return m1;
    }
    else
    {
        int m2 = height_map[size / 2];
        printf("%i\n", m2);
        return m2;
    }
}

// with the median of the density, get every block that
// represents a letter.
SDL_Rect *get_letter_blocks(SDL_Rect *blocks, size_t size, int *m_height,
                            Uint8 *image, int width, size_t *nb_letters)
{
    int density_median = get_density_median(blocks, size, image, width);
    int height_median = get_median_height(blocks, size);
    *m_height = height_median;
    int thresh_min = density_median / 4, thresh_max = density_median * 4;
    int thresh_min_h = height_median / 4, thresh_max_h = height_median * 3 / 2;
    printf("density thresholds: %i %i\n", thresh_min, thresh_max);
    printf("height thresholds: %i %i\n", thresh_min_h, thresh_max_h);

    *nb_letters = 0;
    SDL_Rect *letters = malloc(size * sizeof(SDL_Rect));

    for (size_t i = 0; i < size; i++)
    {
        int density = get_density(blocks[i], image, width);

        if (density > thresh_min && density < thresh_max &&
            blocks[i].h > thresh_min_h && blocks[i].h < thresh_max_h)
        {
            letters[*nb_letters] = blocks[i];
            (*nb_letters)++;
        }
        /*
        else
        {
          printf("Block n°%zu: density: %i\n",i,density);
        }
        */
    }

    printf("number of letters: %zu\n", *nb_letters);
    return letters;
}

int get_dist_blocks(struct Image_zone b1, SDL_Rect b2)
{
    int x_diff;
    int y_diff;

    if (b2.y < b1.y_max)
    {
        y_diff = 0;
    }
    else
    {
        y_diff = b2.y - b1.y_max;
    }

    if (b2.x + b2.w - 1 < b1.x_min)
    {
        x_diff = b1.x_min - (b2.x + b2.w - 1);
    }
    else if (b2.x > b1.x_max)
    {
        x_diff = b2.x - b1.x_max;
    }
    else
    {
        x_diff = 0;
    }

    return sqrt(x_diff * x_diff + y_diff * y_diff);
}

int compare_blocks_y(const void *a, const void *b)
{
    return (((SDL_Rect *)a)->y - ((SDL_Rect *)b)->y);
}

int compare_blocks_x(const void *a, const void *b)
{
    return (((SDL_Rect *)a)->x - ((SDL_Rect *)b)->x);
}

// this function first sorts every letter by their
// y coords. Then every group of letters with approximately
// the same y coords is sorted by their x coords.
void sort_blocks(SDL_Rect *blocks, size_t size)
{

    qsort(blocks, size, sizeof(SDL_Rect), compare_blocks_y);

    size_t i = 1;
    while (i < size)
    {
        size_t j = i - 1;
        size_t s_tmp = 1;
        while (i < size && abs(blocks[i].y - blocks[i - 1].y) < 10)
        {
            s_tmp++;
            i++;
        }

        qsort(blocks + j, s_tmp, sizeof(SDL_Rect), compare_blocks_x);
        i++;
    }
}

// this function fills two sets with letters in the image.
// The two sets represent the grid and the words to find.
// We decide which is which in the save_blocks function.
void get_sets(struct Image_zone *set1, size_t *s1, size_t *cap1,
              struct Image_zone *set2, size_t *s2, size_t *cap2,
              SDL_Rect *blocks, size_t size, int median_height)
{
    size_t i = 0;
    printf("distance threshold: %i\n", 2 * median_height);
    while (i < size)
    {

        if (*s1 == 0)
        {

            // initialize the first set.
            set1->blocks[*s1] = blocks[i];
            set1->x_min = blocks[i].x;
            set1->x_max = blocks[i].x + blocks[i].w - 1;
            set1->y_min = blocks[i].y;
            set1->y_max = blocks[i].y + blocks[i].h - 1;
            set1->nb_col = 1;
            set1->nb_rows = 1;

            (*s1)++;
        }
        else if (get_dist_blocks(*set1, blocks[i]) <= median_height * 2)
        {

            set1->blocks[*s1] = blocks[i];

            if (blocks[i].x < set1->x_min)
            {
                set1->x_min = blocks[i].x;
            }
            else if (blocks[i].x > set1->x_max)
            {
                set1->x_max = blocks[i].x + blocks[i].w - 1;
                set1->nb_col++;
            }
            else if (blocks[i].y > set1->y_max)
            {
                set1->y_max = blocks[i].y + blocks[i].h - 1;
                set1->nb_rows++;
            }

            if ((*s1) + 1 >= *cap1)
            {
                (*cap1) *= 2;
                set1->blocks =
                    realloc(set1->blocks, sizeof(SDL_Rect) * (*cap1));
            }

            (*s1)++;
        }
        else
        {

            printf("\ntoo far for grid: %i\n",
                   get_dist_blocks(*set1, blocks[i]));

            if (*s2 == 0)
            {

                // initialize the second set.
                set2->blocks[*s2] = blocks[i];
                set2->x_min = blocks[i].x;
                set2->x_max = blocks[i].x + blocks[i].w - 1;
                set2->y_min = blocks[i].y;
                set2->y_max = blocks[i].y + blocks[i].h - 1;
                set2->nb_col = 1;
                set2->nb_rows = 1;

                (*s2)++;
            }
            else
            {

                set2->blocks[*s2] = blocks[i];

                if (blocks[i].x < set2->x_min)
                {
                    set2->x_min = blocks[i].x;
                }
                else if (blocks[i].x > set2->x_max)
                {
                    set2->x_max = blocks[i].x + blocks[i].w - 1;
                    set2->nb_col++;
                }
                else if (blocks[i].y > set2->y_max)
                {
                    set2->y_max = blocks[i].y + blocks[i].h - 1;
                    set2->nb_rows++;
                }

                if (*s2 + 1 >= *cap2)
                {
                    *cap2 *= 2;
                    set2->blocks =
                        realloc(set2->blocks, sizeof(SDL_Rect) * (*cap2));
                }

                (*s2)++;
            }
        }
        i++;
    }
}

// save each block into an image with bmp format.
void save_blocks(SDL_Surface *surface, SDL_Rect *letters, int m_height,
                 size_t size)
{
    size_t cap1 = 4;
    size_t cap2 = 4;
    size_t s1 = 0;
    size_t s2 = 0;

    SDL_Rect *blocks1 = malloc(sizeof(SDL_Rect) * cap1);
    SDL_Rect *blocks2 = malloc(sizeof(SDL_Rect) * cap2);
    struct Image_zone set1 = {blocks1, 0, 0, 0, 0, 0, 0};
    struct Image_zone set2 = {blocks2, 0, 0, 0, 0, 0, 0};

    sort_blocks(letters, size);

    /*
    for (size_t i = 0; i < size; i++) {
      printf("%i %i\n",letters[i].y,letters[i].x);
    }
    */

    get_sets(&set1, &s1, &cap1, &set2, &s2, &cap2, letters, size, m_height);

    struct Image_zone grid;
    struct Image_zone words;

    // decide which set is the grid and which are the words
    size_t size_words;
    size_t size_grid;

    if ((set1.x_max - set1.x_min) * (set1.y_max - set1.y_min) >
        (set2.x_max - set2.x_min) * (set2.y_max - set2.y_min))
    {
        grid = set1;
        words = set2;
        size_grid = s1;
        size_words = s2;
    }
    else
    {
        grid = set2;
        words = set1;
        size_grid = s2;
        size_words = s1;
    }

    printf("size of grid: %zu\n", size_grid);
    printf("nb of columns and rows of grid: %i %i\n", grid.nb_col,
           grid.nb_rows);
    printf("%i %i - %i %i\n\n", grid.x_min, grid.x_max, grid.y_min, grid.y_max);

    printf("size of words: %zu\n", size_words);
    printf("nb of columns and rows of words: %i %i\n", words.nb_col,
           words.nb_rows);
    printf("%i %i - %i %i\n\n", words.x_min, words.x_max, words.y_min,
           words.y_max);

    size_t fdg = 0;
    size_t gdf = 0;
    while (fdg < size_grid && gdf < size_words)
    {
        printf("grid: %i %i\tword: %i %i\n", grid.blocks[fdg].x,
               grid.blocks[fdg].y, words.blocks[gdf].x, words.blocks[gdf].y);
        fdg++;
        gdf++;
    }
    while (fdg < size_grid)
    {
        printf("grid: %i %i\n", grid.blocks[fdg].x, grid.blocks[fdg].y);
        fdg++;
    }
    while (gdf < size_words)
    {
        printf("             word: %i %i\n", words.blocks[gdf].x,
               words.blocks[gdf].y);
        gdf++;
    }

    // Save the grid's letters in bmp images
    for (size_t i = 0; i < (size_t)grid.nb_rows; i++)
    {
        for (size_t j = 0; j < (size_t)grid.nb_col; j++)
        {
            SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(
                0, grid.blocks[((size_t)grid.nb_col) * i + j].w,
                grid.blocks[((size_t)grid.nb_col) * i + j].h,
                surface->format->BitsPerPixel, SDL_PIXELFORMAT_RGBA8888);

            // printf("Bloc %zu: x=%d y=%d w=%d
            // h=%d\n",i,blocks[i].x,blocks[i].y, blocks[i].w,blocks[i].h);

            if (!surf)
            {
                printf("error on surfacing block n°%zu: %s\n", i,
                       SDL_GetError());
                free(grid.blocks);
                free(words.blocks);
                SDL_FreeSurface(surf);
                return;
            }

            // fills the image with red if it doesn't work
            // SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 0, 0));

            // fills the surface with a portion of the original surface.
            if (SDL_BlitSurface(surface,
                                &(grid.blocks[((size_t)grid.nb_col) * i + j]),
                                surf, NULL) != 0)
            {
                printf("error on blitting surface: %s\n", SDL_GetError());
            }

            char filename[64];
            // save a name with a different index every time.
            snprintf(filename, sizeof(filename),
                     "blocks/grid/letter %zu %zu.bmp", i, j);
            SDL_SaveBMP(surf, filename);
            SDL_FreeSurface(surf);
        }
    }
    free(grid.blocks);

    // save the words' letters in bmp images
    size_t i = 1;
    int words_count = 0;
    while (i < size_words)
    {
        size_t j = i - 1;
        size_t size_row = 1;
        while (i < size_words &&
               abs(words.blocks[i].y - words.blocks[i - 1].y) < 10)
        {
            i++;
            size_row++;
        }

        printf("size of row: %zu\n", size_row);
        printf("%i\n", words_count);
        printf("%zu\n", j);
        printf("from %i to %i\n\n", words.blocks[j].x, words.blocks[i - 1].x);

        for (size_t k = j; k < j + size_row; k++)
        {
            SDL_Surface *surf = SDL_CreateRGBSurfaceWithFormat(
                0, words.blocks[k].w, words.blocks[k].h,
                surface->format->BitsPerPixel, SDL_PIXELFORMAT_RGBA8888);

            // printf("Bloc %zu: x=%d y=%d w=%d
            // h=%d\n",i,blocks[i].x,blocks[i].y, blocks[i].w,blocks[i].h);

            if (!surf)
            {
                printf("error on surfacing word block n°%zu: %s\n", j,
                       SDL_GetError());
                free(words.blocks);
                SDL_FreeSurface(surf);
                return;
            }

            // fills the image with red if it doesn't work
            // SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 0, 0));

            // fills the surface with a portion of the original surface.
            if (SDL_BlitSurface(surface, &(words.blocks[k]), surf, NULL) != 0)
            {
                printf("error on blitting surface: %s\n", SDL_GetError());
            }

            printf("saving image...\n");

            char filename[64];
            // save a name with a different index every time.
            snprintf(filename, sizeof(filename),
                     "blocks/words/letter %i %zu.bmp", words_count, k - j);
            SDL_SaveBMP(surf, filename);
            SDL_FreeSurface(surf);
        }
        words_count++;
        i++;
    }
    free(words.blocks);

    /*
    for (size_t i = 0; i < size; i++) {
      SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(
        0,
        letters[i].w,
        letters[i].h,
        surface->format->BitsPerPixel,
        SDL_PIXELFORMAT_RGBA8888
      );

      // printf("Bloc %zu: x=%d y=%d w=%d h=%d\n",i,blocks[i].x,blocks[i].y,
      // blocks[i].w,blocks[i].h);

      if (!surf)
      {
        printf("error on surfacing word block n°%zu: %s\n",i,SDL_GetError());
        return;
      }

      // fills the image with red if it doesn't work
      // SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 0, 0));

      // fills the surface with a portion of the original surface.
      if (SDL_BlitSurface(surface,&(letters[i]),
                          surf,NULL) != 0)
      {
        printf("error on blitting surface: %s\n",SDL_GetError());
      }

      char filename[64];
      // save a name with a different index every time.
      snprintf(filename,sizeof(filename),"blocks/letter %zu.bmp",i);
      SDL_SaveBMP(surf,filename);
      SDL_FreeSurface(surf);
    }
    */
}
