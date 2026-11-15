VALGRIND_FLAGS = --leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes
CFLAGS = -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O0 -g -Isrc -Iextra -Ipruebas -I.
CC = gcc
PUERTO = 8080
ARCHIVO = pokedex.csv

SRC = src/tp1.c src/abb.c src/lista.c src/menu.c src/leer_linea.c src/validadores.c src/interfaz_consola.c src/interfaz_grafica.c extra/engine.c src/pokedex_util.c
PRUEBAS_SRC = pruebas/pruebas_alumno.c pruebas/pa2m.c $(SRC)

all: clean demo

demo: main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o demo

valgrind-demo: demo
	valgrind $(VALGRIND_FLAGS) ./demo $(ARCHIVO) $(PUERTO)

pruebas: $(PRUEBAS_SRC)
	$(CC) $(CFLAGS) $(PRUEBAS_SRC) -o pruebas_alumno
	./pruebas_alumno

valgrind-pruebas: $(PRUEBAS_SRC)
	$(CC) $(CFLAGS) $(PRUEBAS_SRC) -o pruebas_alumno
	valgrind $(VALGRIND_FLAGS) ./pruebas_alumno

clean:
	rm -f demo pruebas_alumno