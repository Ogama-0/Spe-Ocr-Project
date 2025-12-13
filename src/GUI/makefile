# Nom de l'exécutable
TARGET = ocr_gui

# Fichiers source
SRCS = ocr_gui.c \
       ../pretreatment/manual_rotate.c \
       ../pretreatment/preprocess.c \
       ../pretreatment/binarize.c \
       ../pretreatment/denoise.c \
       ../pretreatment/blocks_treatment.c

# Compilateur
CC = gcc

# Flags GTK3 + SDL2 + warnings
CFLAGS = -Wall -Wextra `pkg-config --cflags gtk+-3.0 sdl2`
LDFLAGS = `pkg-config --libs gtk+-3.0 sdl2` -lm

# Règle par défaut
all: $(TARGET)

# Compilation
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Nettoyage
clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
