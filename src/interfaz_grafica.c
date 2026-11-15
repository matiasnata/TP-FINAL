#include "interfaz_grafica.h"
#include "pokedex_util.h"
#include "../extra/engine.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EXITO 0
#define FRAME_CONTINUAR 0
#define FRAME_FINALIZAR 1
#define CARACTER_NULO '\0'
#define ESPACIO_NULO 1
#define VALOR_INICIAL_ENTRADA 16
#define SEMILLA 'S'
#define TIPO_NOMBRE 'M'
#define BASE_DECIMAL 10

#define DIFICULTAD_FACIL '1'
#define DIFICULTAD_DIFICIL '3'
#define DIFICULTAD_MEDIA '2'
#define POKEMON_POR_NIVEL 4
#define CANT_PREGUNTAS 3
#define PREGUNTAS_POSIBLES 6
#define CANT_OPCIONES 3
#define PREGUNTA_COMUNES 1
#define PREGUNTA_RAROS 2
#define PREGUNTA_LEGENDARIOS 3
#define PREGUNTA_HACIA_IZQUIERDA 4
#define PREGUNTA_HACIA_DERECHA 5

#define BLANCO 0xFFFFFF
#define GRIS 0xA0A0A0
#define AMARILLO 0xFFE060 // Legendario
#define AZUL 0x60A0FF // Raro
#define VERDE 0x80FF80 // Comun

#define POSICION_X 8
#define POSICION_Y_PRIMERA_LINEA 16
#define SEPARACION_LINEAS 12
#define LINEA_TITULO_MENU 1
#define LINEA_PRIMERA_OPCION_MENU 3
#define LINEA_ENTRADA 3
#define LINEA_INDICACION 15
#define POSICION_Y_TITULO_JUEGO 4
#define POSICION_Y_SEMILLA 212
#define TILE_FONDO_JUEGO 1
#define CANTIDAD_DIRECCIONES 2

#define DIRECCION_DERECHA 1
#define DIRECCION_IZQUIERDA -1
#define PASO_MINIMO 1
#define DIVISOR_VELOCIDAD 40
#define MARGEN_SUPERIOR_Y 16
#define RANGO_ALEATORIO_Y 150
#define SEPARACION_INICIAL_X 30
#define PERIODO_ZIGZAG 40u
#define MITAD_ZIGZAG 20u
#define DIVISOR_ZIGZAG 2u
#define DESFASE_ZIGZAG 7u
#define SPRITE_BASE 16u
#define FORMAS_SPRITE 4
#define CUADROS_POR_FORMA 2u
#define DIVISOR_ANIMACION 10u
#define DESPLAZAMIENTO_ROTULO_X 4

enum modo {
	MODO_MENU,
	MODO_ENTRADA,
	MODO_INFORMACION,
	MODO_MOSTRAR_POKEMON,
	MODO_INTRODUCCION,
	MODO_DIFICULTAD,
	MODO_SIN_POKEMON,
	MODO_JUEGO,
	MODO_PREGUNTA,
	MODO_RESULTADOS
};

struct pokemon_en_pantalla {
	struct pokemon *pokemon;
	int posicion_x;
	int posicion_base_y;
	int paso;
	int direccion;
	unsigned forma;
	unsigned orden;
};

struct estadisticas {
	int comunes;
	int raros;
	int legendarios;
	int hacia_izquierda;
	int hacia_derecha;
};

struct juego {
	tp1_t *pokedex;
	menu_t *menu;
	enum modo modo;
	struct pokemon *pokemon_encontrado;
	struct resumen_pokedex resumen;
	char tipo_entrada;
	char *entrada;
	size_t largo_entrada;
	size_t capacidad_entrada;
	unsigned semilla;
	lista_t *pokemones_en_pantalla;
	unsigned tick;
	struct estadisticas estadisticas;
	size_t pregunta_inicial;
	size_t salto_entre_preguntas;
	size_t pregunta;
	size_t posicion_correcta;
	size_t aciertos;
};

struct pregunta {
	char *texto;
	char *etiqueta;
	int respuesta;
};

struct seleccion {
	struct juego *juego;
	size_t faltan_elegir;
	size_t faltan_recorrer;
	bool correcta;
};

struct contexto_dibujo {
	pantalla_t *pantalla;
	unsigned tick;
	bool quedan_por_cruzar;
};

/**
 * Se asume que pantalla y texto son punteros válidos (no nulos).
 *
 * Dibuja una cadena de texto en la fila indicada con el color especificado.
 *
 * Devuelve true si el texto se dibujó correctamente, o false en caso contrario.
 */
bool dibujar_linea(pantalla_t *pantalla, size_t linea, unsigned color,
		   const char *texto)
{
	int posicion_y =
		POSICION_Y_PRIMERA_LINEA + (int)linea * SEPARACION_LINEAS;
	return (engine_texto(pantalla, POSICION_X, posicion_y, color, texto));
}

/**
 * Se asume que pantalla y formato son punteros válidos (no nulos).
 *
 * Formatea una cadena con un argumento entero sin signo (size_t) y la dibuja
 * en la línea indicada.
 *
 * Devuelve true si el texto se dibujó correctamente, o false ante error de memoria o formato.
 */
bool dibujar_texto_con_numero(pantalla_t *pantalla, size_t linea,
			      unsigned color, const char *formato,
			      size_t numero)
{
	int largo = snprintf(NULL, 0, formato, numero);
	if (largo < 0) {
		return false;
	}
	size_t tamanio = (size_t)largo + ESPACIO_NULO;
	char *texto = malloc(tamanio);
	if (!texto) {
		return false;
	}
	snprintf(texto, tamanio, formato, numero);
	bool dibujada = dibujar_linea(pantalla, linea, color, texto);
	free(texto);
	return dibujada;
}

