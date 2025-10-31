CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address
LDFLAGS =

SRC = solver.c
OBJ = ${SRC:.c=.o}
TARGET = solver

all : ${TARGET}

${TARGET} : ${OBJ}
	${CC} ${CFLAGS} ${OBJ} -o ${TARGET} ${LDFLAGS}

clean :
	${RM} solver.o
	${RM} solver
