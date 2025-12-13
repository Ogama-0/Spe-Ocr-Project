TARGET = ocr_gui

SRCS = ocr_gui.c \
       ../pretreatment/manual_rotate.c \
       ../pretreatment/preprocess.c \
       ../pretreatment/binarize.c \
       ../pretreatment/denoise.c \
       ../pretreatment/blocks_treatment.c \
       ../ocr/ocr.c \
       ../ocr/train_ocr.c \
       ../ocr/tools/activations.c \
       ../ocr/tools/img.c \
       ../ocr/tools/ext_img.c \
       ../ocr/tools/int_matrix.c \
       ../matrix/matrix.c \
       ../matrix/ops.c

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra `pkg-config --cflags gtk+-3.0 sdl2` -I../ocr/tools -I../matrix
LDFLAGS = `pkg-config --libs gtk+-3.0 sdl2` -lm

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
