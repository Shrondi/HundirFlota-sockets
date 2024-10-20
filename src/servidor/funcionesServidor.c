#include "funcionesServidor.h"
#include "usuario.h"
#include "partida.h"
#include "barco.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

bool archivoUsuarios(char *nombreArchivo){

    FILE *archivo;
    archivo = fopen(nombreArchivo, "r");

    if (archivo) {
        fclose(archivo);
        return true;
    }

    return false;
}

void inicializarClientes(struct Usuario clientes[MAX_CLIENTS])
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clientes[i].sd = -1;
        clientes[i].estado = 0;
        clientes[i].jugador1 = false;
        clientes[i].turno = false;
        clientes[i].sdContrincante = -1;
        clientes[i].partida = NULL;
    }
}

int obtenerPosicionCliente(struct Usuario clientes[MAX_CLIENTS], int sd)
{

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (sd == clientes[i].sd)
        {
            return i;
        }
    }

    return -1;
}

bool obtenerUsuario(char *usuario)
{

    char usuarioFichero[MAX_INPUT_USER], contraseniaFichero[MAX_INPUT_USER];
    FILE *file;

    file = fopen("usuarios.txt", "r");

    if (file == NULL)
    {
        perror("Error");
        exit(EXIT_FAILURE); // TO DO Salir bien cerrando clientes y comprobar que existe el fichero antes de iniciar sesion
    }

    while (fscanf(file, "%s %s\n", usuarioFichero, contraseniaFichero) != EOF)
    {
        if (strcmp(usuarioFichero, usuario) == 0)
        {

            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

char *obtenerContrasenia(char *usuario)
{
    char *str = malloc(MAX_INPUT_USER * sizeof(char));

    char usuarioFichero[MAX_INPUT_USER], contraseniaFichero[MAX_INPUT_USER];

    FILE *file;

    file = fopen("usuarios.txt", "r");

    if (file == NULL)
    {
        perror("Error");
        return false; // TO DO Salir bien cerrando clientes y comprobar que existe el fichero antes de iniciar sesion
    }

    while (fscanf(file, "%s %s\n", usuarioFichero, contraseniaFichero) != EOF)
    {
        if (strcmp(usuarioFichero, usuario) == 0)
        {

            strcpy(str, contraseniaFichero);
            fclose(file);

            return str;
        }
    }

    fclose(file);

    return NULL;
}

bool registrarUsuario(char *usuario, char *contrasenia)
{

    FILE *file;

    file = fopen("usuarios.txt", "a");

    if (file == NULL)
    {
        perror("Error");
        return false; // TO DO Salir bien cerrando clientes y comprobar que existe el fichero antes de iniciar sesion
    }

    if (obtenerUsuario(usuario))
    {
        return false;
    }

    if (fprintf(file, "%s %s\n", usuario, contrasenia) < 0)
    {
        perror("Error: ");
        fclose(file);

        return false;
    }

    fclose(file);
    return true;
}

void generarTablero(char tablero[TAM_BOARD][TAM_BOARD], int numeroBarcos[TYPE_SHIP], int tamanioBarcos[TYPE_SHIP], struct Barco barcos[NUM_SHIPS])
{

    // Rellenamos el tablero todo con agua
    inicializarTablero(tablero);

    int columna, fila, orientacion;

    int n = 0;
    // Recorremos todos los tipos de barcos
    for (int k = 0; k < TYPE_SHIP; k++)
    {

        // Se recorre el array que indica el número de barcos de cada tipo
        for (int i = 0; i < numeroBarcos[k]; i++)
        {

            // Tamaño de cada barco
            int tamano = tamanioBarcos[k];

            barcos[n].tamanio = tamano;
            barcos[n].tamanioOriginal = tamano;

            // Se genera aleatoriamente la columna, fila y orientacion de cada barco
            // hasta que sea una posicion valida
            do
            {
                columna = rand() % TAM_BOARD;
                fila = rand() % TAM_BOARD;
                orientacion = rand() % 2; // Vertical=1, Horizontal=0

            } while (!posicionValida(tablero, fila, columna, tamano, orientacion));

            barcos[n].filaProa = fila;
            barcos[n].columnaProa = columna;
            barcos[n].orientacion = orientacion;

            // Coloca el barco en la cuadrícula
            if (orientacion == 0)
            { // Horizontal
                for (int j = 0; j < tamano; j++)
                {
                    tablero[fila][columna + j] = 'B'; // Usamos 'B' para representar un barco
                }

                barcos[n].filaPopa = fila;
                barcos[n].columnaPopa = columna + tamano-1;
            }
            else
            { // Vertical
                for (int j = 0; j < tamano; j++)
                {
                    tablero[fila + j][columna] = 'B'; // Usamos 'B' para representar un barco
                }

                barcos[n].filaPopa = fila + tamano-1;
                barcos[n].columnaPopa = columna;
            }

            ++n;
        }
    }
}

// Función para verificar si una posición es válida para colocar un barco con orientación
bool posicionValida(char tablero[TAM_BOARD][TAM_BOARD], int fila, int columna, int tamano, int orientacion)
{

    // Matriz que contiene las 8 direcciones. La primera columna -> filas, segunda columna -> columnas
    int direcciones[8][2] = {
        {-1, -1}, // Diagonal de arriba a la izquierda
        {-1, 0},  // Arriba
        {-1, 1},  // Diagonal arriba a la derecha
        {0, -1},  // Izquierda
        {0, 1},   // Derecha
        {1, -1},  // Diagonal abajo a la izquierda
        {1, 0},   // Abajo
        {1, 1}    // Diagonal abajo a la derecha
    };

    // Comprobar para cada casilla que forme una parte del barco
    for (int j = 0; j < tamano; j++)
    {

        // Se comprueba que las 8 direcciones que sean agua
        for (int i = 0; i < 8; i++)
        {

            // Se calculan laS celdas vecinas
            int filaVecina = fila + direcciones[i][0];
            int columnaVecina = columna + direcciones[i][1];

            // Solo se comprueban posiciones dentro del tablero
            if (filaVecina >= 0 && filaVecina < TAM_BOARD && columnaVecina >= 0 && columnaVecina < TAM_BOARD)
            {
                if (tablero[filaVecina][columnaVecina] == 'B')
                {

                    // Hay un barco en la celda de al lado
                    return false;
                }
            }
        }

        // Añadimos una parte más del barco según su posición
        if (orientacion == 0)
        { // Horizontal
            ++columna;
        }
        else
        { // Vertical
            ++fila;
        }

        // Comprobamos que el barco no se salga fuera del tablero
        if (fila < 0 || fila >= TAM_BOARD || columna < 0 || columna >= TAM_BOARD)
        {

            return false;
        }
    }

    // El barco está colocado perfectamente
    return true;
}

void inicializarTablero(char tablero[TAM_BOARD][TAM_BOARD])
{

    for (int i = 0; i < TAM_BOARD; i++)
    {
        for (int j = 0; j < TAM_BOARD; j++)
        {
            //'A' -> Agua
            tablero[i][j] = 'A';
        }
    }
}

void imprimirTablero(char tablero[TAM_BOARD][TAM_BOARD])
{
    for (int i = 0; i < TAM_BOARD; i++)
    {
        for (int j = 0; j < TAM_BOARD; j++)
        {
            if (tablero[i][j] == 'A')
            {
                printf("%c ", '~');
            }
            else
            {
                printf("%c ", tablero[i][j]);
            }
        }
        printf("\n");
    }

    printf("\n\n");
}

bool encontrarUsuario(struct Usuario clientes[MAX_CLIENTS], char *usuario)
{

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clientes[i].estado >= 1 && strcmp(clientes[i].usuario, usuario) == 0)
        {
            return true;
        }
    }

    return false;
}

int buscarContrincante(struct Usuario clientes[MAX_CLIENTS], int sd)
{

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clientes[i].estado == 2 && clientes[i].sd != sd)
        {
            return clientes[i].sd;
        }
    }

    return -1;
}

