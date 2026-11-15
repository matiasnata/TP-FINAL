#include "interfaz_consola.h"
#include "pokedex_util.h"
#include "menu.h"
#include "leer_linea.h"
#include <stdio.h>
#include <stdlib.h>

#define CANT_CARACTER_SOLICITADO 1
#define SALTO_DE_LINEA '\n'
#define INDICE_PRIMER_CARACTER 0
#define INDICE_SEGUNDO_CARACTER 1
#define CARACTER_NULO '\0'

/**
 * Informa por pantalla la salida del programa.
 * 
 * Devuelve false para finalizar el mismo. 
 */
static bool accion_salir()
{
	printf("\nSaliendo del programa...\n");
	return false;
}

/**
 * Informa por pantalla que la opción "juego" no se puede ejecutar por consola.
 * 
 * Devuelve true para continuar con el programa
 */
static bool accion_jugar(void *contexto)
{
	(void)contexto;
	printf("\n[!] El juego interactivo solo se puede ejecutar en modo grafico.\n");
	printf("\nInicie el programa indicando un puerto (ej: ./demo pokedex.csv 8080).\n");
	return true;
}

/**
 * Informa por pantalla que la opción "semilla" no se puede ejecutar por consola.
 * 
 * Devuelve true para continuar con el programa.
 */
static bool accion_semilla(void *contexto)
{
	(void)contexto;
	printf("\n[!] El juego con semilla solo se puede ejecutar en modo grafico.\n");
	printf("\nInicie el programa indicando un puerto (ej: ./demo pokedex.csv 8080).\n");
	return true;
}

/**
 * Se asume que pokemon es un puntero válido (no nulo).
 *
 * Muestra por pantalla todos los atributos del pokémon recibido.
 */
void mostrar_datos_pokemon(struct pokemon *pokemon)
{
	printf("\n--- DATOS DEL POKEMON ---\n");
	printf("Nombre: %s\n", pokemon->nombre);
	printf("Velocidad: %d\n", pokemon->velocidad);
	printf("Peso: %.1f\n", pokemon->peso);
	if (pokemon->rareza == RAREZA_LEGENDARIO) {
		printf("Rareza: Legendario\n");
	} else if (pokemon->rareza == RAREZA_RARO) {
		printf("Rareza: Raro\n");
	} else {
		printf("Rareza: Comun\n");
	}
}

/**
 * El puntero contexto debe apuntar a la estructura tp1_t.
 * 
 * Solicita al usuario el ingreso del pokemon que desea obtener información
 * 
 * Muestra la información del pokemon si existe o un mensaje de error si no existe en la pokedex.
 * 
 * Devuelve true para continuar con el programa.
 */
bool accion_mostrar_pokemon(void *contexto)
{
	tp1_t *pokedex = (tp1_t *)contexto;
	if (tp1_cantidad(pokedex) == 0) {
		printf("\n[!] La pokedex esta vacia. No hay pokemones para buscar.\n");
		return true;
	}
	printf("\nIngrese el nombre del pokemon a buscar: ");
	char *nombre = leer_linea(stdin);
	if (!nombre) {
		return true;
	}
	struct pokemon *pokemon_encontrado =
		tp1_buscar_pokemon(pokedex, nombre);
	if (!pokemon_encontrado) {
		printf("\n[!]Pokemon no encontrado.\n");
	} else {
		mostrar_datos_pokemon(pokemon_encontrado);
	}
	free(nombre);
	return true;
}

/**
 * El puntero contexto debe apuntar a una estructura tp1_t válida
 * 
 * Muestra las estadísticas de la pokedex.
 * 
 * Devuelve true para continuar con el programa.
 */
