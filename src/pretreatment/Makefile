CC = gcc
CFLAGS = -Wall -Wextra `pkg-config --cflags --libs gdk-pixbuf-2.0` -fsanitize=address

LDFLAGS = `pkg-config --cflags --libs gdk-pixbuf-2.0` -lm

SRC = main.c denoise.c manual_rotate.c binarize.c blocks_treatment.c pixbuf_scale.c ../solver/find_words.c \
		../solver/solver.c
		
OBJ = $(SRC:.c=.o)

TARGET = preprocessing

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET) images/* blocks/grid/* blocks/words/*
