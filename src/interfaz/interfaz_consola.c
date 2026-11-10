#include "interfaz_consola.h"
#include "../menu/menu.h"
#include <stdio.h>
#include <stdlib.h>

#define CLAVE_JUGAR 'J'
#define CLAVE_SEMILLA 'S'
#define CLAVE_MOSTRAR 'M'
#define CLAVE_INFO 'I'
#define CLAVE_SALIR 'Q'
#define CANT_CARACTER_SOLICITADO 1
#define SALTO_DE_LINEA '\n'

const char *DESC_JUGAR = "Jugar";
const char *DESC_SEMILLA = "Jugar con semilla";
const char *DESC_MOSTRAR = "Mostrar pokemon";
const char *DESC_INFO = "Mostrar informacion";
const char *DESC_SALIR = "Salir";

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
 * Informa por pantalla la salida del programa.
 * 
 * Devuelve false para finalizar el mismo. 
 */
bool accion_salir()
{
    printf("Saliendo del programa...\n");
    return false; 
}

/**
 * Informa por pantalla que la opción "juego" no se puede ejecutar por consola.
 * 
 * Devuelve true para continuar con el programa
 */
bool accion_jugar()
{
    printf("\n[!] El juego interactivo solo se puede ejecutar en modo grafico.\n");
    return true;
}

/**
 * Informa por pantalla que la opción "semilla" no se puede ejecutar por consola.
 * 
 * Devuelve true para continuar con el programa.
 */
bool accion_semilla()
{
    printf("\n[!] El juego con semilla solo se puede ejecutar en modo grafico.\n");
    return true;
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
    char nombre[100];

    printf("Ingrese el nombre del pokemon a buscar: \n");
    if (scanf(" %99s", nombre) != 1) {
        return true;
    }
    struct pokemon *p = tp1_buscar_pokemon(pokedex, nombre);
    if (!p) {
        printf("Pokemon no encontrado.\n");
    } else {
        printf("Nombre: %s\n", p->nombre);
        printf("Velocidad: %d\n", p->velocidad);
        printf("Peso: %.1f\n", p->peso);
        if (p->rareza == RAREZA_LEGENDARIO) {
            printf("Rareza: Legendario\n");
        } else if (p->rareza == RAREZA_RARO) {
            printf("Rareza: Raro\n");
        } else {
            printf("Rareza: Comun\n");
        }
    }
    return true;
}

/**
 * 
 * Actualiza las estadísticas de la pokedex con el pokemon recibido por parametro.
 */
bool contar_pokemon(struct pokemon *p, void *extra)
{
    struct resumen_pokedex *resumen = (struct resumen_pokedex *)extra;
    if (p->rareza == RAREZA_COMUN) {
        resumen->comunes++;
    } else if (p->rareza == RAREZA_RARO) {
        resumen->raros++;
    } else{
        resumen->legendarios++;
    }
    if ((!resumen->mas_veloz) || (p->velocidad > resumen->mas_veloz->velocidad)) {
        resumen->mas_veloz = p;
    }
    if ((!resumen->menos_veloz) || (p->velocidad < resumen->menos_veloz->velocidad)) {
        resumen->menos_veloz = p;
    }
    if ((!resumen->mas_pesado) || (p->peso > resumen->mas_pesado->peso)) {
        resumen->mas_pesado = p;
    }
    if ((!resumen->menos_pesado) || (p->peso < resumen->menos_pesado->peso)) {
        resumen->menos_pesado = p;
    }
    resumen->total++;
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
    struct resumen_pokedex resumen = {0};
    tp1_iterar(pokedex, contar_pokemon, &resumen);
    printf("\n--- INFORMACION DE LA POKEDEX ---\n");
    printf("\nTotal de pokemones: %zu\n", resumen.total);
    printf("Comunes: %zu\n", resumen.comunes);
    printf("Raros: %zu\n", resumen.raros);
    printf("Legendarios: %zu\n", resumen.legendarios);
    if (resumen.total > 0) {
        printf("Mas veloz: %s (%d)\n", resumen.mas_veloz->nombre, resumen.mas_veloz->velocidad);
        printf("Menos veloz: %s (%d)\n", resumen.menos_veloz->nombre, resumen.menos_veloz->velocidad);
        printf("Mas pesado: %s (%.1f)\n", resumen.mas_pesado->nombre, resumen.mas_pesado->peso);
        printf("Menos pesado: %s (%.1f)\n", resumen.menos_pesado->nombre, resumen.menos_pesado->peso);
    }
    return true;
}

/**
 * El puntero descripcion es un puntero válido finalizado en '\0'
 * 
 * Imprime por pantalla todas las claves y su descripción correspondiente del menu.
 */
bool imprimir_opcion_consola(char clave, const char *descripcion, size_t indice, void *extra)
{
    (void)indice; 
    (void)extra;  
    printf("\n  [%c] %s\n", clave, descripcion);
    return true; 
}

bool ejecutar_modo_consola(tp1_t *pokedex,menu_t* menu)
{
    menu_agregar_opcion(menu, CLAVE_JUGAR, DESC_JUGAR, accion_jugar); //Le asocio al caracter J la descripcion y un puntero a funcion (callbackq)
    menu_agregar_opcion(menu, CLAVE_SEMILLA, DESC_SEMILLA, accion_semilla);
    menu_agregar_opcion(menu, CLAVE_MOSTRAR, DESC_MOSTRAR, accion_mostrar_pokemon);
    menu_agregar_opcion(menu, CLAVE_INFO, DESC_INFO, accion_mostrar_informacion);
    menu_agregar_opcion(menu, CLAVE_SALIR, DESC_SALIR, accion_salir);

    bool seguir = true;
    while (seguir) {
        printf("\n ###   MENU PRINCIPAL  ####   \n");
        menu_con_cada_opcion(menu, imprimir_opcion_consola, NULL);
        printf("\nSeleccione una opcion: ");
        char opcion = ' ';
        if (scanf(" %c", &opcion) != CANT_CARACTER_SOLICITADO) {
            seguir = false;
        } else {
            bool formato_invalido = false;
            int c = getchar();
            while ((c != SALTO_DE_LINEA) && (c != EOF)) {
               formato_invalido = true;
                c = getchar();
            }
            if (formato_invalido) {
                printf("\n[!] Formato invalido: debe ingresar unicamente una letra.\n");
            } else if (menu_contiene_clave(menu, opcion)) {
                seguir = menu_ejecutar_clave(menu, opcion, pokedex);
            } else {
                printf("\n[!] Opcion '%c' invalida. Ingrese una de las opciones del menu.\n", opcion);
            }
        }
    }
    return true;
}