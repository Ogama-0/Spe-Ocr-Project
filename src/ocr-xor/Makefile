CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude -Itools

LDFLAGS = -lm
SRC = main.c ocr-xor.c ../matrix/matrix.c ../matrix/ops.c tools/activations.c
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
