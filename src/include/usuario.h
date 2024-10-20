#ifndef _USUARIO_
#define _USUARIO_

#include "macros.h"
#include "partida.h"
#include <stdbool.h>

struct Usuario
{

    int sd;
    int estado;
    char usuario[MAX_INPUT_USER];
    char contraseniaUsuario[MAX_INPUT_USER];
    struct Partida *partida;
    int sdContrincante;
    bool jugador1;
    bool turno;
};

#endif