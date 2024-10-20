#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#include "funcionesCliente.h"


int main (int argc, char *argv[])
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd;
	int estado = 0;
	struct sockaddr_in sockname;
	char buffer[250];
	socklen_t len_sockname;
    fd_set readfds, auxfds;
    int salida;
    int fin = 0;
	char nombreUsuario[MAX_INPUT_USER];
	char tablero[TAM_BOARD][TAM_BOARD];
	char tableroContrincante[TAM_BOARD][TAM_BOARD];


	if (argc != 2){
        printf("ERROR. Para ejecutar el cliente utilice: \n\t ./cliente <direccion IP del servidor>\n");
        exit(EXIT_FAILURE);
    }

	/*--------------------------------------------------
		Se abre el socket
	---------------------------------------------------*/
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    	exit (1);	
	}

   
    
	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2065);
	sockname.sin_addr.s_addr =  inet_addr(argv[1]);

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror ("Error de conexión");
		exit(1);
	}
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

    
	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do
	{
        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);
        
        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)){
            
            bzero(buffer,sizeof(buffer));
            recv(sd,buffer,sizeof(buffer),0);
            
            //MENSAJE QUE SE RECIBE
			//printf("'%s'\n", buffer);

            if(strcmp(buffer,"-Err. Demasiados clientes conectados\n") == 0){
                fin =1;
				break;

			}else if(strcmp(buffer,"+Ok. Desconexión servidor") == 0){
                fin =1;
				break;

			}else if(strcmp(buffer,"-Err. Desconexión servidor") == 0){
                fin =1;
				break;

			}else if(strcmp(buffer, "+Ok. Tu oponente ha terminado la partida") == 0){
				system("clear");
				printf("\nTu oponente ha abandonado la partida\n");

				bzero(buffer,sizeof(buffer));

				home(buffer, nombreUsuario);

				if (strcmp(buffer, "SALIR\n") == 0){
					fin = 1;
				}

				send(sd,buffer,sizeof(buffer),0);

				estado = 2;

			}else if(strncmp(buffer, "+Ok. Disparo en", 15) == 0){
				char columna;
				int fila;

				sscanf(buffer, "%*s %*s %*s %c,%d", &columna, &fila);

				int columnaMatriz = (int)(columna - 'A');
				int filaMatriz = fila - 1;

				// Si el disparo es sobre agua se sustituye en la matriz por O
				if (tablero[filaMatriz][columnaMatriz] == 'A'){
					tablero[filaMatriz][columnaMatriz] = 'O';
				// Si el disparo es sobre un barco se sustituye por una X
				}else{
					tablero[filaMatriz][columnaMatriz] = 'X';
				}
				
				imprimirTableros(tablero, tableroContrincante);

			}else if (strstr(buffer, "+Ok.") != NULL && strstr(buffer, "ha ganado") != NULL) {

				system("clear");

				printf("PARTIDA FINALIZADA\n");

				// Variables para almacenar los datos
				char usuario[MAX_INPUT_USER];
				int numeroDisparos;
				
				if (sscanf(buffer, "+Ok. %s ha ganado, numero de disparos %d", usuario, &numeroDisparos) == 2) {

					if (strcmp(usuario, nombreUsuario) == 0)
					{
						printf("\n¡Has ganado la partida con un total de %d disparos!\n", numeroDisparos);
					}
					else
					{
						printf("\nHas perdido la partida... (Numero de disparos del contrincante: %d)\n", numeroDisparos);
					}
				}
	

				bzero(buffer,sizeof(buffer));
				home(buffer, nombreUsuario);

				if (strcmp(buffer, "SALIR\n") == 0){
					fin = 1;
				}

				send(sd,buffer,sizeof(buffer),0);

				estado = 2;

			}else if(strcmp(buffer, "+Ok. Usuario conectado") == 0){
   				bzero(buffer,sizeof(buffer));

				bienvenida(buffer, nombreUsuario);

				if (strcmp(buffer, "SALIR\n") == 0){
					fin = 1;
				}

				estado = 1;

				send(sd,buffer,sizeof(buffer),0);
			}

			//INICIO DE SESION Y REGISTRO
			if (estado == 1){

				if(strcmp(buffer, "+Ok. Usuario correcto") == 0){
					bzero(buffer,sizeof(buffer));

					iniciarSesionContrasenia(buffer);

					send(sd,buffer,sizeof(buffer),0);

				}else if(strcmp(buffer, "+Ok. Usuario validado") == 0){

					estado = 2;

					bzero(buffer, sizeof(buffer));

            		system("clear");
					home(buffer, nombreUsuario);

					if (strcmp(buffer, "SALIR\n") == 0){
						fin = 1;
					}

					send(sd,buffer,sizeof(buffer),0);

				}else if (strcmp(buffer, "-Err. Usuario incorrecto") == 0){
					bzero(buffer,sizeof(buffer));

					printf("\t\nUsuario incorrecto\n");
					iniciarSesionUsuario(buffer, nombreUsuario);

					send(sd,buffer,sizeof(buffer),0);

				}else if (strcmp(buffer, "-Err. Error en la validación") == 0){
					bzero(buffer,sizeof(buffer));

					printf("\nContrasenia incorrecta\n");
					iniciarSesionContrasenia(buffer);

					send(sd,buffer,sizeof(buffer),0);

				}else if (strncmp(buffer, "+Ok.", 4) == 0){
					char *mensaje;

					sscanf(buffer, "%*s %[^\n]", mensaje);
					printf("\n%s\n", mensaje);

					bzero(buffer,sizeof(buffer));

					bienvenida(buffer, nombreUsuario);

					if (strcmp(buffer, "SALIR\n") == 0){
						fin = 1;
					}

					send(sd,buffer,sizeof(buffer),0);

				}else if (strncmp(buffer, "-Err", 4) == 0){

					char *mensaje;

					sscanf(buffer, "%*s %[^\n]", mensaje);

					printf("\n%s\n", mensaje);

					bzero(buffer,sizeof(buffer));

					bienvenida(buffer, nombreUsuario);

					if (strcmp(buffer, "SALIR\n") == 0){
						fin = 1;
					}

					send(sd,buffer,sizeof(buffer),0);
				}
			}

			// ESPERA E INICIO DE PARTIDA
			if (estado == 2){
				if(strcmp(buffer, "+Ok. Esperando jugadores") == 0){

					system("clear");

					printf("Buscando un contrincante...\n\n");

					printf("(Escriba \"SALIR\" si desea cerrar el juego)\n");
					

				}else if(strncmp(buffer, "+Ok. Empieza la partida", 23) == 0){

					estado = 3;

					convertirCadenaTablero(buffer, tablero);

					inicializarTablero(tableroContrincante);

					imprimirTableros(tablero, tableroContrincante);

				}else if (strncmp(buffer, "-Err", 4) == 0){

					char *mensaje;

					sscanf(buffer, "%*s %[^\n]", mensaje);

					printf("\n%s\n", mensaje);

					bzero(buffer,sizeof(buffer));

                	fin =1;

					strcpy(buffer, "SALIR\n");

					send(sd,buffer,sizeof(buffer),0);
				}
			}

			// TRANSCURSO DE PARTIDA: MI TURNO
			if (estado == 3){
				if(strcmp(buffer, "+Ok. Turno de partida") == 0){

					bzero(buffer,sizeof(buffer));
					
					if (!partida(tablero, tableroContrincante, buffer)){
						fin = 1;

						strcpy(buffer, "SALIR\n");
					}

					send(sd,buffer,sizeof(buffer),0);

					estado = 4;

				}else if (strcmp(buffer, "-Err. Debe esperar su turno") == 0){
					imprimirTableros(tablero, tableroContrincante);

				}else if (strncmp(buffer, "-Err", 4) == 0){

					char *mensaje;

					sscanf(buffer, "%*s %[^\n]", mensaje);

					printf("\n%s\n", mensaje);

					imprimirTableros(tablero, tableroContrincante);

				}
			}

			// TRANSCURSO DE PARTIDA: MI TURNO (RESPUESTA)
			if (estado == 4){

				if(strncmp(buffer, "+Ok. AGUA", 9) == 0){

					char columna;
					int fila;

					sscanf(buffer, "%*s %*s %c,%d", &columna, &fila);

					printf("%c, %d\n", columna, fila);


					int columnaMatriz = (int)(columna - 'A');
					int filaMatriz = fila - 1;

					tableroContrincante[filaMatriz][columnaMatriz] = 'A';

					imprimirTableros(tablero, tableroContrincante);

					estado = 3;

				}else if(strncmp(buffer, "+Ok. TOCADO", 9) == 0){

					char columna;
					int fila;

					sscanf(buffer, "%*s %*s %c,%d", &columna, &fila);

					int columnaMatriz = (int)(columna - 'A');
					int filaMatriz = fila - 1;

					tableroContrincante[filaMatriz][columnaMatriz] = 'B';

					imprimirTableros(tablero, tableroContrincante);

					estado = 3;

				}else if(strncmp(buffer, "+Ok. HUNDIDO", 12) == 0){

					char columna;
					int fila;

					sscanf(buffer, "%*s %*s %c,%d", &columna, &fila);

					int columnaMatriz = (int)(columna - 'A');
					int filaMatriz = fila - 1;

					tableroContrincante[filaMatriz][columnaMatriz] = '#';

					imprimirTableros(tablero, tableroContrincante);

					estado = 3;

				}else if (strncmp(buffer, "-Err", 4) == 0)
				{

					char *mensaje;

					sscanf(buffer, "%*s %[^\n]", mensaje);

					printf("\n%s\n", mensaje);

					if (!partida(tablero, tableroContrincante, buffer)){
						fin = 1;

						strcpy(buffer, "SALIR\n");
					}

					send(sd,buffer,sizeof(buffer),0);
				}
			}
		
		}else{
            
            //He introducido información por teclado
            if(FD_ISSET(0,&auxfds)){
                bzero(buffer,sizeof(buffer));
                
                fgets(buffer,sizeof(buffer),stdin);
                
                if(strcmp(buffer,"SALIR\n") == 0){
                    fin = 1;
                }
                
                send(sd,buffer,sizeof(buffer),0);
                
            }
            
            
        }
        
        
				
    }while(fin == 0);
		
    close(sd);

    return 0;
		
}





















