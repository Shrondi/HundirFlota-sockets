#ifndef _BARCO_
#define _BARCO_

#include <stdbool.h>

struct Barco {
    int tamanio; //Esta variable controla el tamaño actual si es tocado
    int tamanioOriginal; 
    int filaProa;
    int columnaProa;
    int filaPopa;
    int columnaPopa;
    int orientacion;
};

#endif