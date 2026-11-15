#ifndef VALIDADORES_H_
#define VALIDADORES_H_

#include <stdbool.h>
#include "tp1.h"

/**
 * Valida si la cadena representa un peso válido (número flotante mayor a 0.0).
 * En caso de éxito, asigna el valor convertido en *peso.
 *
 * Devuelve true si el formato es correcto y el valor es mayor a 0.0,
 * o false en caso contrario.
 */
bool peso_valido(char *str, float *peso);

/**
 * Valida si la cadena representa una velocidad válida (entero en el rango [1, 99]).
 * En caso de éxito, actualiza el valor de *velocidad.
 *
 * Devuelve true si el formato y el rango son válidos, o false en caso contrario.
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
 * Verifica que el nombre del pokémon no sea nulo ni vacío.
 *
 * Devuelve true si la cadena no es nula y contiene al menos un carácter,
 * o false en caso contrario.
 */
bool nombre_valido(const char *nombre);

#endif // VALIDADORES_H_