/**
 * Se asume que pantalla y formato son punteros válidos (no nulos).
 *
 * Formatea una cadena con un número decimal (double) y la dibuja en la línea indicada.
 *
 * Devuelve true si el texto se dibujó correctamente, o false ante error de memoria o formato.
 */
bool dibujar_texto_con_decimal(pantalla_t *pantalla, size_t linea,
			       unsigned color, const char *formato,
			       double numero)
{
	int largo = snprintf(NULL, 0, formato, numero);
	if (largo < 0) {
		return false;
	}
	size_t tamanio = (size_t)largo + ESPACIO_NULO;
	char *texto = malloc(tamanio);
	if (!texto) {
		return false;
	}
	snprintf(texto, tamanio, formato, numero);
	bool dibujada = dibujar_linea(pantalla, linea, color, texto);
	free(texto);
	return dibujada;
}

/**
 * Se asume que pantalla, formato y cadena son punteros válidos (no nulos).
 *
 * Formatea un texto insertando una cadena de caracteres y lo dibuja en la línea indicada.
 *
 * Devuelve true si el texto se dibujó correctamente, o false ante error de memoria o formato.
 */
bool dibujar_texto_con_cadena(pantalla_t *pantalla, size_t linea,
			      unsigned color, const char *formato,
			      const char *cadena)
{
	int largo = snprintf(NULL, 0, formato, cadena);
	if (largo < 0) {
		return false;
	}
	size_t tamanio = (size_t)largo + ESPACIO_NULO;
	char *texto = malloc(tamanio);
	if (!texto) {
		return false;
	}
	snprintf(texto, tamanio, formato, cadena);
	bool dibujada = dibujar_linea(pantalla, linea, color, texto);
	free(texto);
	return dibujada;
}

/**
 * Se asume que pantalla, formato y cadena son punteros válidos (no nulos).
 *
 * Formatea un texto con una cadena y un entero (int) y lo dibuja en la línea indicada.
 *
 * Devuelve true si el texto se dibujó correctamente, o false ante error de memoria o formato.
 */
bool dibujar_cadena_con_numero(pantalla_t *pantalla, size_t linea,
			       unsigned color, const char *formato,
			       const char *cadena, int numero)
{
	int largo = snprintf(NULL, 0, formato, cadena, numero);
	if (largo < 0) {
		return false;
	}
	size_t tamanio = (size_t)largo + ESPACIO_NULO;
	char *texto = malloc(tamanio);
	if (!texto) {
		return false;
	}
	snprintf(texto, tamanio, formato, cadena, numero);
	bool dibujada = dibujar_linea(pantalla, linea, color, texto);
	free(texto);
	return dibujada;
}

/**
 * Se asume que pantalla, formato y cadena son punteros válidos (no nulos).
 *
 * Formatea un texto con una cadena y un decimal (double) y lo dibuja en la línea indicada.
 *
 * Devuelve true si el texto se dibujó correctamente, o false ante error de memoria o formato.
 */
bool dibujar_cadena_con_decimal(pantalla_t *pantalla, size_t linea,
				unsigned color, const char *formato,
				const char *cadena, double numero)
{
	int largo = snprintf(NULL, 0, formato, cadena, numero);
	if (largo < 0) {
		return false;
	}
	size_t tamanio = (size_t)largo + ESPACIO_NULO;
	char *texto = malloc(tamanio);
	if (!texto) {
		return false;
	}
	snprintf(texto, tamanio, formato, cadena, numero);
	bool dibujada = dibujar_linea(pantalla, linea, color, texto);
	free(texto);
	return dibujada;
}

/**
 * Se asume que pantalla y formato son punteros válidos (no nulos).
 *
 * Formatea un texto con un entero sin signo (size_t) y un entero (int) y lo
 * dibuja en la línea indicada.
 *
 * Devuelve true si el texto se dibujó correctamente, o false ante error de memoria o formato.
 */
bool dibujar_dos_numeros(pantalla_t *pantalla, size_t linea, unsigned color,
			 const char *formato, size_t primero, int segundo)
{
	int largo = snprintf(NULL, 0, formato, primero, segundo);
	if (largo < 0) {
		return false;
	}
	size_t tamanio = (size_t)largo + ESPACIO_NULO;
	char *texto = malloc(tamanio);
	if (!texto) {
		return false;
	}
	snprintf(texto, tamanio, formato, primero, segundo);
	bool dibujada = dibujar_linea(pantalla, linea, color, texto);
	free(texto);
	return dibujada;
}

/**
 * Se asume que pantalla, formato, primera y segunda son punteros válidos (no nulos).
 *
 * Formatea un texto combinando dos cadenas de caracteres y lo dibuja en la línea indicada.
 *
 * Devuelve true si el texto se dibujó correctamente, o false ante error de memoria o formato.
 */
bool dibujar_dos_cadenas(pantalla_t *pantalla, size_t linea, unsigned color,
			 const char *formato, const char *primera,
			 const char *segunda)
{
	int largo = snprintf(NULL, 0, formato, primera, segunda);
	if (largo < 0) {
		return false;
	}
	size_t tamanio = (size_t)largo + ESPACIO_NULO;
	char *texto = malloc(tamanio);
	if (!texto) {
		return false;
	}
	snprintf(texto, tamanio, formato, primera, segunda);
	bool dibujada = dibujar_linea(pantalla, linea, color, texto);
	free(texto);
	return dibujada;
}

