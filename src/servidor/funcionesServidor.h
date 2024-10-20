#ifndef _FUNCIONES_SERVIDOR_
#define _FUNCIONES_SERVIDOR_

#include "macros.h"
#include "usuario.h"
#include "partida.h"
#include "barco.h"
#include <stdbool.h>

bool archivoUsuarios(char *nombreArchivo);

void inicializarClientes(struct Usuario clientes[MAX_CLIENTS]);

int obtenerPosicionCliente(struct Usuario clientes[MAX_CLIENTS], int sd);

bool obtenerUsuario(char *usuario);

char *obtenerContrasenia(char *usuario);

bool registrarUsuario(char *usuario, char *contrasenia);

void generarTablero(char tablero[TAM_BOARD][TAM_BOARD], int numeroBarcos[TYPE_SHIP], int tamanioBarcos[TYPE_SHIP], struct Barco barcos[NUM_SHIPS]);

bool posicionValida(char tablero[TAM_BOARD][TAM_BOARD], int fila, int columna, int tamano, int orientacion);

void inicializarTablero(char tablero[TAM_BOARD][TAM_BOARD]);

void imprimirTablero(char tablero[TAM_BOARD][TAM_BOARD]);

int buscarContrincante(struct Usuario clientes[MAX_CLIENTS], int sd);

bool encontrarUsuario(struct Usuario clientes[MAX_CLIENTS], char *usuario);

void formarPartida(struct Usuario clientes[MAX_CLIENTS], struct Partida partidas[MAX_MATCHES], int numPartidas, int numeroBarcos[TYPE_SHIP], int tamanioBarcos[TYPE_SHIP], int sdJugador1, int sdJugador2);

void convertirTableroCadena(char *buffer, char tablero[TAM_BOARD][TAM_BOARD]);

void append(char *s, char c);

int disparo(char columna, int fila, struct Usuario clientes[MAX_CLIENTS], int posicionCliente, struct Barco barcos[NUM_SHIPS]);

bool validarCoordenadas(char columna, int fila);

bool barcoHundido(int columna, int fila, struct Barco barcos[NUM_SHIPS], int *posBarco);

bool barcoTocado(int columna, int fila, struct Barco barco);

void actualizarBarcoAHundido(char(*punteroTablero)[TAM_BOARD], struct Barco barco);

bool comprobarGanador(struct Barco barcos[NUM_SHIPS]);

#endif