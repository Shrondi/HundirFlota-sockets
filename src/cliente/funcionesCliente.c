#include "funcionesCliente.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "macros.h"

void bienvenida(char *buffer, char usuario[MAX_INPUT_USER])
{

    int opcion;
    bool entradaCorrecta = false;

    printf("\nBienvenido a Hundir la Flota\n");

    do
    {
        char *p, input[MAX_INPUT_USER];

        while (!entradaCorrecta)
        {
            printf("\n1. Iniciar sesión\n");
            printf("2. Registrarse\n");
            printf("3. Salir\n");
            printf("Seleccione la opción deseada: ");

            if (fgets(input, sizeof(input), stdin) != NULL)
            {

                opcion = strtol(input, &p, 10);
                if (p == input || *p != '\n')
                {
                    printf("\nEntrada no válida\n");
                }
                else
                {
                    entradaCorrecta = true;
                }
            }
            else
            {
                printf("\nCarácter EOF no válido\n");
                clearerr(stdin);
            }
        }

        switch (opcion)
        {
        case 1:
            iniciarSesionUsuario(buffer, usuario);
            break;
        case 2:
            registro(buffer);
            break;
        case 3:
            strcpy(buffer, "SALIR\n");
            break;
        default:
            printf("Opción incorrecta\n");
            entradaCorrecta = false;
        }

    } while (opcion < 1 || opcion > 3);
}

void home(char *buffer, char usuario[MAX_INPUT_USER])
{

    int opcion;
    bool entradaCorrecta = false;

    do
    {
        char *p, input[MAX_INPUT_USER];
        while (!entradaCorrecta)
        {
            printf("\n¡Bienvenido/a de nuevo, %s!\n", usuario);

            printf("\n1. Buscar partida\n");
            printf("2. Salir\n");
            printf("¿Qué desea hacer?: ");

            if (fgets(input, sizeof(input), stdin) != NULL)
            {

                opcion = strtol(input, &p, 10);
                if (p == input || *p != '\n')
                {
                    printf("\nEntrada no válida\n");
                }
                else
                {
                    entradaCorrecta = true;
                }
            }
            else
            {
                printf("\nCarácter EOF no válido\n");
                clearerr(stdin);
            }
        }

        switch (opcion)
        {
        case 1:
            strcpy(buffer, "INICIAR-PARTIDA");
            break;
        case 2:
            strcpy(buffer, "SALIR\n");
            break;
        default:
            printf("Opción incorrecta\n");
            entradaCorrecta = false;
        }

    } while (opcion < 1 || opcion > 2);
}

void iniciarSesionUsuario(char *buffer, char usuario[MAX_INPUT_USER])
{
    char nombreUsuario[MAX_INPUT_USER];
    int entradaCorrecta = 0;

    printf("\n\nIntroduzca su usuario para iniciar sesión: \n");

    while (!entradaCorrecta)
    {
        printf("Usuario: ");
        entradaCorrecta = comprobarEntrada(nombreUsuario);
    }

    sprintf(buffer, "USUARIO %s", nombreUsuario);

    strcpy(usuario, nombreUsuario);
}

void iniciarSesionContrasenia(char *buffer)
{
    char contrasenia[MAX_INPUT_USER];
    bool entradaCorrecta = false;

    printf("\t\nUsuario correcto\n\n");

    while (!entradaCorrecta)
    {
        printf("Contrasenia: ");
        entradaCorrecta = comprobarEntrada(contrasenia);
    }

    sprintf(buffer, "PASSWORD %s", contrasenia);
}

void registro(char *buffer)
{

    char usuario[MAX_INPUT_USER];
    char contrasenia[MAX_INPUT_USER];

    bool entradaCorrecta = false;

    printf("\n\nCree una nueva cuenta con un usuario y contrasenia:\n");

    while (!entradaCorrecta)
    {
        printf("Nombre de usuario (máx. %d caracteres): ", MAX_INPUT_USER);
        entradaCorrecta = comprobarEntrada(usuario);
    }

    entradaCorrecta = false;
    while (!entradaCorrecta)
    {
        printf("Contrasenia (máx. %d caracteres): ", MAX_INPUT_USER);

        entradaCorrecta = comprobarEntrada(contrasenia);
    }

    sprintf(buffer, "REGISTRO -u %s -p %s", usuario, contrasenia);
}

