#ifndef _FUNCIONES_CLIENTE_
#define _FUNCIONES_CLIENTE_

#include <stdbool.h>
#include "macros.h"

void iniciarSesionUsuario(char *buffer, char *usuario);

void iniciarSesionContrasenia(char *buffer);

bool comprobarEntrada(char *entrada);

void registro(char *buffer);

void bienvenida(char *buffer, char usuario[MAX_INPUT_USER]);

void home(char *buffer, char usuario[MAX_INPUT_USER]);

void convertirCadenaTablero(char *buffer, char tablero[TAM_BOARD][TAM_BOARD]);


void imprimirTableros(char tablero[TAM_BOARD][TAM_BOARD], char tableroContrincante[TAM_BOARD][TAM_BOARD]);

void inicializarTablero(char tablero[TAM_BOARD][TAM_BOARD]);

bool validarCoordenadas(char entrada[MAX_INPUT_USER]);

bool partida(char tablero[TAM_BOARD][TAM_BOARD], char tableroContrincante[TAM_BOARD][TAM_BOARD], char *buffer);

#endif