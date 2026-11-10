#include "validadores.h"
#include <stdlib.h>

#define CARACTER_NULO '\0'
#define BASE_DECIMAL 10
#define VELOCIDAD_MINIMA 1
#define VELOCIDAD_MAXIMA 99
#define PESO_MINIMO 0.0f
#define CHAR_COMUN 'C'
#define CHAR_RARO 'R'
#define CHAR_LEGENDARIO 'L'

bool peso_valido(char *str, float *peso)
{
	
	if ((!str) || (*str == CARACTER_NULO) || (!peso)) { 
		return false;
	}
    char *fin = NULL;
	float valor = strtof(str, &fin); 
	if (*fin != CARACTER_NULO) { 
		return false;
	}
	if (valor <= PESO_MINIMO) { 
		return false;
	}
	*peso = valor;
	return true;
}

bool velocidad_valida(char *str, int *velocidad)
{
	if ((!str) || (*str == CARACTER_NULO) || (!velocidad)) {
		return false;
	}
    char* fin = NULL;
	long valor = strtol(str, &fin, BASE_DECIMAL); 
	if (*fin != CARACTER_NULO) { 
		return false;
	}
	if ((valor < VELOCIDAD_MINIMA) || (valor > VELOCIDAD_MAXIMA)) {
		return false;
	}
	*velocidad = (int)valor;
	return true;
}

bool rareza_valida(char *str, enum rareza_pokemon *rareza)
{
	if ((!str) || (str[0] == CARACTER_NULO) || (str[1] != CARACTER_NULO)) { 
		return false;
	}
	if (str[0] == CHAR_COMUN) {
		*rareza = RAREZA_COMUN;
	} else if (str[0] == CHAR_RARO) {
		*rareza = RAREZA_RARO;
	} else if (str[0] == CHAR_LEGENDARIO) {
		*rareza = RAREZA_LEGENDARIO;
	} else {
		return false; 
	}
	return true;
}

bool nombre_valido(const char *nombre)
{
    return ((nombre != NULL) && (nombre[0] != CARACTER_NULO));
}