bool accion_mostrar_informacion(void *contexto)
{
	tp1_t *pokedex = (tp1_t *)contexto;
	struct resumen_pokedex resumen = calcular_resumen_pokedex(pokedex);
	printf("\n--- INFORMACION DE LA POKEDEX ---\n");
	printf("\nTotal de pokemones: %zu\n", resumen.total);
	printf("Comunes: %zu\n", resumen.comunes);
	printf("Raros: %zu\n", resumen.raros);
	printf("Legendarios: %zu\n", resumen.legendarios);
	if (resumen.total > 0) {
		printf("Mas veloz: %s (%d)\n", resumen.mas_veloz->nombre,
		       resumen.mas_veloz->velocidad);
		printf("Menos veloz: %s (%d)\n", resumen.menos_veloz->nombre,
		       resumen.menos_veloz->velocidad);
		printf("Mas pesado: %s (%.1f)\n", resumen.mas_pesado->nombre,
		       resumen.mas_pesado->peso);
		printf("Menos pesado: %s (%.1f)\n",
		       resumen.menos_pesado->nombre,
		       resumen.menos_pesado->peso);
	} else {
		printf("[!] La pokedex no tiene pokemones cargados para calcular estadísticas.\n");
	}
	return true;
}

/**
 * El puntero descripcion es un puntero válido finalizado en '\0'
 * 
 * Imprime por pantalla todas las claves y su descripción correspondiente del menu.
 */
bool imprimir_opcion_consola(char clave, const char *descripcion, size_t indice,
			     void *extra)
{
	(void)indice;
	(void)extra;
	printf("\n  [%c] %s\n", clave, descripcion);
	return true;
}

/**
 * Registra todas las opciones disponibles del menú de consola.
 *
 * Devuelve true si todas las opciones fueron agregadas correctamente,
 * o false en caso de error.
 */
static bool cargar_opciones_consola(menu_t *menu)
{
	if (!menu) {
		return false;
	}
	return (menu_agregar_opcion(menu, CLAVE_JUGAR, DESC_JUGAR,
				    accion_jugar) &&
		menu_agregar_opcion(menu, CLAVE_SEMILLA, DESC_SEMILLA,
				    accion_semilla) &&
		menu_agregar_opcion(menu, CLAVE_MOSTRAR, DESC_MOSTRAR,
				    accion_mostrar_pokemon) &&
		menu_agregar_opcion(menu, CLAVE_INFO, DESC_INFO,
				    accion_mostrar_informacion) &&
		menu_agregar_opcion(menu, CLAVE_SALIR, DESC_SALIR,
				    accion_salir));
}

/**
 * Se asume que menu, pokedex y linea_ingresada son punteros válidos.
 *
 * Evalúa la línea ingresada por el usuario, informa errores de formato o de
 * opción inexistente, o ejecuta la acción asociada si la clave es válida.
 *
 * Devuelve true si el programa debe seguir ejecutándose, o false si debe finalizar.
 */
bool procesar_entrada_menu(menu_t *menu, tp1_t *pokedex,
			   const char *linea_ingresada)
{
	if (linea_ingresada[INDICE_PRIMER_CARACTER] == CARACTER_NULO) {
		printf("\n[!] No ingreso ninguna opcion. Por favor, ingrese una letra del menu.\n");
		return true;
	}
	if (linea_ingresada[INDICE_SEGUNDO_CARACTER] != CARACTER_NULO) {
		printf("\n[!] Formato invalido: debe ingresar unicamente una letra.\n");
		return true;
	}
	char opcion_ingresada = linea_ingresada[INDICE_PRIMER_CARACTER];
	if (!menu_contiene_clave(menu, opcion_ingresada)) {
		printf("\n[!] Opcion '%c' invalida. Ingrese una de las opciones del menu.\n",
		       opcion_ingresada);
		return true;
	}
	return menu_ejecutar_clave(menu, opcion_ingresada, pokedex);
}

bool ejecutar_modo_consola(tp1_t *pokedex, menu_t *menu)
{
	if ((!pokedex) || (!cargar_opciones_consola(menu))) {
		return false;
	}

	bool seguir_ejecutando = true;
	while (seguir_ejecutando) {
		printf("\n=== MENU PRINCIPAL ===\n");
		menu_con_cada_opcion(menu, imprimir_opcion_consola, NULL);
		printf("\nSeleccione una opcion: ");
		char *linea_ingresada = leer_linea(stdin);
		if (!linea_ingresada) {
			printf("\n[!] Fin de entrada detectado. Saliendo del programa...\n");
			seguir_ejecutando = false;
		} else {
			seguir_ejecutando = procesar_entrada_menu(
				menu, pokedex, linea_ingresada);
			free(linea_ingresada);
		}
	}
	return true;
}