/**
 * Devuelve el nombre descriptivo en texto correspondiente a la rareza indicada.
 */
char *nombre_rareza(enum rareza_pokemon rareza)
{
	if (rareza == RAREZA_RARO) {
		return "Raro";
	}
	if (rareza == RAREZA_LEGENDARIO) {
		return "Legendario";
	}
	return "Comun";
}

/**
 * Devuelve la etiqueta abreviada ("[C]", "[R]" o "[L]") asociada a la rareza indicada.
 */
char *rotulo_rareza(enum rareza_pokemon rareza)
{
	if (rareza == RAREZA_RARO) {
		return "[R]";
	}
	if (rareza == RAREZA_LEGENDARIO) {
		return "[L]";
	}
	return "[C]";
}

/**
 * Devuelve el color correspondiente a la rareza del pokémon.
 */
unsigned color_rareza(enum rareza_pokemon rareza)
{
	if (rareza == RAREZA_RARO) {
		return AZUL;
	}
	if (rareza == RAREZA_LEGENDARIO) {
		return AMARILLO;
	}
	return VERDE;
}

/**
 * Se asume que juego es un puntero válido (no nulos).
 *
 * Vacía el buffer de entrada de texto del usuario.
 */
void reiniciar_entrada(struct juego *juego)
{
	juego->entrada[0] = CARACTER_NULO;
	juego->largo_entrada = 0;
}

/**
 * Se asume que juego es un puntero válido (no nulo).
 *
 * Agrega un carácter al final del buffer de entrada del usuario, redimensionando
 * la memoria dinámica si es necesario.
 */
void agregar_caracter(struct juego *juego, char caracter)
{
	if (juego->largo_entrada + ESPACIO_NULO >= juego->capacidad_entrada) {
		size_t capacidad =
			juego->capacidad_entrada + VALOR_INICIAL_ENTRADA;
		char *agrandada = realloc(juego->entrada, capacidad);
		if (agrandada != NULL) {
			juego->entrada = agrandada;
			juego->capacidad_entrada = capacidad;
		}
	}
	if (juego->largo_entrada + ESPACIO_NULO < juego->capacidad_entrada) {
		juego->entrada[juego->largo_entrada] = caracter;
		juego->largo_entrada++;
		juego->entrada[juego->largo_entrada] = CARACTER_NULO;
	}
}

/**
 * Se asume que entrada, pantalla y juego son punteros válidos (no nulos).
 *
 * Renderiza y procesa la pantalla de ingreso de texto (ya sea para semilla o
 * para búsqueda de pokémon por nombre dependiendo de la letra que ingreso el usuario).
 */
void frame_entrada(const struct entrada *entrada, pantalla_t *pantalla,
		   struct juego *juego)
{
	bool es_semilla = (juego->tipo_entrada == SEMILLA);
	const char *etiqueta_solicitud = "Ingrese el nombre del pokemon:";
	if (es_semilla) {
		etiqueta_solicitud = "Ingresa la semilla:";
	}
	dibujar_linea(pantalla, 0, BLANCO, etiqueta_solicitud);
	dibujar_linea(pantalla, LINEA_ENTRADA, AMARILLO, juego->entrada);

	if (engine_presionado(entrada, BOTON_ENTER) && es_semilla) {
		juego->semilla =
			(unsigned)strtoul(juego->entrada, NULL, BASE_DECIMAL);
		juego->modo = MODO_INTRODUCCION;
	} else if (engine_presionado(entrada, BOTON_ENTER)) {
		juego->pokemon_encontrado =
			tp1_buscar_pokemon(juego->pokedex, juego->entrada);
		juego->modo = MODO_MOSTRAR_POKEMON;
	} else if (entrada->caracter == CARACTER_BORRAR) {
		if (juego->largo_entrada > 0) {
			juego->largo_entrada--;
			juego->entrada[juego->largo_entrada] = CARACTER_NULO;
		}
	} else if (entrada->caracter != CARACTER_NULO) {
		agregar_caracter(juego, (char)entrada->caracter);
	}
}

/**
 * Se asume que pantalla y resumen son punteros válidos (no nulos).
 *
 * Dibuja en pantalla las estadísticas generales de la Pokédex calculadas en resumen.
 */
void dibujar_informacion(pantalla_t *pantalla,
			 const struct resumen_pokedex *resumen)
{
	dibujar_texto_con_numero(pantalla, 0, BLANCO, "Total: %zu",
				 resumen->total);
	dibujar_texto_con_numero(pantalla, 1, BLANCO, "Comunes: %zu",
				 resumen->comunes);
	dibujar_texto_con_numero(pantalla, 2, BLANCO, "Raros: %zu",
				 resumen->raros);
	dibujar_texto_con_numero(pantalla, 3, BLANCO, "Legendarios: %zu",
				 resumen->legendarios);
	if (resumen->total > 0) {
		dibujar_cadena_con_numero(pantalla, 4, BLANCO,
					  "Mas veloz: %s (%d)",
					  resumen->mas_veloz->nombre,
					  resumen->mas_veloz->velocidad);
		dibujar_cadena_con_numero(pantalla, 5, BLANCO,
					  "Menos veloz: %s (%d)",
					  resumen->menos_veloz->nombre,
					  resumen->menos_veloz->velocidad);
		dibujar_cadena_con_decimal(pantalla, 6, BLANCO,
					   "Mas pesado: %s (%.1f)",
					   resumen->mas_pesado->nombre,
					   (double)resumen->mas_pesado->peso);
		dibujar_cadena_con_decimal(pantalla, 7, BLANCO,
					   "Menos pesado: %s (%.1f)",
					   resumen->menos_pesado->nombre,
					   (double)resumen->menos_pesado->peso);
	}
}

