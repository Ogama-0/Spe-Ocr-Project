# Nom de l'exécutable
TARGET = ocr_gui

# Fichiers source
SRCS = ocr_gui.c

# Compilateur
CC = gcc

# Flags de compilation GTK3 + options -Wall -Wextra
CFLAGS = -Wall -Wextra `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Nettoyage des fichiers générés
clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
