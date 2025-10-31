CC = gcc
CFLAGS = -Wall -Wextra -std=c11 `sdl2-config --cflags` `pkg-config --cflags --libs gdk-pixbuf-2.0`
LDFLAGS = -lSDL2 `pkg-config --cflags --libs gdk-pixbuf-2.0` -lSDL2_gfx

SRC = main.c denoise.c manual_rotate.c binarize.c blocks_treatment.c
OBJ = $(SRC:.c=.o)

TARGET = preprocessing

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET) images/* blocks/*