/**
 * Se asume que pantalla es un puntero válido (no nulo).
 *
 * Muestra los atributos del pokémon encontrado, o un mensaje de error si no se encuentra el pokemon en la pokedex.
 */
void dibujar_pokemon_encontrado(pantalla_t *pantalla,
				const struct pokemon *pokemon)
{
	if (pokemon == NULL) {
		dibujar_linea(pantalla, 0, BLANCO, "Pokemon no encontrado");
	} else {
		dibujar_texto_con_cadena(pantalla, 0, BLANCO, "Nombre: %s",
					 pokemon->nombre);
		dibujar_texto_con_numero(pantalla, 1, BLANCO, "Velocidad: %zu",
					 (size_t)pokemon->velocidad);
		dibujar_texto_con_decimal(pantalla, 2, BLANCO, "Peso: %.1f",
					  (double)pokemon->peso);
		dibujar_texto_con_cadena(pantalla, 3, BLANCO, "Rareza: %s",
					 nombre_rareza(pokemon->rareza));
	}
}

/**
 * Construye y devuelve una estructura pregunta con su enunciado, etiqueta y respuesta correcta.
 */
struct pregunta armar_pregunta(char *texto, char *etiqueta, int respuesta)
{
	struct pregunta pregunta;
	pregunta.texto = texto;
	pregunta.etiqueta = etiqueta;
	pregunta.respuesta = respuesta;
	return pregunta;
}

/**
 * Se asume que juego es un puntero válido (no nulo).
 *
 * Obtiene la pregunta correspondiente al índice indicado (0 a 5) junto con su
 * respuesta.
 */
struct pregunta obtener_pregunta(const struct juego *juego, size_t numero)
{
	const struct estadisticas *estadisticas = &juego->estadisticas;
	int total = estadisticas->comunes + estadisticas->raros +
		    estadisticas->legendarios;
	struct pregunta pregunta =
		armar_pregunta("Cuantos pokemon viste pasar?", "Total", total);
	if (numero == PREGUNTA_COMUNES) {
		pregunta = armar_pregunta("Cuantos eran comunes?", "Comunes",
					  estadisticas->comunes);
	} else if (numero == PREGUNTA_RAROS) {
		pregunta = armar_pregunta("Cuantos eran raros?", "Raros",
					  estadisticas->raros);
	} else if (numero == PREGUNTA_LEGENDARIOS) {
		pregunta = armar_pregunta("Cuantos eran legendarios?",
					  "Legendarios",
					  estadisticas->legendarios);
	} else if (numero == PREGUNTA_HACIA_IZQUIERDA) {
		pregunta = armar_pregunta("Cuantos fueron de der. a izq.?",
					  "Der. a izq.",
					  estadisticas->hacia_izquierda);
	} else if (numero == PREGUNTA_HACIA_DERECHA) {
		pregunta = armar_pregunta("Cuantos fueron de izq. a der.?",
					  "Izq. a der.",
					  estadisticas->hacia_derecha);
	}
	return pregunta;
}

/**
 * Se asume que juego es un puntero válido (no nulo).
 *
 * Devuelve la k-ésima pregunta de la partida actual.
 */
struct pregunta pregunta_de_la_partida(const struct juego *juego, size_t k)
{
	return obtener_pregunta(juego, (juego->pregunta_inicial +
					k * juego->salto_entre_preguntas) %
					       PREGUNTAS_POSIBLES);
}

/**
 * Se asume que entrada, pantalla y juego son punteros válidos (no nulos).
 *
 * Dibuja la pregunta actual con sus opciones y evalúa la respuesta ingresada por el jugador.
 */
void frame_pregunta(const struct entrada *entrada, pantalla_t *pantalla,
		    struct juego *juego)
{
	struct pregunta pregunta =
		pregunta_de_la_partida(juego, juego->pregunta);
	dibujar_dos_numeros(pantalla, 0, AMARILLO, "Pregunta %zu de %d",
			    juego->pregunta + 1, CANT_PREGUNTAS);
	dibujar_linea(pantalla, 2, BLANCO, pregunta.texto);
	for (size_t opcion = 0; opcion < CANT_OPCIONES; opcion++) {
		size_t distancia =
			(opcion + CANT_OPCIONES - juego->posicion_correcta) %
			CANT_OPCIONES;
		dibujar_dos_numeros(pantalla, opcion + 4, BLANCO, "%zu) %d",
				    opcion + 1,
				    pregunta.respuesta + (int)distancia);
	}

	if ((entrada->caracter >= '1') &&
	    (entrada->caracter < '1' + CANT_OPCIONES)) {
		if ((size_t)(entrada->caracter - '1') ==
		    juego->posicion_correcta) {
			juego->aciertos++;
		}
		juego->pregunta++;
		juego->posicion_correcta = (size_t)rand() % CANT_OPCIONES;
		if (juego->pregunta == CANT_PREGUNTAS) {
			juego->modo = MODO_RESULTADOS;
		}
	}
}

/**
 * Se asume que pantalla y juego son punteros válidos (no nulos).
 *
 * Dibuja en pantalla los aciertos obtenidos y el resumen completo de la partida.
 */
