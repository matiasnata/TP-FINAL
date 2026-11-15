#ifndef INTERFAZ_CONSOLA_H_
#define INTERFAZ_CONSOLA_H_

#include "tp1.h"
#include <stdbool.h>
#include "menu.h"

/**
 * Configura las opciones del menú de texto y corre el bucle
 * interactivo por consola hasta que el usuario decida salir.
 * Devuelve true si finalizó normalmente o false en caso de error.
 */
bool ejecutar_modo_consola(tp1_t *pokedex, menu_t *menu);

#endif // INTERFAZ_CONSOLA_H_