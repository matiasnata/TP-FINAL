VALGRIND_FLAGS = --leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes
CFLAGS = -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0 -g
CC = gcc
PUERTO = 8080

SRCS = src/tp1.c \
       src/abb/abb.c \
       src/lista/lista.c \
       src/menu/menu.c \
       src/leer_linea/leer_linea.c \
       src/validadores/validadores.c

all: clean valgrind-demo


demo: main.c extra/engine.c $(SRCS)
	$(CC) $(CFLAGS) -Isrc -I. $^ -o demo

valgrind-demo: demo
	valgrind $(VALGRIND_FLAGS) ./demo $(PUERTO)


pruebas: $(SRCS) pruebas/*.c
	$(CC) $(CFLAGS) -Isrc -I. $^ -o pruebas
	./pruebas

valgrind-pruebas: pruebas
	valgrind $(VALGRIND_FLAGS) ./pruebas


clean:
	rm -f demo pruebas

.PHONY: all demo valgrind-demo pruebas valgrind-pruebas clean