void dibujar_resultados(pantalla_t *pantalla, const struct juego *juego)
{
	dibujar_dos_numeros(pantalla, 0, AMARILLO, "Aciertos: %zu de %d",
			    juego->aciertos, CANT_PREGUNTAS);
	dibujar_linea(pantalla, 1, BLANCO, "--- Estadisticas ---");
	for (size_t numero = 0; numero < PREGUNTAS_POSIBLES; numero++) {
		struct pregunta pregunta = obtener_pregunta(juego, numero);
		dibujar_cadena_con_numero(pantalla, numero + 2, BLANCO,
					  "%s: %d", pregunta.etiqueta,
					  pregunta.respuesta);
	}
}

/**
 * Se asume que estadisticas y pokemon son punteros válidos (no nulos).
 *
 * Incrementa los contadores del struct estadisticas, de rareza y dirección de desplazamiento según el pokémon recibido.
 */
void registrar_en_estadisticas(struct estadisticas *estadisticas,
			       const struct pokemon_en_pantalla *pokemon)
{
	if (pokemon->pokemon->rareza == RAREZA_COMUN) {
		estadisticas->comunes++;
	} else if (pokemon->pokemon->rareza == RAREZA_RARO) {
		estadisticas->raros++;
	} else {
		estadisticas->legendarios++;
	}
	if (pokemon->direccion == DIRECCION_IZQUIERDA) {
		estadisticas->hacia_izquierda++;
	} else {
		estadisticas->hacia_derecha++;
	}
}

/**
 * Se asume que pokemon es un puntero válido (no nulo).
 *
 * Reserva e inicializa una estructura pokemon_en_pantalla con dirección, velocidad,
 * posición inicial y sprite asignados.
 *
 * Devuelve un puntero al nuevo elemento creado, o NULL en caso de error..
 */
struct pokemon_en_pantalla *
crear_pokemon_en_pantalla(struct pokemon *pokemon, size_t posicion_en_pokedex,
			  size_t orden)
{
	struct pokemon_en_pantalla *nuevo =
		malloc(sizeof(struct pokemon_en_pantalla));
	if (nuevo != NULL) {
		int separacion = (int)orden * SEPARACION_INICIAL_X;
		nuevo->pokemon = pokemon;
		if (rand() % CANTIDAD_DIRECCIONES == EXITO) {
			nuevo->direccion = DIRECCION_DERECHA;
		} else {
			nuevo->direccion = DIRECCION_IZQUIERDA;
		}
		nuevo->paso =
			PASO_MINIMO + pokemon->velocidad / DIVISOR_VELOCIDAD;
		nuevo->posicion_base_y =
			MARGEN_SUPERIOR_Y + rand() % RANGO_ALEATORIO_Y;
		nuevo->forma = (unsigned)(posicion_en_pokedex % FORMAS_SPRITE);
		nuevo->orden = (unsigned)orden;
		if (nuevo->direccion == DIRECCION_DERECHA) {
			nuevo->posicion_x = -SPRITE_PX - separacion;
		} else {
			nuevo->posicion_x = PANTALLA_ANCHO_PX + separacion;
		}
	}
	return nuevo;
}

/**
 * Se asume que pokemon y extra son punteros válidos (no nulos).
 *
 * Selecciona aleatoriamente los pokemones de la
 * partida, los inserta en la lista de pantalla y actualiza las estadísticas.
 *
 * Devuelve true para continuar la selección, o false si ocurrió un error de memoria.
 */
bool elegir_pokemon(struct pokemon *pokemon, void *extra)
{
	struct seleccion *seleccion = extra;
	struct juego *juego = seleccion->juego;

	if ((size_t)rand() % seleccion->faltan_recorrer <
	    seleccion->faltan_elegir) {
		size_t posicion = tp1_cantidad(juego->pokedex) -
				  seleccion->faltan_recorrer;
		size_t orden = lista_cantidad(juego->pokemones_en_pantalla);
		struct pokemon_en_pantalla *nuevo =
			crear_pokemon_en_pantalla(pokemon, posicion, orden);
		seleccion->correcta =
			(nuevo != NULL) &&
			lista_insertar(juego->pokemones_en_pantalla, nuevo,
				       orden);
		if (seleccion->correcta) {
			registrar_en_estadisticas(&juego->estadisticas, nuevo);
			seleccion->faltan_elegir--;
		} else {
			free(nuevo);
		}
	}
	seleccion->faltan_recorrer--;
	return seleccion->correcta;
}

/**
 * Convierte el carácter de dificultad elegido ('1', '2' o '3') en un entero.
 */
int dificultad_elegida(char opcion)
{
	if (opcion == DIFICULTAD_FACIL) {
		return 1;
	}
	if (opcion == DIFICULTAD_MEDIA) {
		return 2;
	}
	return 3;
}

/**
 * Se asume que juego es un puntero válido (no nulo).
 *
 * Inicializa una nueva partida según la dificultad seleccionada, sorteando los
 * pokemones y las preguntas.
 * 
 * Si no hay pookemones suficientes cambia a MODO_SIN_POKEMON.
 */
