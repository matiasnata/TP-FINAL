#include "pa2m.h"
#include "../src/tp1.h"
#include "../src/menu.h"
#include "../src/interfaz_grafica.h"
#include "../src/interfaz_consola.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ARCHIVO_PRUEBA "pruebas/pokedex_prueba.csv"

static bool accion_continuar(void *contexto)
{
	if (contexto != NULL) {
		int *contador = (int *)contexto;
		(*contador)++;
	}
	return true;
}

static bool accion_finalizar(void *contexto)
{
	(void)contexto;
	return false;
}

static bool iterador_menu_contar_todos(char clave, const char *descripcion,
				       size_t indice, void *extra)
{
	(void)clave;
	(void)descripcion;
	(void)indice;
	if (!extra) {
		int *contador = (int *)extra;
		(*contador)++;
	}
	return true;
}

static bool iterador_menu_cortar_en_uno(char clave, const char *descripcion,
					size_t indice, void *extra)
{
	(void)clave;
	(void)descripcion;
	(void)indice;
	int *contador = (int *)extra;
	(*contador)++;
	return ((*contador) < 1);
}

static bool iterador_tp1_contar_todos(struct pokemon *pokemon, void *extra)
{
	(void)pokemon;
	if (extra != NULL) {
		int *contador = (int *)extra;
		(*contador)++;
	}
	return true;
}

static bool iterador_tp1_cortar_en_uno(struct pokemon *pokemon, void *extra)
{
	(void)pokemon;
	int *contador = (int *)extra;
	(*contador)++;
	return ((*contador) < 1);
}

void crear_menu_devuelve_puntero_valido()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(menu != NULL, "Crear un menu devuelve un puntero valido");
	menu_destruir(menu);
}

void crear_menu_inicia_con_cantidad_cero()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(menu_cantidad(menu) == 0,
		     "Un menu recien creado tiene cantidad 0");
	menu_destruir(menu);
}

void pedir_cantidad_menu_nulo_devuelve_cero()
{
	pa2m_afirmar(menu_cantidad(NULL) == 0,
		     "Pedir la cantidad de un menu nulo devuelve 0");
}

void agregar_opcion_valida_devuelve_true()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(menu_agregar_opcion(menu, 'J', "Jugar",
					 accion_continuar) == true,
		     "Agregar una opcion valida al menu devuelve true");
	menu_destruir(menu);
}

void agregar_opcion_valida_incrementa_cantidad()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	pa2m_afirmar(menu_cantidad(menu) == 1,
		     "Agregar una opcion actualiza la cantidad del menu a 1");
	menu_destruir(menu);
}

void agregar_opcion_en_menu_nulo_devuelve_false()
{
	pa2m_afirmar(menu_agregar_opcion(NULL, 'J', "Jugar",
					 accion_continuar) == false,
		     "Agregar una opcion en un menu nulo devuelve false");
}

void agregar_opcion_con_descripcion_nula_devuelve_false()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(menu_agregar_opcion(menu, 'J', NULL, accion_continuar) ==
			     false,
		     "Agregar una opcion con descripcion nula devuelve false");
	menu_destruir(menu);
}

void agregar_opcion_con_accion_nula_devuelve_false()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(menu_agregar_opcion(menu, 'J', "Jugar", NULL) == false,
		     "Agregar una opcion con accion nula devuelve false");
	menu_destruir(menu);
}

void agregar_opcion_con_clave_duplicada_devuelve_false()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	pa2m_afirmar(menu_agregar_opcion(menu, 'J', "Otra", accion_continuar) ==
			     false,
		     "No se puede agregar una opcion con clave repetida");
	menu_destruir(menu);
}

void agregar_opcion_con_clave_duplicada_minuscula_devuelve_false()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	pa2m_afirmar(
		menu_agregar_opcion(menu, 'j', "Otra", accion_continuar) ==
			false,
		"No se puede agregar una clave repetida en minuscula (case-insensitive)");
	menu_destruir(menu);
}

void contiene_clave_existente_en_minuscula_devuelve_true()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	pa2m_afirmar(menu_contiene_clave(menu, 'j') == true,
		     "Buscar una clave registrada en minuscula devuelve true");
	menu_destruir(menu);
}

void contiene_clave_inexistente_devuelve_false()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	pa2m_afirmar(menu_contiene_clave(menu, 'X') == false,
		     "Buscar una clave inexistente en el menu devuelve false");
	menu_destruir(menu);
}

void contiene_clave_en_menu_nulo_devuelve_false()
{
	pa2m_afirmar(menu_contiene_clave(NULL, 'J') == false,
		     "Buscar una clave en un menu nulo devuelve false");
}

