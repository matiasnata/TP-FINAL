#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "leer_linea.h"

#define ERROR -1
#define TAMANIO_INICIAL 10
#define MULTIPLICADOR_TAMANIO 2
#define ESPACIO_NULO 1
#define CARACTER_NULO '\0'
#define SALTO_DE_LINEA '\n'

int leer_linea_ptr(char **ptr, size_t *tamanio, FILE *archivo)
{
	if ((!ptr) || !(*ptr) || (!tamanio) || (!archivo)) {
		return ERROR;
	}
	int caracter_leido = fgetc(archivo);
	if (caracter_leido == EOF) {
		return ERROR;
	}
	size_t tope = 0;
	bool error_memoria = false;
	while ((caracter_leido != EOF) && (caracter_leido != SALTO_DE_LINEA) &&
	       (!error_memoria)) {
		if ((tope + ESPACIO_NULO) >= *tamanio) {
			size_t nuevo_tamanio =
				(*tamanio) * (MULTIPLICADOR_TAMANIO);
			char *ptr_aux = realloc(*ptr, nuevo_tamanio);
			if (!ptr_aux) {
				error_memoria = true;
			} else {
				*ptr = ptr_aux;
				*tamanio = nuevo_tamanio;
			}
		}
		if (!error_memoria) {
			(*ptr)[tope] = (char)caracter_leido;
			tope++;
			caracter_leido = fgetc(archivo);
		}
	}
	if (error_memoria) {
		return ERROR;
	}
	(*ptr)[tope] = CARACTER_NULO;
	return (int)tope;
}

char *leer_linea(FILE *archivo)
{
	if (!archivo) {
		return NULL;
	}
	size_t tamanio = TAMANIO_INICIAL;
	char *ptr_string = malloc(tamanio * sizeof(char));
	if (!ptr_string) {
		return NULL;
	}
	int resultado = leer_linea_ptr(&ptr_string, &tamanio, archivo);
	if (resultado == ERROR) {
		free(ptr_string);
		return NULL;
	}
	return ptr_string;
}