void iniciar_partida(struct juego *juego, char opcion_dificultad)
{
	int dificultad = dificultad_elegida(opcion_dificultad);
	size_t cantidad_pokemones = tp1_cantidad(juego->pokedex);

	struct seleccion seleccion;
	seleccion.juego = juego;
	seleccion.faltan_elegir = (size_t)dificultad * POKEMON_POR_NIVEL;
	seleccion.faltan_recorrer = cantidad_pokemones;
	seleccion.correcta = true;

	if (seleccion.faltan_elegir > cantidad_pokemones) {
		juego->modo = MODO_SIN_POKEMON;
	} else {
		struct estadisticas estadisticas_vacias = { 0 };
		srand(juego->semilla);
		lista_destruir_todo(juego->pokemones_en_pantalla, free);
		juego->pokemones_en_pantalla = lista_crear();
		juego->estadisticas = estadisticas_vacias;
		juego->tick = 0;
		juego->pregunta = 0;
		juego->aciertos = 0;
		juego->pregunta_inicial = (size_t)rand() % PREGUNTAS_POSIBLES;
		juego->salto_entre_preguntas = 1 + (size_t)rand() % 2;
		juego->posicion_correcta = (size_t)rand() % CANT_OPCIONES;
		tp1_iterar(juego->pokedex, elegir_pokemon, &seleccion);
		if (seleccion.correcta) {
			juego->modo = MODO_JUEGO;
		} else {
			juego->modo = MODO_MENU;
		}
	}
}

/**
 * Se asume que pokemon es un puntero válido (no nulo).
 *
 * Devuelve true si el pokémon ya atravesó por completo la pantalla según su dirección.
 */
bool ya_cruzo(const struct pokemon_en_pantalla *pokemon)
{
	bool salio_derecha = ((pokemon->direccion == DIRECCION_DERECHA) &&
			      (pokemon->posicion_x >= PANTALLA_ANCHO_PX));
	bool salio_izquierda = ((pokemon->direccion == DIRECCION_IZQUIERDA) &&
				(pokemon->posicion_x <= -SPRITE_PX));
	return ((salio_derecha) || (salio_izquierda));
}

/**
 * Se asume que pantalla y pokemon son punteros válidos (no nulos).
 *
 * Dibuja el sprite animado del pokémon con movimiento en zigzag.
 */
void dibujar_pokemon(pantalla_t *pantalla, unsigned tick,
		     const struct pokemon_en_pantalla *pokemon)
{
	unsigned fase =
		(tick / DIVISOR_ZIGZAG + pokemon->orden * DESFASE_ZIGZAG) %
		PERIODO_ZIGZAG;
	int desplazamiento = (int)fase;
	if (fase > MITAD_ZIGZAG) {
		desplazamiento = (int)(PERIODO_ZIGZAG - fase);
	}
	int posicion_y = pokemon->posicion_base_y + desplazamiento;
	unsigned cuadro = (tick / DIVISOR_ANIMACION) % CUADROS_POR_FORMA;
	unsigned sprite =
		SPRITE_BASE + CUADROS_POR_FORMA * pokemon->forma + cuadro;
	int reflejo = REFLEJAR_NO;
	if (pokemon->direccion == DIRECCION_DERECHA) {
		reflejo = REFLEJAR_X;
	}
	engine_sprite(pantalla, (unsigned char)sprite, pokemon->posicion_x,
		      posicion_y, reflejo);
	engine_texto(pantalla, pokemon->posicion_x - DESPLAZAMIENTO_ROTULO_X,
		     posicion_y - FUENTE_PX,
		     color_rareza(pokemon->pokemon->rareza),
		     rotulo_rareza(pokemon->pokemon->rareza));
}

/**
 * Se asume que elemento y extra son punteros válidos (no nulos).
 *
 * Actualiza la posición horizontal del pokémon en cada cuadro y lo dibuja si está visible.
 *
 * Siempre devuelve true para continuar iterando sobre todos los pokemones en pantalla.
 */
bool actualizar_y_dibujar(void *elemento, void *extra)
{
	struct pokemon_en_pantalla *pokemon = elemento;
	struct contexto_dibujo *contexto = extra;
	bool visible = ((pokemon->posicion_x > -SPRITE_PX) &&
			(pokemon->posicion_x < PANTALLA_ANCHO_PX));

	pokemon->posicion_x += pokemon->direccion * pokemon->paso;
	contexto->quedan_por_cruzar =
		((contexto->quedan_por_cruzar) || (!ya_cruzo(pokemon)));
	if (visible) {
		dibujar_pokemon(contexto->pantalla, contexto->tick, pokemon);
	}
	return true;
}

/**
 * Se asume que pantalla y juego son punteros válidos (no nulos).
 *
 * Ejecuta un cuadro de animación de la partida y pasa a MODO_PREGUNTA cuando todos
 * los pokemones terminaron de cruzar la pantalla.
 */
void frame_juego(pantalla_t *pantalla, struct juego *juego)
{
	juego->tick++;
	struct contexto_dibujo contexto;
	contexto.pantalla = pantalla;
	contexto.tick = juego->tick;
	contexto.quedan_por_cruzar = false;

	engine_llenar(pantalla, TILE_FONDO_JUEGO);
	dibujar_texto_con_numero(
		pantalla,
		(POSICION_Y_SEMILLA - POSICION_Y_PRIMERA_LINEA) /
			SEPARACION_LINEAS,
		BLANCO, "Semilla: %zu", (size_t)juego->semilla);
	lista_iterar(juego->pokemones_en_pantalla, actualizar_y_dibujar,
		     &contexto);
	if (!contexto.quedan_por_cruzar) {
		juego->modo = MODO_PREGUNTA;
	}
}

/**
 * Se asume que pantalla es un puntero válido (no nulo).
 *
 * Dibuja la pantalla de instrucciones previas al inicio de una partida.
 */
