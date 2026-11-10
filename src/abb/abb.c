#include "abb.h"
#include <stddef.h>
#include <stdlib.h>
#include "abb_estructura_privada.h"
#define SON_IGUALES 0

typedef struct {
    void **elementos;
    size_t capacidad_maxima;
    size_t cantidad_guardada;
} datos_array_t;

/**
 * 
 *
 * 
 */
static bool guardar_en_array(void *elemento, void *aux) {
    datos_array_t *datos = (datos_array_t *)aux;
    bool continuar = false;

    if (datos->cantidad_guardada < datos->capacidad_maxima) {
        datos->elementos[datos->cantidad_guardada] = elemento;
        datos->cantidad_guardada++;
        if (datos->cantidad_guardada < datos->capacidad_maxima) {
            continuar = true;
        }
    }

    return continuar;
}

/**
 * Los punteros a funciónn e invocaciones son válidos.
 * Recorre los nodos en inorden e invoca la función con cada elemento.
 * Devuelve true si se debe continuar la iteración o false si se debe detener.
 * Se incrementa el contador de invocaciones en cada llamada.
 */
bool iterar_inorden(nodo_abb_t *nodo, bool (*funcion)(void *, void *), void *aux, size_t *invocaciones) {
    bool continuar = true;
    if (nodo) {
        continuar = iterar_inorden(nodo->izquierda, funcion, aux, invocaciones);
        if (continuar) {
            (*invocaciones)++;
            continuar = funcion(nodo->elemento, aux);
        }
        if (continuar) {
            continuar = iterar_inorden(nodo->derecha, funcion, aux, invocaciones);
        }
    }
    return continuar;
}

/**
 * Los punteros a funciónn e invocaciones son válidos.
 * Recorre los nodos en preorden e invoca la función con cada elemento.
 * Devuelve true si se debe continuar la iteración o false si se debe detener.
 * Se incrementa el contador de invocaciones en cada llamada.
 */
bool iterar_preorden(nodo_abb_t *nodo, bool (*funcion)(void *, void *), void *aux, size_t *invocaciones) {
    bool continuar = true;
    if (nodo) {
        (*invocaciones)++;
        continuar = funcion(nodo->elemento, aux);
        if (continuar) {
            continuar = iterar_preorden(nodo->izquierda, funcion, aux, invocaciones);
        }
        if (continuar) {
            continuar = iterar_preorden(nodo->derecha, funcion, aux, invocaciones);
        }
    }
    return continuar;
}

/**
 * Los punteros a funciónn e invocaciones son válidos.
 * Recorre los nodos en postorden e invoca la función con cada elemento.
 * Devuelve true si se debe continuar la iteración o false si se debe detener.
 * Se incrementa el contador de invocaciones en cada llamada.
 */
bool iterar_postorden(nodo_abb_t *nodo, bool (*funcion)(void *, void *), void *aux, size_t *invocaciones) {
    bool continuar = true;
    if (nodo) {
        continuar = iterar_postorden(nodo->izquierda, funcion, aux, invocaciones);
        if (continuar) {
            continuar = iterar_postorden(nodo->derecha, funcion, aux, invocaciones);
        }
        if (continuar) {
            (*invocaciones)++;
            continuar = funcion(nodo->elemento, aux);
        }
    }
    return continuar;
}

/**
 * Crea un nodo para el árbol binario de búsqueda, y le asigna el elemento recibido por parámetro.
 *
 * Devuelve un puntero al nodo creado o NULL en caso de error.
 */
nodo_abb_t* crear_nodo(void* elemento){
  nodo_abb_t* nodo = calloc (1, sizeof(nodo_abb_t));
  if (!nodo){
    return NULL;
  }
  nodo->elemento = elemento;
  return nodo;
}

/**
 * Busca el elemento del predecesor inorden (el mayor de la rama izquierda).
 * 
 * Se asume que el nodo provisto siempre tiene un hijo izquierdo.
 * 
 * Devuelve el elemento encontrado.
 */
void *buscar_elemento_predecesor(nodo_abb_t *nodo)
{
    nodo_abb_t *actual = nodo->izquierda;
    while (actual->derecha) {
        actual = actual->derecha;
    }
    return actual->elemento;
}

