#include "tp1.h"
#include "abb/abb.h"
#include "leer_linea/leer_linea.h"
#include "validadores/validadores.h"
#include  <ctype.h>
#include  <string.h>

#define ERROR -1
#define EXITO 0
#define LECTURA "r"
#define ESCRITURA "w"
#define FORMATO_ESCRITURA "%s,%d,%.1f,%c\n"
#define CARACTER_NULO '\0'
#define SEPARADOR ','
#define CHAR_COMUN 'C'
#define CHAR_RARO 'R'
#define CHAR_LEGENDARIO 'L'

struct tp1{
    abb_t* pokemones;
};

struct buscador_orden {
    size_t n_buscado;
    size_t actual;
    struct pokemon *encontrado;
};

/**
 * Se asume que s1 y s2 son punteros válidos (no nulos) que apuntan a cadenas
 * terminadas en '\0'.
 * 
 * Compara dos cadenas carácter a carácter ignorando mayúsculas y minúsculas (case-insensitive).
 * 
 * Devuelve un entero < 0 si s1 es menor que s2, 0 si son iguales,
 * o un entero > 0 si s1 es mayor que s2.
 * 
 */
int comparar_nombres(char *s1, char *s2)
{
    unsigned char c1 = 0;
    unsigned char c2 = 0;
    while ((*s1 != CARACTER_NULO) && (*s2 != CARACTER_NULO)) {
        c1 = (unsigned char)tolower((unsigned char)*s1);
        c2 = (unsigned char)tolower((unsigned char)*s2);
        if (c1 != c2) {
            return ((int)c1 - (int)c2);
        }
        s1++;
        s2++;
    }
    c1 = (unsigned char)tolower((unsigned char)*s1);
    c2 = (unsigned char)tolower((unsigned char)*s2);
    return ((int)c1 - (int)c2);
}

/**
 * Se asume que p1 y p2 son punteros válidos (no nulos) a estructuras pokemon
 * 
 * Compara dos pokemones alfabéticamente a partir de sus nombres sin distinguir mayúsculas.
 * 
 * Devuelve un entero < 0 si p1 es menor que p2, 0 si son iguales,
 * o un entero > 0 si p1 es mayor que p2.
 */
int comparar_pokemones(void *p1, void *p2)
{
    struct pokemon* pokemon1 = (struct pokemon *)p1;
    struct pokemon* pokemon2 = (struct pokemon *)p2;
    return (comparar_nombres(pokemon1->nombre, pokemon2->nombre));
}

/**
 * Se asume que elemento y aux son punteros válidos (no nulos)
 *
 * Escribe un registro de pokémon en el archivo según el formato especificado.
 *
 * Siempre devuelve true para que el iterador del ABB continúe con el siguiente elemento.     
 */
bool escribir_pokemon_en_archivo(void *elemento, void *aux)
{
    struct pokemon *pokemon = (struct pokemon *)elemento;
    FILE *archivo = (FILE *)aux;
    char letra_rareza = CHAR_COMUN;
    if (pokemon->rareza == RAREZA_RARO) {
        letra_rareza = CHAR_RARO;
    } else if (pokemon->rareza == RAREZA_LEGENDARIO) {
        letra_rareza = CHAR_LEGENDARIO;
    }
    fprintf(archivo, FORMATO_ESCRITURA, pokemon->nombre, pokemon->velocidad, pokemon->peso, letra_rareza);
    return true;
}

/**
 * Se asume que tp1 y nombre son punteros válidos (no nulos) y que nombre
 * es una cadena terminada en '\0'.
 * 
 * Inserta el pokemon en la estructura manteniendo el orden alfabético. 
 * 
 * Devuelve EXITO (0) si se pudo crear e insertar correctamente, o ERROR (-1)
 * si falló la reserva de memoria dinámica o la inserción en el árbol.
 */
int agregar_pokemon(tp1_t *tp1, char *nombre, int velocidad, float peso,
		    enum rareza_pokemon rareza)
{
	char *nombre_copia = malloc(strlen(nombre) + 1);
    if (!nombre_copia){
        return ERROR;
    }
	strcpy(nombre_copia, nombre); 
	struct pokemon* pokemon = malloc (sizeof(struct pokemon));
    if (!pokemon){
        free (nombre_copia);
        return ERROR;
    }
    pokemon->nombre = nombre_copia;
    pokemon->peso = peso;
    pokemon->rareza = rareza;
    pokemon->velocidad = velocidad;
    if (!abb_insertar(tp1->pokemones,pokemon)){
        free(nombre_copia);
        free(pokemon);
	    return ERROR;
    }
	return EXITO;
}

