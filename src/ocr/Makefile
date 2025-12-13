
CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -Iinclude -Itools \
         $(shell pkg-config --cflags gdk-pixbuf-2.0)

LDFLAGS = -lm \
          $(shell pkg-config --libs gdk-pixbuf-2.0)

SRC     = main.c ocr.c ../matrix/matrix.c ../matrix/ops.c tools/activations.c tools/img.c tools/ext_img.c tools/int_matrix.c
SRCPRED = predict.c ocr.c ../matrix/matrix.c ../matrix/ops.c tools/activations.c tools/img.c tools/ext_img.c tools/int_matrix.c

OBJ     = $(SRC:.c=.o)
OBJPRED = $(SRCPRED:.c=.o)

EXEC = ocr_solver
PRED = predict

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(PRED): $(OBJPRED)
	$(CC) $(OBJPRED) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXEC)
	@echo "Running the application..."
	@./$(EXEC)
	@echo "Exiting the application."

clean:
	rm -f $(OBJ) $(OBJPRED) $(EXEC) $(PRED)