void formarPartida(struct Usuario clientes[MAX_CLIENTS], struct Partida partidas[MAX_MATCHES], int numPartidas, int numeroBarcos[TYPE_SHIP], int tamanioBarcos[TYPE_SHIP], int sdJugador1, int sdJugador2)
{

    generarTablero(partidas[numPartidas - 1].tableroJugador1, numeroBarcos, tamanioBarcos, partidas[numPartidas - 1].barcosJugador1);
    generarTablero(partidas[numPartidas - 1].tableroJugador2, numeroBarcos, tamanioBarcos, partidas[numPartidas - 1].barcosJugador2);

    partidas[numPartidas - 1].disparosJugador1 = 0;
    partidas[numPartidas - 1].disparosJugador2 = 0;

    clientes[obtenerPosicionCliente(clientes, sdJugador1)].partida = &partidas[numPartidas - 1];
    clientes[obtenerPosicionCliente(clientes, sdJugador2)].partida = &partidas[numPartidas - 1];

}

void convertirTableroCadena(char *buffer, char tablero[TAM_BOARD][TAM_BOARD])
{

    for (int i = 0; i < TAM_BOARD; i++)
    {
        for (int j = 0; j < TAM_BOARD; j++)
        {

            append(buffer, tablero[i][j]);
            append(buffer, ',');
        }

        buffer[strlen(buffer) - 1] = ';';
    }

    buffer[strlen(buffer) - 1] = '.';
    buffer[strlen(buffer)] = '\0';
}

