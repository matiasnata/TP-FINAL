#ifndef INTERFAZ_GRAFICA_H_
#define INTERFAZ_GRAFICA_H_

#include "tp1.h"
#include "menu.h"
#include <stdbool.h>

/**
 * Configura las opciones del menú gráfico y corre el servidor del engine
 * en el puerto indicado hasta que el usuario decida salir.
 *
 * Devuelve true si finalizó normalmente o false en caso de error.
 */
bool ejecutar_modo_grafico(tp1_t *pokedex, menu_t *menu, const char *puerto);

#endif // INTERFAZ_GRAFICA_H_