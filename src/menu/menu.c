#include "menu.h"
#include "../lista/lista.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NO_ENCONTRADO -1

struct opcion {
    char clave;
    char *descripcion;
    menu_accion_t accion;
};

struct menu {
    lista_t *opciones;
};

struct contexto_iterador_menu {
    menu_iterador_t funcion;
    void *extra;
    size_t indice_actual;
};

/**
 * Compara la clave de una struct opcion con un carácter clave buscado.
 * 
 * Devuelve 0 si coinciden, o un valor distinto de 0 en caso contrario.
 */
int comparar_opcion_clave(void *elemento, void *clave_buscada)
{
    struct opcion *opcion = (struct opcion *)elemento;
    char *clave = (char *)clave_buscada;
    return (int)opcion->clave - (int)(*clave);
}

/**
 * Función auxiliar para adaptar lista_iterar a la firma de menu_iterador_t.
 */
bool aplicar_iterador_menu(void *elemento, void *ctx)
{
    struct opcion *op = (struct opcion *)elemento;
    struct contexto_iterador_menu *datos = (struct contexto_iterador_menu *)ctx;
    bool continuar = datos->funcion(op->clave, op->descripcion, datos->indice_actual, datos->extra);
    datos->indice_actual++;
    return continuar;
}

/**
 * Duplica una cadena de caracteres en memoria dinámica.
 * Devuelve un puntero a la nueva cadena o NULL si falla la reserva.
 */
char *duplicar_string(const char *origen)
{
    size_t longitud = strlen(origen);
    char *copia = malloc(longitud + 1);
    if (!copia) {
        return NULL;
    }
    strcpy(copia, origen);
    return copia;
}

/**
 * Libera la memoria ocupada por una opción individual.
 */
void opcion_destruir(void *elemento)
{
    if (!elemento) {
        return;
    }
    struct opcion *op = (struct opcion *)elemento;
    free(op->descripcion);
    free(op);
}

/**
 * Crea una nueva opcion en memoria dinamica asignando su clave, una copia de la
 * descripcion y su accion asociada.
 * Devuelve un puntero a la opcion creada o NULL ante fallos de memoria.
 */
struct opcion *opcion_crear(char clave, const char *descripcion, menu_accion_t accion)
{
    struct opcion *operacion = malloc(sizeof(struct opcion));
    if (!operacion) {
        return NULL;
    }
    char *copia_descripcion = duplicar_string(descripcion);
    if (!copia_descripcion) {
        free(operacion);
        return NULL;
    }
    operacion->clave = clave;
    operacion->descripcion = copia_descripcion;
    operacion->accion = accion;
    return operacion;
}

menu_t *menu_crear()
{
    menu_t *menu = malloc(sizeof(menu_t));
    if (!menu) {
        return NULL;
    }
    menu->opciones = lista_crear();
    if (!menu->opciones) {
        free(menu);
        return NULL;
    }
    return menu;
}

bool menu_agregar_opcion(menu_t *menu, char clave, const char *descripcion, menu_accion_t accion)
{
    if ((!menu) || (!descripcion) || (!accion)) {
        return false;
    }
    char clave_normalizada = (char)toupper((unsigned char)clave);
    if (lista_buscar(menu->opciones, &clave_normalizada, comparar_opcion_clave, NULL) != NO_ENCONTRADO) {
        return false;
    }
    struct opcion *nueva_opcion = opcion_crear(clave_normalizada, descripcion, accion);
    if (!nueva_opcion) {
        return false;
    }
    size_t total = lista_cantidad(menu->opciones);
    if (!lista_insertar(menu->opciones, nueva_opcion, total)) {
        opcion_destruir(nueva_opcion);
        return false;
    }
    return true;
}

bool menu_ejecutar_clave(menu_t *menu, char clave, void *contexto)
{
    if (!menu) {
        return true;
    }
    char clave_normalizada = (char)toupper((unsigned char)clave);
    struct opcion *op = NULL;
    if (lista_buscar(menu->opciones, &clave_normalizada, comparar_opcion_clave, (void **)&op) != NO_ENCONTRADO && op) {
        return op->accion(contexto);
    }
    return true;
}

bool menu_ejecutar_indice(menu_t *menu, size_t indice, void *contexto)
{
    if (!menu) {
        return true;
    }
    struct opcion *opcion = (struct opcion *)lista_obtener(menu->opciones, indice);
    if (!opcion) {
        return true;
    }
    return (opcion->accion(contexto));
}

size_t menu_con_cada_opcion(menu_t *menu, menu_iterador_t f, void *extra)
{
    if ((!menu) || (!f)) {
        return 0;
    }
    struct contexto_iterador_menu contexto = {
        .funcion = f,
        .extra = extra,
        .indice_actual = 0
    };
    return lista_iterar(menu->opciones, aplicar_iterador_menu, &contexto);
}

size_t menu_cantidad(menu_t *menu)
{
    if (!menu) {
        return 0;
    }
    return lista_cantidad(menu->opciones);
}

bool menu_contiene_clave(menu_t *menu, char clave)
{
    if (!menu) {
        return false;
    }
    char clave_normalizada = (char)toupper((unsigned char)clave);
    return (lista_buscar(menu->opciones, &clave_normalizada, comparar_opcion_clave, NULL) != NO_ENCONTRADO);
}

void menu_destruir(menu_t *menu)
{
    if (!menu) {
        return;
    }
    lista_destruir_todo(menu->opciones, opcion_destruir);
    free(menu);
}