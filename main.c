#include <stdio.h>
#include "src/tp1.h"
#include "src/menu/menu.h"
#include "src/interfaz/interfaz_consola.h"

#define CANT_ARGUMENTOS_MINIMO 2
#define CANT_ARGUMENTOS_TERMINAL 2
#define CANT_ARGUMENTOS_MAXIMO 3
#define ERROR -1
#define POS_ARGV_ARCHIVO 1
#define POS_ARGV_PUERTO 2
#define EXITO 0

int main(int argc, char *argv[])
{
	if ((argc < CANT_ARGUMENTOS_MINIMO) || (argc > CANT_ARGUMENTOS_MAXIMO)){
		printf("Cantidad de argumentos incorrecta\n");
		return ERROR;
	}

	tp1_t* pokedex = tp1_leer_archivo(argv[POS_ARGV_ARCHIVO]);
	if (!pokedex){
		printf ("Error al intentar leer el archivo\n");
		return ERROR;
	}

	menu_t* menu = menu_crear();
	if (!menu) {
		tp1_destruir(pokedex);
		return ERROR;
	}
    int resultado = EXITO;
    if (argc == CANT_ARGUMENTOS_TERMINAL){
        if (!ejecutar_modo_consola(pokedex, menu)){
            resultado = ERROR;
        }
    }
    else{
        if (!ejecutar_modo_grafico(pokedex,menu,argv[POS_ARGV_PUERTO])){
			resultado = ERROR;
		}
    }
	menu_destruir(menu);
	tp1_destruir(pokedex);
	return resultado;
}