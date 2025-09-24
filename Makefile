CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -std=c11 -Iinclude
SRC = src/main.c src/solver.c src/ocr.c
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
