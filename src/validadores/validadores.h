#ifndef VALIDADORES_H_
#define VALIDADORES_H_

#include <stdbool.h>
#include "tp1.h"

/**
 * Devuelve true si el formato es correcto y el valor es mayor a 0.0,
 * o false en caso contrario.
 */
bool peso_valido(char *str, float *peso);

/**
 * En caso de éxito, actualiza el valor de *velocidad.
 *
 * Devuelve true si el formato y el rango son válidos ([1,99]), o false en caso contrario.
 */
bool velocidad_valida(char *str, int *velocidad);

/**
 * Valida si la cadena representa una rareza válida ('C', 'R' o 'L').
 * En caso de éxito, asigna el valor correspondiente a *rareza.
 *
 * Devuelve true si la cadena corresponde a un carácter de rareza válido,
 * o false en caso contrario.
 */
bool rareza_valida(char *str, enum rareza_pokemon *rareza);

/**
 * Devuelve true si la cadena no es nula y contiene al menos un carácter, 
 * o false en caso contrario.
 */
bool nombre_valido(const char *nombre);

#endif // VALIDADORES_H_