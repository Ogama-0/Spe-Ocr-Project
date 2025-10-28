CC := gcc
CFLAGS := -Wall -Wextra -fsanitize=address
LIBS := -lSDL2 -lSDL2_image -lm
SRC := binarize.c blocks_treatment.c detect_blocks.c 
OBJ := $(SRC:.c=.o)

detect_grid : $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o detect_grid $(LIBS)

clean :
	$(RM) "detect_grid"
	$(RM) "detect_grid.o"
	$(RM) "binarized.bmp"
	$(RM) blocks/*
	$(RM) images/*

.PHONY = clean