void ejecutar_clave_existente_invoca_accion()
{
	menu_t *menu = menu_crear();
	int contador = 0;
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	menu_ejecutar_clave(menu, 'j', &contador);
	pa2m_afirmar(contador == 1,
		     "Ejecutar una clave existente invoca su accion asociada");
	menu_destruir(menu);
}

void ejecutar_clave_salir_devuelve_false()
{
	menu_t *menu = menu_crear();
	menu_agregar_opcion(menu, 'Q', "Salir", accion_finalizar);
	pa2m_afirmar(menu_ejecutar_clave(menu, 'Q', NULL) == false,
		     "Ejecutar una accion de salida devuelve false");
	menu_destruir(menu);
}

void ejecutar_clave_inexistente_devuelve_true()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(menu_ejecutar_clave(menu, 'Z', NULL) == true,
		     "Ejecutar una clave inexistente no falla y devuelve true");
	menu_destruir(menu);
}

void ejecutar_indice_valido_invoca_accion()
{
	menu_t *menu = menu_crear();
	int contador = 0;
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	menu_ejecutar_indice(menu, 0, &contador);
	pa2m_afirmar(contador == 1,
		     "Ejecutar un indice valido invoca su accion asociada");
	menu_destruir(menu);
}

void ejecutar_indice_fuera_de_rango_devuelve_true()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(menu_ejecutar_indice(menu, 5, NULL) == true,
		     "Ejecutar un indice fuera de rango devuelve true");
	menu_destruir(menu);
}

void iterar_menu_recorre_todas_las_opciones()
{
	menu_t *menu = menu_crear();
	int contador = 0;
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	menu_agregar_opcion(menu, 'S', "Semilla", accion_continuar);
	menu_agregar_opcion(menu, 'Q', "Salir", accion_finalizar);

	size_t iteradas = menu_con_cada_opcion(menu, iterador_menu_contar_todos,
					       &contador);
	pa2m_afirmar(iteradas == 3,
		     "El iterador del menu recorre las 3 opciones cargadas");
	menu_destruir(menu);
}

void iterar_menu_corta_cuando_funcion_devuelve_false()
{
	menu_t *menu = menu_crear();
	int contador = 0;
	menu_agregar_opcion(menu, 'J', "Jugar", accion_continuar);
	menu_agregar_opcion(menu, 'S', "Semilla", accion_continuar);

	size_t iteradas = menu_con_cada_opcion(
		menu, iterador_menu_cortar_en_uno, &contador);
	pa2m_afirmar(
		iteradas == 1,
		"El iterador del menu corta cuando la funcion devuelve false");
	menu_destruir(menu);
}

void iterar_menu_nulo_devuelve_cero()
{
	pa2m_afirmar(menu_con_cada_opcion(NULL, iterador_menu_contar_todos,
					  NULL) == 0,
		     "Iterar un menu nulo devuelve 0 elementos recorridos");
}

void leer_archivo_nulo_devuelve_null()
{
	pa2m_afirmar(tp1_leer_archivo(NULL) == NULL,
		     "Leer un archivo con nombre NULL devuelve NULL");
}

void leer_archivo_inexistente_devuelve_null()
{
	tp1_t *tp1 = tp1_leer_archivo("no_existe_este_archivo.csv");
	pa2m_afirmar(tp1 == NULL, "Leer un archivo inexistente devuelve NULL");
}

void leer_archivo_valido_devuelve_tp1_no_nulo()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(tp1 != NULL,
		     "Leer un archivo CSV valido devuelve un puntero no nulo");
	tp1_destruir(tp1);
}

void leer_archivo_con_lineas_invalidas_y_duplicadas_filtra_correctamente()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(
		tp1_cantidad(tp1) == 3,
		"Leer archivo ignora duplicados y lineas invalidas contabilizando solo validos");
	tp1_destruir(tp1);
}

void pedir_cantidad_tp1_nulo_devuelve_cero()
{
	pa2m_afirmar(tp1_cantidad(NULL) == 0,
		     "Pedir la cantidad de un tp1 nulo devuelve 0");
}

void buscar_pokemon_existente_devuelve_datos_correctos()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	struct pokemon *encontrado = tp1_buscar_pokemon(tp1, "Abra");
	pa2m_afirmar(encontrado != NULL,
		     "Buscar un pokemon existente devuelve un puntero valido");
	tp1_destruir(tp1);
}

void buscar_pokemon_ignora_mayusculas_y_minusculas()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	struct pokemon *encontrado = tp1_buscar_pokemon(tp1, "aBrA");
	pa2m_afirmar(
		encontrado != NULL,
		"Buscar un pokemon por nombre ignora mayusculas y minusculas");
	tp1_destruir(tp1);
}

