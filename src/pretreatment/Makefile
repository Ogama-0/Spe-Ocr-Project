CC = gcc
CFLAGS = -Wall -Wextra `pkg-config --cflags --libs gdk-pixbuf-2.0` -lSDL2 -fsanitize=address
LDFLAGS = -lSDL2 `pkg-config --cflags --libs gdk-pixbuf-2.0` -lm

SRC = main.c denoise.c manual_rotate.c binarize.c blocks_treatment.c pixbuf_scale.c
OBJ = $(SRC:.c=.o)

TARGET = preprocessing

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET) images/* blocks/grid/* blocks/words/*