bool buscar_enesimo(void *elemento, void *extra)
{
    struct buscador_orden *b = (struct buscador_orden *)extra;

    if (b->actual == b->n_buscado) {
        b->encontrado = (struct pokemon *)elemento;
        return false;
    }
    b->actual++;
    return true;
}

/**
 * Se asume que linea, velocidad, peso y rareza son punteros válidos
 * (previamente verificados o creados en el contexto de procesar_linea).
 * 
 * Verifica que linea contenga exactamente tres separadores y la divide 
 * colocando caracteres nulos.
 *
 * Devuelve true si la estructura de campos es correcta, o false en caso contrario.
 */
bool separar_campos(char *linea, char **velocidad, char **peso, char **rareza)
{
    char *coma1 = strchr(linea, SEPARADOR);
    if (!coma1) {
        return false;
    }
    char *coma2 = strchr(coma1 + 1, SEPARADOR);
    if (!coma2) {
        return false;
    }
    char *coma3 = strchr(coma2 + 1, SEPARADOR);
    if (!coma3) {
        return false;
    }
    if (strchr(coma3 + 1, SEPARADOR) != NULL) {
        return false;
    }
    *coma1 = CARACTER_NULO;
    *coma2 = CARACTER_NULO;
    *coma3 = CARACTER_NULO;
    *velocidad = coma1 + 1;
    *peso = coma2 + 1;
    *rareza = coma3 + 1;
    return true;
}

/**
 * Se asume que los punteros linea y tp1 son válidos (previamente verificados 
 * por tp1_leer_archivo).
 * 
 * Modifica linea aislando sus campos. Si el formato o los valores
 * son inválidos, se descarta la línea retornando EXITO.
 * Si ocurre un error de memoria al agregar el pokémon, retorna ERROR.
 */
int procesar_linea(char *linea, tp1_t *tp1)
{
    char *velocidad_str = NULL;
    char *peso_str = NULL;
    char *rareza_str = NULL;
    if (!separar_campos(linea, &velocidad_str, &peso_str, &rareza_str)) {
        return EXITO;
    }
    char *nombre = linea;
    int vel = 0;
    float peso_total = 0;
    enum rareza_pokemon rareza = 0;
    if (!nombre_valido(nombre) || tp1_buscar_pokemon(tp1, nombre) != NULL ||
        !velocidad_valida(velocidad_str, &vel) ||
        !peso_valido(peso_str, &peso_total) ||
        !rareza_valida(rareza_str, &rareza)) {
        return EXITO;
    }
    if (agregar_pokemon(tp1, nombre, vel, peso_total, rareza) != EXITO) {
        return ERROR;
    }
    return EXITO;
}

/**
 * Libera la memoria asignada a un struct pokemon y a su cadena interna nombre.
 * 
 * Si elemento es NULL, no realiza ninguna acción.
 */
void destruir_pokemon(void *elemento)
{
    if (!elemento) {
        return;
    }
    struct pokemon *pokemon = (struct pokemon *)elemento;
    free(pokemon->nombre);
    free(pokemon);
}

/**
 * Se asume que elemento y aux son punteros válidos (no nulos).
 * 
 * Si el pokémon ya existe
 * en el destino, no lo duplica y continúa la iteración retornando true.
 * 
 * Devuelve true si se pudo procesar/insertar con éxito, o false si ocurrió
 * un error al intentar agregar el pokémon.
 */
bool copiar_pokemon(void *elemento, void *aux)
{
    struct pokemon *pokemon = (struct pokemon *)elemento;
    tp1_t *tp1_destino = (tp1_t *)aux;
    if (tp1_buscar_pokemon(tp1_destino, pokemon->nombre) != NULL) {
        return true;
    }
    int resultado = agregar_pokemon(tp1_destino, pokemon->nombre, pokemon->velocidad, pokemon->peso, pokemon->rareza);
    return (resultado == EXITO);
}

/**
 * Se asume que destino y origen son punteros válidos (no nulos).
 *
 * Recorre todos los pokemones del árbol origen en PREORDEN y los copia
 * en el TDA destino utilizando copiar_pokemon.
 *
 * Devuelve true si se recorrieron e insertaron todos los elementos, o false si falló la copia de alguno.
 */