void buscar_pokemon_inexistente_devuelve_null()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(tp1_buscar_pokemon(tp1, "Bulbasaur") == NULL,
		     "Buscar un pokemon inexistente devuelve NULL");
	tp1_destruir(tp1);
}

void buscar_pokemon_en_tp1_nulo_devuelve_null()
{
	pa2m_afirmar(tp1_buscar_pokemon(NULL, "Abra") == NULL,
		     "Buscar un pokemon en un tp1 nulo devuelve NULL");
}

void buscar_pokemon_con_nombre_nulo_devuelve_null()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(tp1_buscar_pokemon(tp1, NULL) == NULL,
		     "Buscar un pokemon con nombre NULL devuelve NULL");
	tp1_destruir(tp1);
}

void buscar_orden_cero_devuelve_primer_pokemon_alfabetico()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	struct pokemon *primero = tp1_buscar_orden(tp1, 0);
	pa2m_afirmar(strcmp(primero->nombre, "Abra") == 0,
		     "El pokemon en el orden 0 es el primero alfabeticamente");
	tp1_destruir(tp1);
}

void buscar_orden_fuera_de_rango_devuelve_null()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(
		tp1_buscar_orden(tp1, 10) == NULL,
		"Buscar un orden mayor o igual a la cantidad devuelve NULL");
	tp1_destruir(tp1);
}

void buscar_orden_en_tp1_nulo_devuelve_null()
{
	pa2m_afirmar(tp1_buscar_orden(NULL, 0) == NULL,
		     "Buscar por orden en un tp1 nulo devuelve NULL");
}

void combinar_dos_tp1_une_pokemones_sin_repetir()
{
	tp1_t *tp1_a = tp1_leer_archivo(ARCHIVO_PRUEBA);
	tp1_t *tp1_b = tp1_leer_archivo(ARCHIVO_PRUEBA);
	tp1_t *combinado = tp1_combinar(tp1_a, tp1_b);

	pa2m_afirmar(
		tp1_cantidad(combinado) == 3,
		"Combinar dos TP1 con los mismos pokemones no duplica elementos");
	tp1_destruir(combinado);
	tp1_destruir(tp1_a);
	tp1_destruir(tp1_b);
}

void combinar_con_tp1_nulo_devuelve_null()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(tp1_combinar(tp1, NULL) == NULL,
		     "Combinar un TP1 valido con NULL devuelve NULL");
	tp1_destruir(tp1);
}

void iterar_tp1_recorre_todos_los_pokemones()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	int contador = 0;

	size_t iterados = tp1_iterar(tp1, iterador_tp1_contar_todos, &contador);
	pa2m_afirmar(iterados == 3,
		     "El iterador de TP1 recorre todos los pokemones cargados");

	tp1_destruir(tp1);
}

void iterar_tp1_corta_cuando_funcion_devuelve_false()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	int contador = 0;

	size_t iterados =
		tp1_iterar(tp1, iterador_tp1_cortar_en_uno, &contador);
	pa2m_afirmar(
		iterados == 1,
		"El iterador de TP1 corta inmediatamente cuando la funcion devuelve false");

	tp1_destruir(tp1);
}

void iterar_tp1_con_funcion_nula_devuelve_cero()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(tp1_iterar(tp1, NULL, NULL) == 0,
		     "Iterar un TP1 con funcion NULL devuelve 0");
	tp1_destruir(tp1);
}

void ejecutar_modo_grafico_con_pokedex_nula_devuelve_false()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(ejecutar_modo_grafico(NULL, menu, NULL) == false,
		     "Ejecutar modo grafico con pokedex nula devuelve false");
	menu_destruir(menu);
}

void ejecutar_modo_grafico_con_menu_nulo_devuelve_false()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(ejecutar_modo_grafico(tp1, NULL, NULL) == false,
		     "Ejecutar modo grafico con menu nulo devuelve false");
	tp1_destruir(tp1);
}

void ejecutar_modo_consola_con_pokedex_nula_devuelve_false()
{
	menu_t *menu = menu_crear();
	pa2m_afirmar(ejecutar_modo_consola(NULL, menu) == false,
		     "Ejecutar modo consola con pokedex nula devuelve false");
	menu_destruir(menu);
}

void ejecutar_modo_consola_con_menu_nulo_devuelve_false()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(ejecutar_modo_consola(tp1, NULL) == false,
		     "Ejecutar modo consola con menu nulo devuelve false");
	tp1_destruir(tp1);
}

void ejecutar_modo_grafico_con_ambos_parametros_nulos_devuelve_false()
{
	pa2m_afirmar(
		ejecutar_modo_grafico(NULL, NULL, "8080") == false,
		"Ejecutar modo grafico con pokedex y menu nulos devuelve false");
}