void append(char *s, char c)
{
    int len = strlen(s);
    s[len] = c;
    s[len + 1] = '\0';
}

int disparo(char columna, int fila, struct Usuario clientes[MAX_CLIENTS], int posicionCliente, struct Barco barcos[NUM_SHIPS])
{

    char(*punteroTablero)[TAM_BOARD];

    // Según el jugador que haya hecho el disparo, se modifica el tablero del contrincante
    if (clientes[posicionCliente].jugador1)
    {
        punteroTablero = clientes[posicionCliente].partida->tableroJugador2;
    }
    else
    {
        punteroTablero = clientes[posicionCliente].partida->tableroJugador1;
    }

    int columnaMatriz = (int)(columna - 'A');
    int filaMatriz = fila - 1;

    if (punteroTablero[filaMatriz][columnaMatriz] == 'A')
    {
        return 0;
    }
    else if (punteroTablero[filaMatriz][columnaMatriz] == 'B')
    {

        int posBarco = 0;
        if (barcoHundido(columnaMatriz, filaMatriz, barcos, &posBarco)){

            actualizarBarcoAHundido(punteroTablero, barcos[posBarco]);
            return 2;
        }

        return 1;
    }else if (punteroTablero[filaMatriz][columnaMatriz] == 'H'){

        return 2;
    }
}

bool validarCoordenadas(char columna, int fila)
{
    // Verifica que el primer carácter sea una letra entre A y H (mayúscula o minúscula)
    if (!isalpha(columna) || columna < 'A' || columna > 'J')
    {
        return false;
    }

    if (fila < 1 || fila > 10)
    {
        return false;
    }

    return true;
}

bool barcoHundido (int columna, int fila, struct Barco barcos[NUM_SHIPS], int *posBarco){

    //Se recorren todos los barcos
    for (int i = 0; i < NUM_SHIPS; i++){

        // Para evitar tamaños negativos ya que no se controla el disparo reiterado sobre la misma casilla
        if (barcos[i].tamanio >= 0){

            if (barcoTocado(columna, fila, barcos[i])){
                --(barcos[i].tamanio);
            }


            if (barcos[i].tamanio == 0){
                *posBarco = i;

                //Se marca el tamaño como -1 para que no se vuelva a comprobar más este barco
                barcos[i].tamanio = -1;
                return true;
            }
           
        }

    }

    return false;
}


//TODO CONTROLAR QUE EL USUARIO NO INTRODUZCA LAS MISMAS COORDENADAS
bool barcoTocado (int columna, int fila, struct Barco barco){
     // Se comprueba si el disparo es sobre la misma columna de un barco
    if (columna == barco.columnaProa && columna == barco.columnaPopa) {
        // Se comprueba el rango
        if ((barco.filaProa <= fila && fila <= barco.filaPopa) ||
            (barco.filaProa >= fila && fila >= barco.filaPopa)) {
            return true;

        }

    // Se comprueba si el disparo es sobre la misma fila de un barco
    } else if (fila == barco.filaProa && fila == barco.filaPopa) {

        // Se comprueba el rango
        if ((barco.columnaProa <= columna && columna <= barco.columnaPopa) ||
            (barco.columnaProa >= columna && columna >= barco.columnaPopa)) {
            return true;

        }
    }

    return false;
}

void actualizarBarcoAHundido(char(*punteroTablero)[TAM_BOARD], struct Barco barco){

    // Coloca el barco en la cuadrícula
    if (barco.orientacion == 0)
    { // Horizontal
        for (int j = 0; j < barco.tamanioOriginal; j++)
        {
            punteroTablero[barco.filaProa][barco.columnaProa + j] = 'H'; // Usamos 'H' para representar un barco hundido
        }
    }
    else
    { // Vertical
        for (int j = 0; j < barco.tamanioOriginal; j++)
        {
            punteroTablero[barco.filaProa + j][barco.columnaProa] = 'H'; // Usamos 'H' para representar un barco hundido
        }
    }
}

bool comprobarGanador (struct Barco barcos[NUM_SHIPS]){

    int numBarcosHundidos = 0;
    for (int i = 0; i < NUM_SHIPS; i++){
        
        if (barcos[i].tamanio == -1){
            ++numBarcosHundidos;
        }
    }

    return numBarcosHundidos == NUM_SHIPS;
}
