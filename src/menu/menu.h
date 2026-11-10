#ifndef MENU_H_
#define MENU_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct menu menu_t;

/**
 *
 * Recibe un puntero genérico al contexto de la aplicación.
 *
 * Devuelve true si la aplicación debe continuar ejecutándose, o false si la acción
 * indica que debe finalizar el programa.
 */
typedef bool (*menu_accion_t)(void *contexto);

/**
 * Devuelve true si se debe continuar con la iteración, false caso contrario.
 */
typedef bool (*menu_iterador_t)(char clave, const char *descripcion, size_t indice, void *extra);

/**
 * Crea un menú vacío.
 *
 * Devuelve un puntero al nuevo menú, o NULL en caso de fallo de memoria.
 */
menu_t *menu_crear(void);

/**
 * Agrega una nueva opción al menú.
 *
 * Se almacena una copia interna de la descripción. La clave no debe existir previamente
 * en el menú (se normaliza insensible a mayúsculas/minúsculas).
 *
 * Devuelve true si la opción se agregó correctamente, o false si ocurrió un error
 *
 */
bool menu_agregar_opcion(menu_t *menu, char clave, const char *descripcion, menu_accion_t accion);

/**
 * Ejecuta la acción vinculada al carácter clave dado.
 *
 * Si la clave no se
 * encuentra en el menú, no realiza ninguna acción y retorna true para continuar.
 *
 * Devuelve el valor de retorno de la acción ejecutada, o true si el menú es nulo
 * o la opción no existe.
 */
bool menu_ejecutar_clave(menu_t *menu, char clave, void *contexto);

/**
 * Ejecuta la acción vinculada a la opción en la posición 'indice' (0 <= indice < cantidad).
 *
 * Devuelve el valor de retorno de la acción ejecutada, o true si el índice está
 * fuera de rango o el menú es nulo.
 */
bool menu_ejecutar_indice(menu_t *menu, size_t indice, void *contexto);

/**
 * Recorre secuencialmente todas las opciones del menú invocando la función 'f'.
 *
 * Devuelve la cantidad de opciones procesadas por el callback.
 */
size_t menu_con_cada_opcion(menu_t *menu, menu_iterador_t f, void *extra);

/**
 * Devuelve la cantidad de opciones almacenadas en el menú.
 */
size_t menu_cantidad(menu_t *menu);

/**
 * Devuelve true si la clave está registrada en el mnu o false en caso contrario.
 */
bool menu_contiene_clave(menu_t *menu, char clave);

/**
 * Destruye el menú y libera toda la memoria dinámica asociada.
 */
void menu_destruir(menu_t *menu);

#endif // MENU_H_