void dibujar_introduccion(pantalla_t *pantalla)
{
	dibujar_linea(pantalla, 0, AMARILLO, "Como se juega");
	dibujar_linea(pantalla, 2, BLANCO, "Mira los pokemon que cruzan");
	dibujar_linea(pantalla, 3, BLANCO, "la pantalla y despues");
	dibujar_texto_con_numero(pantalla, 4, BLANCO, "responde %zu preguntas.",
				 CANT_PREGUNTAS);
	dibujar_linea(pantalla, 6, BLANCO, "La rareza se ve sobre cada");
	dibujar_linea(pantalla, 7, BLANCO, "pokemon con una letra:");
	dibujar_dos_cadenas(pantalla, 9, VERDE, "%s %s",
			    rotulo_rareza(RAREZA_COMUN),
			    nombre_rareza(RAREZA_COMUN));
	dibujar_dos_cadenas(pantalla, 10, AZUL, "%s %s",
			    rotulo_rareza(RAREZA_RARO),
			    nombre_rareza(RAREZA_RARO));
	dibujar_dos_cadenas(pantalla, 11, AMARILLO, "%s %s",
			    rotulo_rareza(RAREZA_LEGENDARIO),
			    nombre_rareza(RAREZA_LEGENDARIO));
}

/**
 * Se asume que entrada, pantalla y juego son punteros válidos (no nulos).
 *
 * Renderiza las pantallas informativas estáticas (estadísticas, resultado de búsqueda,
 * resultados finales, falta de pokemones o introducción) y espera ENTER para avanzar.
 */
void frame_informativa(const struct entrada *entrada, pantalla_t *pantalla,
		       struct juego *juego)
{
	char *indicacion = "ENTER: volver al menu";
	if (juego->modo == MODO_INFORMACION) {
		dibujar_informacion(pantalla, &juego->resumen);
	} else if (juego->modo == MODO_MOSTRAR_POKEMON) {
		dibujar_pokemon_encontrado(pantalla, juego->pokemon_encontrado);
	} else if (juego->modo == MODO_RESULTADOS) {
		dibujar_resultados(pantalla, juego);
	} else if (juego->modo == MODO_SIN_POKEMON) {
		dibujar_linea(
			pantalla, 0, AMARILLO,
			"No hay pokemon suficientes para esta dificultad.");
	} else {
		dibujar_introduccion(pantalla);
		indicacion = "ENTER: continuar";
	}
	dibujar_linea(pantalla, LINEA_INDICACION, GRIS, indicacion);

	if (engine_presionado(entrada, BOTON_ENTER)) {
		if (juego->modo == MODO_INTRODUCCION) {
			juego->modo = MODO_DIFICULTAD;
		} else {
			juego->modo = MODO_MENU;
		}
	}
}

/**
 * Se asume que entrada, pantalla y juego son punteros válidos (no nulos).
 *
 * Dibuja las dificultades del juego disponibles e inicia la partida al luego de que el usuario ingrese una respuesta.
 */
void frame_dificultad(const struct entrada *entrada, pantalla_t *pantalla,
		      struct juego *juego)
{
	dibujar_linea(pantalla, 0, AMARILLO, "Elegi la dificultad:");
	dibujar_linea(pantalla, 2, BLANCO, "1 - Dificultad facil");
	dibujar_linea(pantalla, 3, BLANCO, "2 - Dificultad media");
	dibujar_linea(pantalla, 4, BLANCO, "3 - Dificultad dificil");
	if ((entrada->caracter >= DIFICULTAD_FACIL) &&
	    (entrada->caracter <= DIFICULTAD_DIFICIL)) {
		iniciar_partida(juego, (char)entrada->caracter);
	}
}

/**
 * Se asume que contexto apunta a una estructura struct juego válida.
 *
 * Inicia el flujo de juego con una semilla generada a partir del reloj del sistema.
 *
 * Devuelve true para mantener activa la aplicación.
 */
static bool accion_jugar(void *contexto)
{
	struct juego *juego = contexto;
	juego->semilla = (unsigned)time(NULL);
	juego->modo = MODO_INTRODUCCION;
	return true;
}

/**
 * Se asume que juego es un puntero válido (no nulo).
 *
 * Prepara la interfaz para solicitar una entrada de texto al usuario.
 *
 * Devuelve true para mantener activa la aplicación.
 */
bool pedir_entrada(struct juego *juego, char tipo_entrada)
{
	reiniciar_entrada(juego);
	juego->tipo_entrada = tipo_entrada;
	juego->modo = MODO_ENTRADA;
	return true;
}

/**
 * Se asume que contexto apunta a una estructura struct juego válida.
 *
 * Cambia al modo de ingreso de semilla manual.
 *
 * Devuelve true para mantener activa la aplicación.
 */
static bool accion_semilla(void *contexto)
{
	return pedir_entrada(contexto, SEMILLA);
}

/**
 * Se asume que contexto apunta a una estructura struct juego válida.
 *
 * Cambia al modo de búsqueda de pokémon por nombre.
 *
 * Devuelve true para mantener activa la aplicación.
 */
bool accion_mostrar(void *contexto)
{
	return pedir_entrada(contexto, TIPO_NOMBRE);
}

/**
 * Se asume que contexto apunta a una estructura struct juego válida.
 *
 * Calcula el resumen estadístico de la Pokédex y cambia a MODO_INFORMACION.
 *
 * Devuelve true para mantener activa la aplicación.
 */
bool accion_info(void *contexto)
{
	struct juego *juego = contexto;
	juego->resumen = calcular_resumen_pokedex(juego->pokedex);
	juego->modo = MODO_INFORMACION;
	return true;
}

/**
 * Acción de salida del menú gráfico.
 *
 * Devuelve false para indicar al bucle principal que debe finalizar.
 */
