#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lista.h"

#define ERROR -1
#define SON_IGUALES 0

typedef struct nodo {
	void *dato;
	struct nodo *siguiente_nodo;
} nodo_t;

struct lista {
	nodo_t *nodo_inicio;
	nodo_t *nodo_final;
	size_t cantidad;
};

struct lista_iterador {
	nodo_t *nodo_actual;
};

/*
 * La lista debe existir y la posición tiene que ser menor a la cantidad de elementos de la lista.
 * Recorre la lista hasta alcanzar la posición buscada.
 * 
 * Devuelve un puntero al nodo ubicado en la posición buscada.
 */
nodo_t *obtener_nodo(lista_t *lista, size_t posicion)
{
	nodo_t *nodo_actual = lista->nodo_inicio;
	for (size_t i = 0; i < posicion; i++) {
		nodo_actual = nodo_actual->siguiente_nodo;
	}
	return nodo_actual;
}

/*
 * La lista existe y no está vacía. El nodo es válido.
 *
 * Inserta un nodo previamente creado en la primera posición de la lista.
 */
void insertar_al_principio(lista_t *lista, nodo_t *nodo_nuevo)
{
	nodo_nuevo->siguiente_nodo = lista->nodo_inicio;
	lista->nodo_inicio = nodo_nuevo;
}

/*
 * La lista existe y no está vacía. El nodo es válido.
 *
 * Inserta un nodo previamente creado en la última posición de la lista.
 */
void insertar_final(lista_t *lista, nodo_t *nodo_nuevo)
{
	lista->nodo_final->siguiente_nodo = nodo_nuevo;
	nodo_nuevo->siguiente_nodo = NULL;
	lista->nodo_final = nodo_nuevo;
}

/*
 * La lista existe, el nodo es válido y la posición está entre 1 y cantidad - 1.
 * 
 * Inserta un nodo previamente creado en la posición indicada.
 */
void insertar_en_posicion(lista_t *lista, nodo_t *nodo_nuevo, size_t posicion)
{
	nodo_t *nodo_anterior = obtener_nodo(lista, posicion - 1);
	nodo_nuevo->siguiente_nodo = nodo_anterior->siguiente_nodo;
	nodo_anterior->siguiente_nodo = nodo_nuevo;
}

/*
 * La lista existe y no está vacía.
 * 
 * Elimina el primer nodo de la lista, liberando la memoria del mismo.
 * Devuelve el dato que estaba almacenado en el nodo eliminado.
 */
void *eliminar_inicio(lista_t *lista)
{
	void *dato = lista->nodo_inicio->dato;
	nodo_t *nodo_aux = lista->nodo_inicio;
	lista->nodo_inicio = nodo_aux->siguiente_nodo;
	free(nodo_aux);
	if (lista->cantidad == 1) {
		lista->nodo_final = NULL;
	}
	return dato;
}

/*
 * La lista existe, no está vacía y la posición está entre 1 y cantidad - 1.
 *
 * Elimina el nodo ubicado en la posición indicada, liberando su memoria.
 * Devuelve el dato que estaba almacenado en el nodo eliminado.
 */
void *eliminar_posicion(lista_t *lista, size_t posicion)
{
	nodo_t *nodo_anterior = obtener_nodo(lista, posicion - 1);
	nodo_t *nodo_a_eliminar = nodo_anterior->siguiente_nodo;
	void *dato = nodo_a_eliminar->dato;
	nodo_anterior->siguiente_nodo = nodo_a_eliminar->siguiente_nodo;
	free(nodo_a_eliminar);
	if (nodo_anterior->siguiente_nodo == NULL) {
		lista->nodo_final = nodo_anterior;
	}
	return dato;
}

/*
 * Crea un nuevo nodo reservando la memoria necesaria y asignando el dato recibido por parámetro
 *
 * Devuelve un puntero al nodo creado, o NULL en caso de fallar la reserva de memoria
 */
nodo_t *nodo_crear(void *dato)
{
	nodo_t *nodo_nuevo = calloc(1, sizeof(nodo_t));
	if (!nodo_nuevo) {
		return NULL;
	}
	nodo_nuevo->dato = dato;
	return nodo_nuevo;
}

lista_t *lista_crear()
{
	lista_t *lista = calloc(1, sizeof(lista_t));
	if (!lista) {
		return NULL;
	}
	return lista;
}

size_t lista_cantidad(lista_t *lista)
{
	if (!lista) {
		return 0;
	}
	return lista->cantidad;
}

bool lista_esta_vacia(lista_t *lista)
{
	if (!lista) {
		return true;
	}
	return lista_cantidad(lista) == 0;
}

