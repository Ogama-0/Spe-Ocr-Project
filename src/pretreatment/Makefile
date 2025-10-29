CC = gcc
CFLAGS = -Wall -Wextra -std=c11 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lSDL2_gfx

SRC = main.c denoise.c manual_rotate.c binarize.c blocks_treatment.c detect_blocks.c
OBJ = $(SRC:.c=.o)
TARGET = preprocessing

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
