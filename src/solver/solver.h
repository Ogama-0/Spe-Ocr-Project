#pragma once

void solve_word(char* grid,char* word,struct Rect* grid_blocks,
                int* results);

char **read_words(const char *filename, int *count);

int line_pos(struct Rect* grid_blocks);