static bool copiar_todos_los_pokemones(tp1_t *destino, tp1_t *origen)
{
    size_t copiados = abb_con_cada_elemento(origen->pokemones, PREORDEN,copiar_pokemon, destino);
    return (copiados == abb_tamanio(origen->pokemones));
}

/**
 * Reserva memoria para la estructura tp1 e inicializa su ABB interno
 * utilizando la función de comparación por nombres.
 *
 * Devuelve el puntero al nuevo tp1_t creado, o NULL si falla la memoria.
 */
tp1_t *tp1_crear()
{
    tp1_t *tp1 = calloc(1, sizeof(tp1_t));
    if (!tp1) {
        return NULL;
    }
    tp1->pokemones = abb_crear(comparar_pokemones);
    if (!tp1->pokemones) {
        free(tp1);
        return NULL;
    }
    return tp1;
}

tp1_t *tp1_leer_archivo(const char *nombre){
    if (!nombre){
        return NULL;
    }
    FILE* archivo_pokemones = fopen(nombre, LECTURA);
    if (!archivo_pokemones){
        return NULL;
    }
    tp1_t *tp1 = tp1_crear();
    if (!tp1) {
        fclose(archivo_pokemones);
        return NULL;
    }
    char *linea = leer_linea(archivo_pokemones);
	while (linea != NULL) {
        if (procesar_linea(linea,tp1) == ERROR){
            free(linea);
            fclose(archivo_pokemones);
            tp1_destruir(tp1);
            return NULL;
        }
		free(linea); 
		linea = leer_linea(archivo_pokemones);
	}
	fclose(archivo_pokemones);
	return tp1;
}

size_t tp1_cantidad(tp1_t *tp1){
    if (!tp1){
        return 0;
    }
    return (abb_tamanio(tp1->pokemones));
}

tp1_t *tp1_combinar(tp1_t *tp1_a, tp1_t *tp1_b){
    if ((!tp1_a) || (!tp1_b)){
        return NULL;
    }
    tp1_t *tp1_c = tp1_crear();
    if (!tp1_c) {
        return NULL;
    }
    if ((!copiar_todos_los_pokemones(tp1_c, tp1_a)) || (!copiar_todos_los_pokemones(tp1_c, tp1_b))) {
        tp1_destruir(tp1_c);
        return NULL;
    }
    return tp1_c;
}

tp1_t *tp1_escribir_archivo(tp1_t *tp1, const char *nombre){
    if ((!tp1) || (!nombre)) {
		return NULL;
	}
	FILE *archivo = fopen(nombre, ESCRITURA);
	if (!archivo) {
		return NULL;
	}
    size_t pokemones_escritos = abb_con_cada_elemento(tp1->pokemones,INORDEN,escribir_pokemon_en_archivo,archivo);
    fclose(archivo);
    if (pokemones_escritos != abb_tamanio(tp1->pokemones)){
        return NULL;
    }
    return tp1;
}

struct pokemon *tp1_buscar_pokemon(tp1_t *tp1, const char *nombre){
    if ((!tp1) || (!nombre)){
        return NULL;
    } 
    struct pokemon pokemon_aux = {0};
    pokemon_aux.nombre = (char *)nombre;
    return (abb_buscar(tp1->pokemones, &pokemon_aux));
}

struct pokemon *tp1_buscar_orden(tp1_t *tp1, size_t n){ 
    if (!tp1 || n >= tp1_cantidad(tp1)) {
        return NULL;
    }
    struct buscador_orden b = {
        .n_buscado = n,
        .actual = 0,
        .encontrado = NULL
    };

    abb_con_cada_elemento(tp1->pokemones, INORDEN, buscar_enesimo, &b);

    return b.encontrado;
}

size_t tp1_iterar(tp1_t *tp1, bool (*f)(struct pokemon *, void *), void *extra){
    if ((!tp1) || (!tp1->pokemones) || (!f)) {
        return 0;
    }
    return (abb_con_cada_elemento(tp1->pokemones, INORDEN, (bool (*)(void *, void *))f, extra));
}

void *tp1_destruir(tp1_t *tp1)
{
	if (!tp1) {
		return NULL;
    }
    abb_destruir_todo(tp1->pokemones,destruir_pokemon);
    free(tp1);
    return NULL;
}