/**
 * Intenta eliminar un nodo del árbol si este se encuentra en los casos
 * simples de eliminación (es un nodo hoja o tiene un único hijo). 
 * 
 * Actualiza el valor apuntado por se_resolvio a true en caso de éxito, 
 * o a false si el nodo posee dos hijos y no pudo ser eliminado.
 *
 * Devuelve el nodo que debe reemplazar al nodo eliminado (su hijo o NULL),
 * o el mismo nodo original si no se aplicó la eliminación.
 */
nodo_abb_t *eliminar_casos_simples(nodo_abb_t *nodo, bool *se_elimino)
{
    if ((!nodo->izquierda) || (!nodo->derecha)) {
        nodo_abb_t *reemplazo = NULL;
        if (nodo->izquierda) {
            reemplazo = nodo->izquierda;
        } else {
            reemplazo = nodo->derecha;
        }
        free(nodo);
        *se_elimino = true;
        return reemplazo;
    }
    return nodo;
}

/**
 * Recorre recursivamente el subárbol buscando un elemento igual al provisto 
 * utilizando la función de comparación. Si lo encuentra, lo elimina aplicando 
 * las reglas de un árbol binario de búsqueda y guarda el elemento extraído 
 * en el puntero pasado por parámetro.
 *
 * Devuelve la nueva raíz del subárbol tras la eliminación o NULL si el nodo 
 * original era NULL.
 */
nodo_abb_t* eliminar_nodo(nodo_abb_t* nodo_actual, abb_comparador comparador, void** elemento_extraido,void* elemento,bool* se_elimino) {
    if (!nodo_actual) {
        return NULL;
    }
    int comparacion = comparador(elemento, nodo_actual->elemento);
    if (comparacion < SON_IGUALES) {
        nodo_actual->izquierda = eliminar_nodo(nodo_actual->izquierda, comparador, elemento_extraido,elemento, se_elimino);
    }
    else if (comparacion > SON_IGUALES){
        nodo_actual->derecha = eliminar_nodo (nodo_actual->derecha, comparador, elemento_extraido,elemento, se_elimino);
    }
    else{
        *elemento_extraido = nodo_actual->elemento;
        *se_elimino = true;
        bool caso_simple = false;
        nodo_abb_t *nodo_reemplazo = eliminar_casos_simples(nodo_actual, &caso_simple);
        if (caso_simple) {
            return nodo_reemplazo; 
        }
        void *reemplazo = buscar_elemento_predecesor(nodo_actual);
        nodo_actual->elemento = reemplazo;
        void *aux_extraido = NULL;
        nodo_actual->izquierda = eliminar_nodo(nodo_actual->izquierda, comparador, &aux_extraido, reemplazo, se_elimino);
    }
    return nodo_actual;
}

/**
 * Nodo_a_insertar es un puntero válido al igual que el comparador.
 *
 * Devuelve la raíz del subárbol con el nuevo nodo insertado.
 */
nodo_abb_t* insertar_nodo(abb_comparador comparador, nodo_abb_t* nodo_a_insertar, nodo_abb_t* nodo_actual) {
    if (!nodo_actual) {
        return nodo_a_insertar;
    }
    int comparacion = comparador(nodo_a_insertar->elemento, nodo_actual->elemento);
    if (comparacion <= SON_IGUALES) {
        nodo_actual->izquierda = insertar_nodo(comparador, nodo_a_insertar, nodo_actual->izquierda);
    } 
    else {
        nodo_actual->derecha = insertar_nodo(comparador, nodo_a_insertar, nodo_actual->derecha);
    }
    return nodo_actual;
}

/**
 * Comparador es válido. Busca un elemento dentro del árbol.
 *
 * Devuelve un puntero al elemento encontrado si existe en el árbol, o NULL en el caso de que no este o el nodo_actual sea NULL.
 */
