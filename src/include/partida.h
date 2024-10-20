#ifndef _PARTIDA_
#define _PARTIDA_

#include "macros.h"
#include "barco.h"

struct Partida
{
    int sdJugador1;
    int sdJugador2;
    char tableroJugador1[TAM_BOARD][TAM_BOARD];
    char tableroJugador2[TAM_BOARD][TAM_BOARD];
    struct Barco barcosJugador1[NUM_SHIPS];
    struct Barco barcosJugador2[NUM_SHIPS];
    int disparosJugador1;
    int disparosJugador2;
};

#endif