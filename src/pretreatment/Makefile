CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 `sdl2-config --cflags`
LDFLAGS := `sdl2-config --libs` -lSDL2_image -lSDL2_gfx

SRC     := grayscale.c manual_rotate.c main.c
OBJ     := $(SRC:.c=.o)
TARGET  := preprocessing

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re run