bool lista_insertar(lista_t *lista, void *dato, size_t posicion)
{
	if ((!lista) || (posicion > lista_cantidad(lista))) {
		return false;
	}
	nodo_t *nodo_nuevo = nodo_crear(dato);
	if (!nodo_nuevo) {
		return false;
	}
	nodo_nuevo->dato = dato;
	if (lista->cantidad == 0) {
		lista->nodo_inicio = nodo_nuevo;
		lista->nodo_final = nodo_nuevo;
	} else if (posicion == 0) {
		insertar_al_principio(lista, nodo_nuevo);
	} else if (posicion == lista->cantidad) {
		insertar_final(lista, nodo_nuevo);
	} else {
		insertar_en_posicion(lista, nodo_nuevo, posicion);
	}
	lista->cantidad++;
	return true;
}

void *lista_eliminar(lista_t *lista, size_t posicion)
{
	if ((!lista) || (posicion >= lista_cantidad(lista)) ||
	    (lista_esta_vacia(lista))) {
		return NULL;
	}
	void *dato_nodo = NULL;
	if (posicion == 0) {
		dato_nodo = eliminar_inicio(lista);
	} else {
		dato_nodo = eliminar_posicion(lista, posicion);
	}
	lista->cantidad--;
	return dato_nodo;
}

void *lista_reemplazar(lista_t *lista, void *dato, size_t posicion)
{
	if ((!lista) || (posicion >= lista->cantidad)) {
		return NULL;
	}
	nodo_t *nodo_a_reemplzar = obtener_nodo(lista, posicion);
	void *dato_aux = nodo_a_reemplzar->dato;
	nodo_a_reemplzar->dato = dato;
	return dato_aux;
}

void *lista_obtener(lista_t *lista, size_t posicion)
{
	if ((!lista) || (posicion >= lista->cantidad)) {
		return NULL;
	}
	nodo_t *nodo_buscado = obtener_nodo(lista, posicion);
	return nodo_buscado->dato;
}

int lista_buscar(lista_t *lista, void *buscado,
		 int (*comparador)(void *, void *), void **encontrado)
{
	if (encontrado != NULL) {
		*encontrado = NULL;
	}
	if ((!lista) || (!comparador)) {
		return ERROR;
	}
	bool dato_encontrado = false;
	nodo_t *nodo_actual = lista->nodo_inicio;
	int indice = ERROR;
	int i = 0;
	while ((nodo_actual) && (!dato_encontrado)) {
		if (comparador(nodo_actual->dato, buscado) == SON_IGUALES) {
			indice = i;
			dato_encontrado = true;
			if (encontrado) {
				*encontrado = nodo_actual->dato;
			}
		}
		i++;
		nodo_actual = nodo_actual->siguiente_nodo;
	}
	return indice;
}

size_t lista_iterar(lista_t *lista, bool (*f)(void *, void *), void *extra)
{
	if ((!lista) || (!f)) {
		return 0;
	}
	size_t contador = 0;
	bool seguir_iterando = true;
	nodo_t *nodo_actual = lista->nodo_inicio;
	while ((nodo_actual) && (seguir_iterando)) {
		seguir_iterando = f(nodo_actual->dato, extra);
		nodo_actual = nodo_actual->siguiente_nodo;
		contador++;
	}
	return contador;
}

void lista_destruir(lista_t *lista)
{
	if (!lista) {
		return;
	}
	nodo_t *nodo_actual = lista->nodo_inicio;
	nodo_t *nodo_aux = lista->nodo_inicio;
	while (nodo_actual) {
		nodo_aux = nodo_actual;
		nodo_actual = nodo_actual->siguiente_nodo;
		free(nodo_aux);
	}
	free(lista);
}

void lista_destruir_todo(lista_t *lista, void (*destructor)(void *))
{
	if (!lista) {
		return;
	}
	nodo_t *nodo_actual = lista->nodo_inicio;
	nodo_t *nodo_aux = lista->nodo_inicio;
	while (nodo_actual) {
		nodo_aux = nodo_actual;
		nodo_actual = nodo_actual->siguiente_nodo;
		if (destructor != NULL) {
			destructor(nodo_aux->dato);
		}
		free(nodo_aux);
	}
	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista) {
		return NULL;
	}
	lista_iterador_t *lista_iterador = malloc(sizeof(lista_iterador_t));
	if (!lista_iterador) {
		return NULL;
	}
	lista_iterador->nodo_actual = lista->nodo_inicio;
	return lista_iterador;
}

bool lista_iterador_se_puede_iterar(lista_iterador_t *it)
{
	if (!it) {
		return false;
	}
	return (it->nodo_actual != NULL);
}

void lista_iterador_siguiente(lista_iterador_t *it)
{
	if ((!it) || (!it->nodo_actual)) {
		return;
	}
	it->nodo_actual = it->nodo_actual->siguiente_nodo;
}

void *lista_iterador_obtener_elemento(lista_iterador_t *it)
{
	if ((!it) || (!it->nodo_actual)) {
		return NULL;
	}
	return it->nodo_actual->dato;
}

void lista_iterador_destruir(lista_iterador_t *it)
{
	if (!it) {
		return;
	}
	free(it);
}