static bool accion_salir(void *contexto)
{
	(void)contexto;
	return false;
}

/**
 * Se asume que descripcion y pantalla son punteros válidos (no nulos).
 *
 * Devuelve true si la opción se dibujó correctamente, o false en caso contrario.
 */
bool dibujar_opcion(char clave, const char *descripcion, size_t indice,
		    void *pantalla)
{
	int largo = snprintf(NULL, 0, "%c - %s", clave, descripcion);
	if (largo < 0) {
		return false;
	}
	size_t tamanio = (size_t)largo + ESPACIO_NULO;
	char *texto = malloc(tamanio);
	if (!texto) {
		return false;
	}
	snprintf(texto, tamanio, "%c - %s", clave, descripcion);
	bool dibujada = dibujar_linea(
		pantalla, indice + LINEA_PRIMERA_OPCION_MENU, BLANCO, texto);
	free(texto);
	return dibujada;
}

/**
 * Se asume que entrada, pantalla y juego son punteros válidos (no nulos).
 *
 * Dibuja el menú principal y ejecuta la acción correspondiente a la tecla presionada.
 *
 * Devuelve 0 para continuar ejecutando o 1 si se seleccionó salir.
 */
int frame_menu(const struct entrada *entrada, pantalla_t *pantalla,
	       struct juego *juego)
{
	int resultado = 0;
	dibujar_linea(pantalla, LINEA_TITULO_MENU, AMARILLO, "DEMO POKEMON");
	menu_con_cada_opcion(juego->menu, dibujar_opcion, pantalla);
	if ((entrada->caracter != CARACTER_NULO) &&
	    (!menu_ejecutar_clave(juego->menu, (char)entrada->caracter,
				  juego))) {
		resultado = 1;
	}
	return resultado;
}

/**
 * Se asume que entrada, pantalla y contexto son punteros válidos (no nulos).
 *
 * Modifica la pantalla siguiente a mostrar.
 *
 * Devuelve 0 para continuar la ejecución o 1 para finalizar el bucle gráfico.
 */
int procesar_frame(const struct entrada *entrada, pantalla_t *pantalla,
		   void *contexto)
{
	struct juego *juego = contexto;
	int resultado = 0;

	if (juego->modo == MODO_MENU) {
		resultado = frame_menu(entrada, pantalla, juego);
	} else if (juego->modo == MODO_ENTRADA) {
		frame_entrada(entrada, pantalla, juego);
	} else if (juego->modo == MODO_DIFICULTAD) {
		frame_dificultad(entrada, pantalla, juego);
	} else if (juego->modo == MODO_JUEGO) {
		frame_juego(pantalla, juego);
	} else if (juego->modo == MODO_PREGUNTA) {
		frame_pregunta(entrada, pantalla, juego);
	} else {
		frame_informativa(entrada, pantalla, juego);
	}
	return resultado;
}

/**
 * Libera toda la memoria dinámica asociada a la estructura struct juego.
 */
void destruir_juego(struct juego *juego)
{
	if (juego) {
		lista_destruir_todo(juego->pokemones_en_pantalla, free);
		free(juego->entrada);
		free(juego);
	}
}

/**
 * Reserva e inicializa la estructura principal de estado del juego gráfico.
 *
 * Devuelve un puntero al nuevo juego creado, o NULL en caso de error.
 */
struct juego *crear_juego(tp1_t *pokedex, menu_t *menu)
{
	struct juego *juego = calloc(1, sizeof(struct juego));
	if (juego) {
		juego->pokedex = pokedex;
		juego->menu = menu;
		juego->capacidad_entrada = VALOR_INICIAL_ENTRADA;
		juego->entrada = malloc(juego->capacidad_entrada);
		juego->pokemones_en_pantalla = lista_crear();
		if ((!juego->entrada) || (!juego->pokemones_en_pantalla)) {
			destruir_juego(juego);
			juego = NULL;
		} else {
			reiniciar_entrada(juego);
		}
	}
	return juego;
}

/**
 * Se asume que menu es un puntero válido (no nulo).
 *
 * Registra en el menú todas las opciones disponibles de la interfaz gráfica.
 *
 * Devuelve true si todas las opciones se agregaron correctamente, o false en caso de error.
 */
bool registrar_opciones_menu(menu_t *menu)
{
	return ((menu_agregar_opcion(menu, CLAVE_JUGAR, DESC_JUGAR,
				     accion_jugar)) &&
		(menu_agregar_opcion(menu, CLAVE_SEMILLA, DESC_SEMILLA,
				     accion_semilla)) &&
		(menu_agregar_opcion(menu, CLAVE_MOSTRAR, DESC_MOSTRAR,
				     accion_mostrar)) &&
		(menu_agregar_opcion(menu, CLAVE_INFO, DESC_INFO,
				     accion_info)) &&
		(menu_agregar_opcion(menu, CLAVE_SALIR, DESC_SALIR,
				     accion_salir)));
}

bool ejecutar_modo_grafico(tp1_t *pokedex, menu_t *menu, const char *puerto)
{
	if ((!pokedex) || (!menu)) {
		return false;
	}
	bool ejecutado = false;
	struct juego *juego = crear_juego(pokedex, menu);
	if ((juego != NULL) && registrar_opciones_menu(menu)) {
		struct engine_config configuracion = { 0 };
		configuracion.titulo = "Demo TP2";
		configuracion.puerto = puerto;
		ejecutado = (engine_correr(&configuracion, procesar_frame,
					   juego) == EXITO);
	}
	destruir_juego(juego);
	return ejecutado;
}