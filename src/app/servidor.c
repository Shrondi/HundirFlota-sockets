#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include <stdbool.h>

#include "funcionesServidor.h"
#include "usuario.h"
#include "macros.h"
#include "partida.h"
#include "barco.h"

/*
 * El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set *readfds, int *numClientes, struct Usuario clientes[MAX_CLIENTS]);

int main()
{
    srand(time(NULL));

    /*----------------------------------------------------
        Descriptor del socket y buffer de datos
    -----------------------------------------------------*/
    int sd, new_sd;
    struct sockaddr_in sockname, from;
    char buffer[MSG_SIZE];
    socklen_t from_len;
    fd_set readfds, auxfds;
    int salida;
    int numClientes = 0;
    int numPartidas = 0;
    int recibidos;
    char identificador[MSG_SIZE];

    int on;

    /*----------------------------------------------------
       Se comprueba si existe el archivo de usuarios registrados
    -----------------------------------------------------*/

    char *nombreArchivo = "usuarios.txt";
    if (!archivoUsuarios(nombreArchivo)){
        printf("ERROR. No existe el fichero de usuarios registrados. \nCree un archivo llamado '%s' para iniciar el servidor", nombreArchivo);
        exit(EXIT_FAILURE);
    }

    /*----------------------------------------------------
        Variables del juego
    -----------------------------------------------------*/

    struct Usuario clientes[MAX_CLIENTS]; //Información de los clientes conectados
    struct Partida partidas[MAX_MATCHES]; //Información de las partidas actuales

    int numeroBarcos[TYPE_SHIP] = {1, 2, 2}; //Numero de barcos por cada tipo
    int tamanioBarcos[TYPE_SHIP] = {4, 3, 2}; //Tamaño de cada tipo de barco


    /* --------------------------------------------------
        Se abre el socket
    ---------------------------------------------------*/

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("No se puede abrir el socket cliente\n");
        exit(1);
    }

    // Activaremos una propiedad del socket para permitir· que otros
    // sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    // Esto permite· en protocolos como el TCP, poder ejecutar un
    // mismo programa varias veces seguidas y enlazarlo siempre al
    // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    // quedase disponible (TIME_WAIT en el caso de TCP)
    on = 1;
    // ret =
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2065);
    sockname.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1)
    {
        perror("Error en la operación bind");
        exit(1);
    }

    /*---------------------------------------------------------------------
        Del las peticiones que vamos a aceptar sólo necesitamos el
        tamanio de su estructura, el resto de información (familia, puerto,
        ip), nos la proporcionará el método que recibe las peticiones.
    ----------------------------------------------------------------------*/
    from_len = sizeof(from);

    if (listen(sd, 1) == -1)
    {
        perror("Error en la operación de listen");
        exit(1);
    }

    printf("Servidor iniciado. Esperando conexiones...\n"); // Inicializar los conjuntos fd_set

    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd, &readfds);
    FD_SET(0, &readfds);

    // Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT, manejador);

    inicializarClientes(clientes);
    /*-----------------------------------------------------------------------
        El servidor acepta una petición
    ------------------------------------------------------------------------ */
    while (1)
    {

        // Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)

        auxfds = readfds;

        salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);

        if (salida > 0)
        {

            for (int i = 0; i < FD_SETSIZE; i++)
            {

                // Buscamos el socket por el que se ha establecido la comunicación
                if (FD_ISSET(i, &auxfds))
                {

                    if (i == sd)
                    {

                        if ((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1)
                        {
                            perror("Error aceptando peticiones");
                        }
                        else
                        {
                            if (numClientes < MAX_CLIENTS)
                            {

                                clientes[numClientes].sd = new_sd;

                                numClientes++;

                                FD_SET(new_sd, &readfds);

                                strcpy(buffer, "+Ok. Usuario conectado");

                                send(new_sd, buffer, sizeof(buffer), 0);

                                /* for(j=0; j<(numClientes-1);j++){

                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer, "Nuevo Cliente conectado en <%d>",new_sd);
                                    send(arrayClientes[j],buffer,sizeof(buffer),0);
                                } */

                                printf("Nueva conexión. IP: %s, Puerto: %d\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));
                            }
                            else
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "-Err. Demasiados clientes conectados");
                                send(new_sd, buffer, sizeof(buffer), 0);
                                close(new_sd);
                            }
                        }
                    }
                    else if (i == 0)
                    {
                        // Se ha introducido información de teclado
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);

                        // Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                        if (strcmp(buffer, "SALIR\n") == 0)
                        {

                            for (int j = 0; j < numClientes; j++)
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "+Ok. Desconexión servidor");
                                send(clientes[j].sd, buffer, sizeof(buffer), 0);
                                close(clientes[j].sd);
                                FD_CLR(clientes[j].sd, &readfds);
                            }
                            close(sd);
                            exit(-1);
                        }
                        // Mensajes que se quieran mandar a los clientes (implementar)
                    }
                    else
                    {
                        bzero(buffer, sizeof(buffer));

                        recibidos = recv(i, buffer, sizeof(buffer), 0);

                        int posicionCliente = obtenerPosicionCliente(clientes, i);

                        if (recibidos > 0)
                        {

                            printf("'%s'\n", buffer);

                            if (strcmp(buffer, "SALIR\n") == 0)
                            {

                                int posContrincante = obtenerPosicionCliente(clientes, clientes[posicionCliente].sdContrincante);

                                printf("Desconexión usuario. Usuario: %s, IP: %s, Puerto: %d\n", clientes[posicionCliente].usuario, inet_ntoa(from.sin_addr), ntohs(from.sin_port));

                                if (clientes[posicionCliente].estado == 3)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Tu oponente ha terminado la partida");
                                    send(clientes[posicionCliente].sdContrincante, buffer, sizeof(buffer), 0);

                                    clientes[posContrincante].estado = 1;
                                    clientes[posContrincante].jugador1 = false;
                                    clientes[posContrincante].turno = false;
                                    clientes[posContrincante].sdContrincante = -1;
                                    clientes[posContrincante].partida = NULL;

                                    --numPartidas;

                                    printf("Abandono partida. Usuario: %s, IP: %s, Puerto: %d\n", clientes[posicionCliente].usuario, inet_ntoa(from.sin_addr), ntohs(from.sin_port));
                                }

                                salirCliente(i, &readfds, &numClientes, clientes);
                            }
                            else if (strncmp(buffer, "USUARIO", 7) == 0)
                            {

                                // Obtenemos el nombre de usuario de la cadena: USUARIO user
                                char *usuario = strstr(buffer, " ");
                                ++usuario;

                                // El usuario existe en el fichero -> Se envía la confirmación al cliente
                                if (obtenerUsuario(usuario))
                                {

                                    // Se comprueba si el usuario ya se ha logueado previamente en otra sesion
                                    if (encontrarUsuario(clientes, usuario))
                                    {
                                        printf("Intento Re-login. Usuario: %s, IP: %s, Puerto: %d\n", usuario, inet_ntoa(from.sin_addr), ntohs(from.sin_port));

                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Desconexión servidor");
                                        send(i, buffer, sizeof(buffer), 0);

                                        salirCliente(i, &readfds, &numClientes, clientes);
                                    }
                                    else
                                    {
                                        strcpy(clientes[posicionCliente].usuario, usuario);

                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Usuario correcto");
                                    }
                                }
                                else
                                {

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usuario incorrecto");
                                }

                                send(i, buffer, sizeof(buffer), 0);
                            }
                            else if (strncmp(buffer, "PASSWORD", 8) == 0)
                            {

                                // Obtenemos la contrasenia de la cadena: PASSWORD password
                                char *contrasenia = strstr(buffer, " ");
                                ++contrasenia;

                                char *contraseniaFichero = obtenerContrasenia(clientes[posicionCliente].usuario);
                                // TODO comprobar que no es NULL

                                // La contrasenia es correcta -> Se envía la confirmación al cliente
                                if (strcmp(contraseniaFichero, contrasenia) == 0)
                                {

                                    strcpy(clientes[posicionCliente].contraseniaUsuario, contrasenia);

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Usuario validado");

                                    // Se cambia de estado: El usuario está validado en el sistema
                                    clientes[posicionCliente].estado = 1;
                                }
                                else
                                {

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Error en la validación");
                                }

                                send(i, buffer, sizeof(buffer), 0);
                            }
                            else if (strncmp(buffer, "REGISTRO", 8) == 0)
                            {

                                // Obtenemos el nombre de usuario de la cadena: REGISTRO -u user -p password

                                // Buscar la cadena "-u" en la entrada
                                char *posicionNombreUsuario = strstr(buffer, "-u");

                                // Buscar la cadena "-p" en la entrada
                                char *poisicionContrasenia = strstr(buffer, "-p");

                                char *usuario = strtok(posicionNombreUsuario + 2, " ");
                                char *contrasenia = strtok(poisicionContrasenia + 2, " ");

                                if (registrarUsuario(usuario, contrasenia))
                                {

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Registro exitoso");
                                }
                                else
                                {

                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usuario existe");
                                }

                                send(i, buffer, sizeof(buffer), 0);
                            }
                            else if (strcmp(buffer, "INICIAR-PARTIDA") == 0)
                            {

                                // Se cambia de estado al cliente indicando que está buscando partida
                                clientes[posicionCliente].estado = 2;

                                // Comprobar que el numero de partidas en el servidor no sea el máximo permitido
                                if (numPartidas < MAX_MATCHES)
                                {

                                    int sdContrincante = buscarContrincante(clientes, i);

                                    if (sdContrincante > 0)
                                    {

                                        ++numPartidas;

                                        int posContrincante = obtenerPosicionCliente(clientes, sdContrincante);

                                        clientes[posicionCliente].sdContrincante = sdContrincante;
                                        clientes[posContrincante].sdContrincante = i;

                                        // Indicamos que los dos jugadores han formado una pareja
                                        clientes[posicionCliente].estado = 3;
                                        clientes[posContrincante].estado = 3;


                                        // Formamos la partida teniendo en cuenta quien es el jugador 1 (el primero que solicitó partida)
                                        if (clientes[posicionCliente].jugador1)
                                        {
                                            formarPartida(clientes, partidas, numPartidas, numeroBarcos, tamanioBarcos, i, sdContrincante);
                                            clientes[posicionCliente].partida->sdJugador1 = i;
                                            clientes[posicionCliente].partida->sdJugador2 = sdContrincante;
                                        }
                                        else
                                        {
                                            formarPartida(clientes, partidas, numPartidas, numeroBarcos, tamanioBarcos, sdContrincante, i);
                                            clientes[posicionCliente].partida->sdJugador1 = sdContrincante;
                                            clientes[posicionCliente].partida->sdJugador2 = i;
                                        }

                                        printf("Partida formada\n");
                                        printf("\tJugador 1: Socket: %d, IP: %s, Puerto: %d\n", clientes[posicionCliente].partida->sdJugador1, inet_ntoa(from.sin_addr), ntohs(from.sin_port));
                                        printf("\tJugador 2: Socket: %d, IP: %s, Puerto: %d\n", clientes[posicionCliente].partida->sdJugador2, inet_ntoa(from.sin_addr), ntohs(from.sin_port));


                                        imprimirTablero(clientes[posicionCliente].partida->tableroJugador1);
                                        imprimirTablero(clientes[posicionCliente].partida->tableroJugador2);

                                        // Enviamos el tablero al jugador 1
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Empieza la partida. ");
                                        convertirTableroCadena(buffer, clientes[posicionCliente].partida->tableroJugador1);
                                        send(sdContrincante, buffer, sizeof(buffer), 0);

                                        // Enviamos el tablero al jugador 2
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Empieza la partida.");
                                        convertirTableroCadena(buffer, clientes[posicionCliente].partida->tableroJugador2);
                                        send(i, buffer, sizeof(buffer), 0);

                                        if (clientes[posicionCliente].turno)
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Turno de partida");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                        else
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Turno de partida");
                                            send(sdContrincante, buffer, sizeof(buffer), 0);
                                        }
                                    }
                                    else
                                    {

                                        // Indicamos que es el jugador 1 de su proxima partida al haber solicitado partida primero
                                        //  y el jugador 1 empezará primero
                                        clientes[posicionCliente].jugador1 = true;
                                        clientes[posicionCliente].turno = true;

                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Esperando jugadores");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Esperando jugadores");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                            }
                            else if (strncmp(buffer, "DISPARO", 7) == 0)
                            {

                                if (clientes[posicionCliente].turno)
                                {

                                    char columna;
                                    int fila;

                                    sscanf(buffer, "%*s %c,%d", &columna, &fila);

                                    if (validarCoordenadas(columna, fila))
                                    {

                                        int resultado;

                                        if (clientes[posicionCliente].jugador1)
                                        {
                                            resultado = disparo(columna, fila, clientes, posicionCliente, clientes[posicionCliente].partida->barcosJugador2);

                                            ++(clientes[posicionCliente].partida->disparosJugador1);
                                        }else{
                                            resultado = disparo(columna, fila, clientes, posicionCliente, clientes[posicionCliente].partida->barcosJugador1);
                                            
                                            ++(clientes[posicionCliente].partida->disparosJugador2);
                                            
                                        }

                                        imprimirTablero(clientes[posicionCliente].partida->tableroJugador1);
                                        imprimirTablero(clientes[posicionCliente].partida->tableroJugador2);

                                        bzero(buffer, sizeof(buffer));

                                        if (resultado == 0)
                                        {
                                            sprintf(buffer, "+Ok. AGUA: %c,%d", columna, fila);

                                            // Cambiamos los turnos si el disparo es sobre agua
                                            clientes[posicionCliente].turno = false;
                                            clientes[obtenerPosicionCliente(clientes, clientes[posicionCliente].sdContrincante)].turno = true;

                                            // Mensaje al que disparo
                                            send(i, buffer, sizeof(buffer), 0);

                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Disparo en: %c,%d", columna, fila);

                                            // Mensaje al contrincante
                                            send(clientes[posicionCliente].sdContrincante, buffer, sizeof(buffer), 0);

                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Turno de partida");

                                            // Mensaje de turno de partida
                                            send(clientes[posicionCliente].sdContrincante, buffer, sizeof(buffer), 0);
                                        }
                                        else if (resultado == 1)
                                        {
                                            sprintf(buffer, "+Ok. TOCADO: %c,%d", columna, fila);

                                            // Mensaje al que disparo
                                            send(i, buffer, sizeof(buffer), 0);

                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Disparo en: %c,%d", columna, fila);

                                            // Mensaje al contrincante
                                            send(clientes[posicionCliente].sdContrincante, buffer, sizeof(buffer), 0);

                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "+Ok. Turno de partida");

                                            // Mensaje de turno de partida
                                            send(i, buffer, sizeof(buffer), 0);

                                        }else if(resultado == 2){
                                            sprintf(buffer, "+Ok. HUNDIDO: %c,%d", columna, fila);

                                            struct Barco *barcos;
                                            int disparos;
                                            if (clientes[posicionCliente].jugador1)
                                            {
                                                barcos = clientes[posicionCliente].partida->barcosJugador2;
                                                disparos = clientes[posicionCliente].partida->disparosJugador1;
                                            }
                                            else
                                            {
                                                barcos = clientes[posicionCliente].partida->barcosJugador1;
                                                disparos = clientes[posicionCliente].partida->disparosJugador2;
                                            }

                                            // Comprobamos si el jugador ha ganado
                                            if (comprobarGanador(barcos)){
                                                // Enviamos el mensaje de ganador al contrincante
                                                bzero(buffer, sizeof(buffer));
                                                sprintf(buffer, "+Ok. %s ha ganado, numero de disparos %d", clientes[posicionCliente].usuario, disparos);
                                                send(clientes[posicionCliente].sdContrincante, buffer, sizeof(buffer), 0);

                                                // Informamos al cliente actual que ha ganado
                                                bzero(buffer, sizeof(buffer));
                                                sprintf(buffer, "+Ok. %s ha ganado, numero de disparos %d", clientes[posicionCliente].usuario, disparos);
                                                send(i, buffer, sizeof(buffer), 0);

                                                clientes[posicionCliente].estado = 1;
                                                clientes[posicionCliente].jugador1 = false;
                                                clientes[posicionCliente].turno = false;
                                                clientes[posicionCliente].sdContrincante = -1;
                                                clientes[posicionCliente].partida = NULL;

                                                int posContrincante = obtenerPosicionCliente(clientes, clientes[posicionCliente].sdContrincante);

                                                clientes[posContrincante].estado = 1;
                                                clientes[posContrincante].jugador1 = false;
                                                clientes[posContrincante].turno = false;
                                                clientes[posContrincante].sdContrincante = -1;
                                                clientes[posContrincante].partida = NULL;

                                                --numPartidas;
                                            
                                            //En caso contrario, la partida continua
                                            }else{
                                                // Mensaje al que disparo
                                                send(i, buffer, sizeof(buffer), 0);

                                                bzero(buffer, sizeof(buffer));
                                                sprintf(buffer, "+Ok. Disparo en: %c,%d", columna, fila);

                                                // Mensaje al contrincante
                                                send(clientes[posicionCliente].sdContrincante, buffer, sizeof(buffer), 0);

                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "+Ok. Turno de partida");

                                                // Mensaje de turno de partida
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                        }

                                        
                                    }
                                    else
                                    {
                                        bzero(buffer, sizeof(buffer));

                                        strcpy(buffer, "-Err. Coordenadas invalidas");

                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
                                else if (clientes[posicionCliente].sdContrincante != -1)
                                {
                                    bzero(buffer, sizeof(buffer));

                                    strcpy(buffer, "-Err. Debe esperar su turno");

                                    send(i, buffer, sizeof(buffer), 0);
                                }

                                // MENSAJES NO RECONOCIDOS
                            }
                            else
                            {

                                bzero(buffer, sizeof(buffer));

                                strcpy(buffer, "-Err");

                                send(i, buffer, sizeof(buffer), 0);
                            }
                        }
                        // Si el cliente introdujo ctrl+c
                        if (recibidos == 0)
                        {
                            int posContrincante = obtenerPosicionCliente(clientes, clientes[posicionCliente].sdContrincante);

                            printf("Desconexión usuario (ctrl+c). Usuario: %s, IP: %s, Puerto: %d\n", clientes[posicionCliente].usuario, inet_ntoa(from.sin_addr), ntohs(from.sin_port));

                            if (clientes[posicionCliente].estado == 3)
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "+Ok. Tu oponente ha terminado la partida");
                                send(clientes[posicionCliente].sdContrincante, buffer, sizeof(buffer), 0);

                                clientes[posContrincante].estado = 1;
                                clientes[posContrincante].jugador1 = false;
                                clientes[posContrincante].turno = false;
                                clientes[posContrincante].sdContrincante = -1;
                                clientes[posContrincante].partida = NULL;

                                --numPartidas;

                                printf("Abandono partida. Usuario: %s, IP: %s, Puerto: %d\n", clientes[posicionCliente].usuario, inet_ntoa(from.sin_addr), ntohs(from.sin_port));
                            }

                            salirCliente(i, &readfds, &numClientes, clientes);
                        }
                    }
                }
            }
        }
    }

    close(sd);
    return 0;
}

void salirCliente(int socket, fd_set *readfds, int *numClientes, struct Usuario clientes[MAX_CLIENTS])
{

    char buffer[250];
    int j;

    close(socket);
    FD_CLR(socket, readfds);

    for (int i = 0; i < (*numClientes); i++)
    {

        if (clientes[i].sd == socket)
        {
            for (int j = i; j < (*numClientes); j++)
            {
                clientes[j] = clientes[j + 1];
            }

            break;
        }
    }

    (*numClientes)--;
}

void manejador(int signum)
{
    printf("\nSe ha recibido la señal sigint\n");
    signal(SIGINT, manejador);

    // Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}