void ejecutar_modo_consola_con_ambos_parametros_nulos_devuelve_false()
{
	pa2m_afirmar(
		ejecutar_modo_consola(NULL, NULL) == false,
		"Ejecutar modo consola con pokedex y menu nulos devuelve false");
}

void ejecutar_modo_grafico_con_puerto_explicito_y_menu_nulo_devuelve_false()
{
	tp1_t *tp1 = tp1_leer_archivo(ARCHIVO_PRUEBA);
	pa2m_afirmar(
		ejecutar_modo_grafico(tp1, NULL, "9090") == false,
		"Ejecutar modo grafico con puerto valido pero menu nulo devuelve false");
	tp1_destruir(tp1);
}

int main()
{
	pa2m_nuevo_grupo(" PRUEBAS DE MENU: CREACION Y CANTIDAD ");
	crear_menu_devuelve_puntero_valido();
	crear_menu_inicia_con_cantidad_cero();
	pedir_cantidad_menu_nulo_devuelve_cero();

	pa2m_nuevo_grupo(" PRUEBAS DE MENU: INSERCION Y CLAVES ");
	agregar_opcion_valida_devuelve_true();
	agregar_opcion_valida_incrementa_cantidad();
	agregar_opcion_en_menu_nulo_devuelve_false();
	agregar_opcion_con_descripcion_nula_devuelve_false();
	agregar_opcion_con_accion_nula_devuelve_false();
	agregar_opcion_con_clave_duplicada_devuelve_false();
	agregar_opcion_con_clave_duplicada_minuscula_devuelve_false();
	contiene_clave_existente_en_minuscula_devuelve_true();
	contiene_clave_inexistente_devuelve_false();
	contiene_clave_en_menu_nulo_devuelve_false();

	pa2m_nuevo_grupo(" PRUEBAS DE MENU: EJECUCION E ITERADOR ");
	ejecutar_clave_existente_invoca_accion();
	ejecutar_clave_salir_devuelve_false();
	ejecutar_clave_inexistente_devuelve_true();
	ejecutar_indice_valido_invoca_accion();
	ejecutar_indice_fuera_de_rango_devuelve_true();
	iterar_menu_recorre_todas_las_opciones();
	iterar_menu_corta_cuando_funcion_devuelve_false();
	iterar_menu_nulo_devuelve_cero();

	pa2m_nuevo_grupo(" PRUEBAS DE TP1: LECTURA Y CANTIDAD ");
	leer_archivo_nulo_devuelve_null();
	leer_archivo_inexistente_devuelve_null();
	leer_archivo_valido_devuelve_tp1_no_nulo();
	leer_archivo_con_lineas_invalidas_y_duplicadas_filtra_correctamente();
	pedir_cantidad_tp1_nulo_devuelve_cero();

	pa2m_nuevo_grupo(" PRUEBAS DE TP1: BUSQUEDA POR NOMBRE Y ORDEN ");
	buscar_pokemon_existente_devuelve_datos_correctos();
	buscar_pokemon_ignora_mayusculas_y_minusculas();
	buscar_pokemon_inexistente_devuelve_null();
	buscar_pokemon_en_tp1_nulo_devuelve_null();
	buscar_pokemon_con_nombre_nulo_devuelve_null();
	buscar_orden_cero_devuelve_primer_pokemon_alfabetico();
	buscar_orden_fuera_de_rango_devuelve_null();
	buscar_orden_en_tp1_nulo_devuelve_null();

	pa2m_nuevo_grupo(" PRUEBAS DE TP1: COMBINAR, ESCRIBIR E ITERAR ");
	combinar_dos_tp1_une_pokemones_sin_repetir();
	combinar_con_tp1_nulo_devuelve_null();
	iterar_tp1_recorre_todos_los_pokemones();
	iterar_tp1_corta_cuando_funcion_devuelve_false();
	iterar_tp1_con_funcion_nula_devuelve_cero();

	pa2m_nuevo_grupo(" PRUEBAS DE INTERFACES: PARAMETROS INVALIDOS ");
	ejecutar_modo_grafico_con_pokedex_nula_devuelve_false();
	ejecutar_modo_grafico_con_menu_nulo_devuelve_false();
	ejecutar_modo_consola_con_pokedex_nula_devuelve_false();
	ejecutar_modo_consola_con_menu_nulo_devuelve_false();
	ejecutar_modo_grafico_con_ambos_parametros_nulos_devuelve_false();
	ejecutar_modo_consola_con_ambos_parametros_nulos_devuelve_false();
	ejecutar_modo_grafico_con_puerto_explicito_y_menu_nulo_devuelve_false();

	return pa2m_mostrar_reporte();
}