bool comprobarEntrada(char *entrada)
{

    if (fgets(entrada, MAX_INPUT_USER, stdin) != NULL)
    {

        // Verificar si la entrada contiene algún espacio o solo es un salto de línea
        if (strpbrk(entrada, " ") != NULL || strcmp(entrada, "\n") == 0)
        {
            printf("\nCampo vacío. Inténtelo de nuevo\n");
        }
        else
        {
            size_t len = strlen(entrada);
            // printf("%d\n", len);

            if (len > 0 && entrada[len - 1] != '\n')
            {

                // El usuario ingresó más de caracteres de los permitidos, descartar el resto de la línea
                printf("\nSe ha introducido más caracteres de los permitidos. Inténtelo de nuevo.\n");
                int c;
                while ((c = getchar()) != '\n' && c != EOF)
                {
                }
            }
            else
            {
                // Eliminar el carácter de nueva línea si está presente
                entrada[len - 1] = '\0';
                return true; // Salir, la entrada es válida
            }
        }
    }
    else
    {
        printf("\nCarácter EOF no válido\n");
        clearerr(stdin);
    }

    return false;
}

void convertirCadenaTablero(char *buffer, char tablero[TAM_BOARD][TAM_BOARD])
{

    int fila = 0, columna = 0;

    for (int k = 0; k < strlen(buffer); k++)
    {
        if (buffer[k] == 'A')
        {
            tablero[fila][columna] = 'A';
            ++columna;
        }
        else if (buffer[k] == 'B')
        {
            tablero[fila][columna] = 'B';
            ++columna;
        }
        else if (buffer[k] == ';')
        {
            ++fila;
            columna = 0;
        }
    }
}

void imprimirTableros(char tablero[TAM_BOARD][TAM_BOARD], char tableroContrincante[TAM_BOARD][TAM_BOARD])
{

    system("clear");

    printf("\t Mi Tablero\t\t\t        Tablero Contrincante\n");
    printf("\t -------------------\t\t        ---------------------\n\n");
    printf("\t   A B C D E F G H I J\t\t          A B C D E F G H I J\n");

    for (int i = 0; i < 10; i++)
    {
        printf("\t");
        printf("%2d ", i + 1); // Etiqueta de fila

        for (int j = 0; j < 10; j++)
        {

            // Como es el tablero del jugador, el agua y los barcos son visibles
            if (tablero[i][j] == 'A')
            {
                printf("%c ", '~');
            }
            else
            {
                printf("%c ", tablero[i][j]);
            }
        }

        printf("                "); // Espacio entre las dos matrices

        printf("%2d ", i + 1); // Etiqueta de fila

        for (int j = 0; j < 10; j++)
        {

            // Cuando se inicializa el tablero, las casillas con 0 no se han descubierto todavia,
            //  por tanto se muestran con un '-'
            if (tableroContrincante[i][j] == '0')
            {
                printf("%c ", '-');

                // El tablero está formado por agua (A) y barcos (B)
            }
            else if (tableroContrincante[i][j] == 'A')
            {
                printf("%c ", '~'); // Mostramos el agua con ~
            }
            else if (tableroContrincante[i][j] == 'B')
            {
                printf("%c ", 'X'); // Al ser descubierto un barco, es decir, es tocado, se muestra con x
            }else{
                printf("%c ", tableroContrincante[i][j]);
            }
        }
        printf("\n");
    }
}

void inicializarTablero(char tablero[TAM_BOARD][TAM_BOARD])
{

    // Se inicializa el tablero del contrincante con 0
    for (int i = 0; i < TAM_BOARD; i++)
    {
        for (int j = 0; j < TAM_BOARD; j++)
        {
            //'0' -> No se ha descubierto
            tablero[i][j] = '0';
        }
    }
}

bool partida(char tablero[TAM_BOARD][TAM_BOARD], char tableroContrincante[TAM_BOARD][TAM_BOARD], char *buffer)
{

    bool entradaCorrecta = false;
    char columna;
    int fila;
    char aux[25] = "";
    char entrada[MAX_INPUT_USER];

    while (!entradaCorrecta)
    {

        imprimirTableros(tablero, tableroContrincante);

        printf("%s", aux);
        printf("\n* Coordenadas disparo: ");

        entradaCorrecta = comprobarEntrada(entrada);

        // Dejamos la posibilidad al jugador de salir durante su turno
        if (strcmp(entrada, "SALIR") == 0)
        {
            return false;
        }

        entradaCorrecta = validarCoordenadas(entrada);

        if (!entradaCorrecta)
        {
            strcpy(aux, "\nCoordenada incorrecta\n");
        }
    }

    columna = entrada[0];
    fila = atoi(&entrada[1]);

    sprintf(buffer, "DISPARO %c,%d", columna, fila);

    return true;
}

bool validarCoordenadas(char entrada[MAX_INPUT_USER])
{
    // Verifica que el primer carácter sea una letra entre A y H (mayúscula o minúscula)
    if (!isalpha(entrada[0]) || entrada[0] < 'A' || entrada[0] > 'J')
    {
        return false;
    }

    int fila = atoi(&entrada[1]);
    if (fila < 1 || fila > 10)
    {
        return false;
    }

    return true;
}