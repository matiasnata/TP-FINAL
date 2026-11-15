#include "pokedex_util.h"
#include <stdbool.h>

/**
 * Se asume que candidato es un puntero válido (no nulo) a un struct pokemon.
 *
 * Compara la velocidad entre el pokémon actual y el candidato según el
 * criterio indicado por buscar_mayor (true para el más veloz, false para el menos veloz).
 *
 * Devuelve el puntero al pokémon elegido (o candidato si actual es NULL).
 */
struct pokemon *elegir_por_velocidad(struct pokemon *actual,
				     struct pokemon *candidato,
				     bool buscar_mayor)
{
	if (!actual) {
		return candidato;
	}
	if ((buscar_mayor) && (candidato->velocidad > actual->velocidad)) {
		return candidato;
	}
	if ((!buscar_mayor) && (candidato->velocidad < actual->velocidad)) {
		return candidato;
	}
	return actual;
}

/**
 * Se asume que candidato es un puntero válido (no nulo) a un struct pokemon.
 *
 * Compara el peso entre el pokémon actual y el candidato según el
 * criterio indicado por buscar_mayor (true para el más pesado, false para el menos pesado).
 *
 * Devuelve el puntero al pokémon elegido (o candidato si actual es NULL).
 */
struct pokemon *elegir_por_peso(struct pokemon *actual,
				struct pokemon *candidato, bool buscar_mayor)
{
	if (!actual) {
		return candidato;
	}
	if ((buscar_mayor) && (candidato->peso > actual->peso)) {
		return candidato;
	}
	if ((!buscar_mayor) && (candidato->peso < actual->peso)) {
		return candidato;
	}
	return actual;
}

/**
 * Se asume que pokemon y extra son punteros válidos (no nulos), donde extra
 * apunta a una estructura struct resumen_pokedex.
 *
 * Actualiza los contadores totales y por rareza del resumen, así como las
 * referencias a los pokemones con velocidad y peso extremos.
 *
 * Siempre devuelve true para continuar la iteración sobre toda la pokedex.
 */
bool agregar_al_resumen(struct pokemon *pokemon, void *extra)
{
	struct resumen_pokedex *resumen = extra;
	resumen->total++;
	if (pokemon->rareza == RAREZA_COMUN) {
		resumen->comunes++;
	} else if (pokemon->rareza == RAREZA_RARO) {
		resumen->raros++;
	} else {
		resumen->legendarios++;
	}
	resumen->mas_veloz =
		elegir_por_velocidad(resumen->mas_veloz, pokemon, true);
	resumen->menos_veloz =
		elegir_por_velocidad(resumen->menos_veloz, pokemon, false);
	resumen->mas_pesado =
		elegir_por_peso(resumen->mas_pesado, pokemon, true);
	resumen->menos_pesado =
		elegir_por_peso(resumen->menos_pesado, pokemon, false);
	return true;
}

struct resumen_pokedex calcular_resumen_pokedex(tp1_t *pokedex)
{
	struct resumen_pokedex resumen = {0};
	if (pokedex != NULL) {
		tp1_iterar(pokedex, agregar_al_resumen, &resumen);
	}
	return resumen;
}