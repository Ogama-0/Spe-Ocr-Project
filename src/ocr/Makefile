CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -Iinclude -Itools \
         $(shell pkg-config --cflags gdk-pixbuf-2.0)

LDFLAGS = -lm \
          $(shell pkg-config --libs gdk-pixbuf-2.0)

SRC = main.c ocr.c ../matrix/matrix.c ../matrix/ops.c tools/activations.c tools/img.c tools/resize_dataset.c
OBJ = $(SRC:.c=.o)
EXEC = ocr_solver

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

run: $(EXEC)
	@echo "Running the application..."
	@./$(EXEC)
	@echo "Exiting the application."

clean:
	rm -f $(OBJ) $(EXEC)