void* buscar_elemento(abb_comparador comparador,nodo_abb_t* nodo_actual,void* elemento_buscado){
  if (!nodo_actual){
    return NULL;
  }
  int comparacion = comparador(elemento_buscado, nodo_actual->elemento);
  if (comparacion == SON_IGUALES){
    return nodo_actual->elemento;
  }
  else if (comparacion < SON_IGUALES){
    return (buscar_elemento(comparador,nodo_actual->izquierda,elemento_buscado));
  }
  else{
    return (buscar_elemento(comparador,nodo_actual->derecha,elemento_buscado));
  }
}

/**
 * 
 * Libera de forma recursiva la memoria de los nodos del árbol y sus elementos (si corresponde).
 */
void destruir_nodos(nodo_abb_t *nodo_actual, void (*destructor)(void *)) {
    if (!nodo_actual) {
        return;
    }
    destruir_nodos(nodo_actual->izquierda, destructor);
    destruir_nodos(nodo_actual->derecha, destructor);
    if (destructor) {
        destructor(nodo_actual->elemento);
    }
    free(nodo_actual);
}

abb_t* abb_crear(abb_comparador comparador){
  if (!comparador){
    return NULL;
  }
  abb_t* arbol = calloc (1,sizeof(abb_t));
  if (!arbol){
    return NULL;
  }
  arbol->comparador = comparador;
  return arbol;
}

abb_t* abb_insertar(abb_t* arbol, void* elemento){
  if (!arbol){
    return NULL;
  }
  nodo_abb_t* nodo_a_insertar = crear_nodo(elemento);
  if (!nodo_a_insertar){
    return NULL;
  }
  arbol->nodo_raiz = insertar_nodo(arbol->comparador,nodo_a_insertar,arbol->nodo_raiz);
  arbol->tamanio++;
  return arbol;
}

void* abb_quitar(abb_t* arbol, void *elemento) {
    if (!arbol) {
        return NULL;
    }
    void* elemento_extraido = NULL;
    bool se_elimino = false;
    arbol->nodo_raiz = eliminar_nodo(arbol->nodo_raiz, arbol->comparador, &elemento_extraido,elemento,&se_elimino);
    if (se_elimino) {
        arbol->tamanio--;
        return elemento_extraido;
    }
    return NULL;
}

void* abb_buscar(abb_t* arbol, void* elemento){
  if (!arbol){
    return NULL;
  }
  return (buscar_elemento(arbol->comparador,arbol->nodo_raiz,elemento));
}

bool abb_vacio(abb_t* arbol){
  return ((!arbol) || (arbol->tamanio == 0));
}

size_t abb_tamanio(abb_t *arbol){
  if (!arbol){
    return 0;
  }
  return (arbol->tamanio);
}

void abb_destruir(abb_t *arbol){
  abb_destruir_todo(arbol, NULL);
}

void abb_destruir_todo(abb_t *arbol, void (*destructor)(void *)){
  if (!arbol){
    return;
  }
  destruir_nodos(arbol->nodo_raiz,destructor);
  free(arbol);
}

size_t abb_con_cada_elemento(abb_t *arbol, abb_recorrido recorrido, bool (*funcion)(void *, void *), void *aux){
  size_t invocaciones = 0;
    if ((arbol) && (funcion)) {
        if (recorrido == INORDEN) {
            iterar_inorden(arbol->nodo_raiz, funcion, aux, &invocaciones);
        } else if (recorrido == PREORDEN) {
            iterar_preorden(arbol->nodo_raiz, funcion, aux, &invocaciones);
        } else if (recorrido == POSTORDEN) {
            iterar_postorden(arbol->nodo_raiz, funcion, aux, &invocaciones);
        }
    }
    return invocaciones;
}

size_t abb_recorrer(abb_t* arbol, abb_recorrido recorrido, void** array, size_t tamanio_array){
  size_t guardados = 0;
    if ((arbol) && (array) && (tamanio_array > 0)) {
        datos_array_t datos;
        datos.elementos = array;
        datos.capacidad_maxima = tamanio_array;
        datos.cantidad_guardada = 0;
        abb_con_cada_elemento(arbol, recorrido, guardar_en_array, &datos);
        guardados = datos.cantidad_guardada;
    }
    return guardados;
}