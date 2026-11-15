#ifndef POKEDEX_UTIL_H_
#define POKEDEX_UTIL_H_

#include "tp1.h"
#include <stdlib.h>

#define CLAVE_JUGAR 'J'
#define CLAVE_SEMILLA 'S'
#define CLAVE_MOSTRAR 'M'
#define CLAVE_INFO 'I'
#define CLAVE_SALIR 'Q'

#define DESC_JUGAR "Jugar"
#define DESC_SEMILLA "Jugar con semilla"
#define DESC_MOSTRAR "Mostrar pokemon"
#define DESC_INFO "Mostrar informacion"
#define DESC_SALIR "Salir"

struct resumen_pokedex {
	size_t total;
	size_t comunes;
	size_t raros;
	size_t legendarios;
	struct pokemon *mas_veloz;
	struct pokemon *menos_veloz;
	struct pokemon *mas_pesado;
	struct pokemon *menos_pesado;
};

/**
 * Recorre todos los pokemones de la pokedex y devuelve un resumen con
 * las cantidades por rareza y los extremos de velocidad y peso.
 */
struct resumen_pokedex calcular_resumen_pokedex(tp1_t *pokedex);

#endif // POKEDEX_UTIL_H_