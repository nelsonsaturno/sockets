/*
    Archivo: sem_svr.c
    Proyecto: Estacionamiento Centro Comercial Moriah
    Integrantes:
        Maria Bracamonte    10-11147
        Nelson Saturno      09-10797
*/

#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
        #include <pthread.h>

// Constantes
#define MAX 80
#define PORT1 21147
#define PORT2 20797
#define SA struct sockaddr
#define PUERTO "-l"
#define ENTR "-i"
#define SAL "-o"

/*
    Estructura que maneja informacion de vehiculo y puesto
    en el estacionamiento
*/

typedef struct $
{
    char placa[10]; // placa del vehiculo
    int identificador; // identificador del vehiculo
    time_t llegada; // fecha y hora de llegada del vehiculo
    char llegada_str[30]; //fecha y hora de llegada directamente en string
    int ocupado; // Indicador de puesto, si esta ocupado o no
} vehiculo;

// estructura para pasar parametros por hilos
typedef struct
{
    int sockfd;
    char entrada[20];
    char salida[20];
} hilos_args;

// Variables Globales
int NUM = 0; // numero de puestos ocupados en el estacionamiento
vehiculo PUESTOS[200]; // arreglo de puestos en el estacionamiento
pthread_t tid[4]; // ID de Hilos, crearemos hasta 4 hilos

/*
    eliminar_vehiculo: funcion que desocupa un puesto del estacionamiento
                       dado una placa
*/

int eliminar_vehiculo(char *placa)
{
    int i = 0;
    while(i < 200)
    {
        if (strcmp(placa, PUESTOS[i].placa) == 0)
        {
            return i;
        }
        i = i + 1;
    }
    return -1;
}

/*
    agregar_vehiculo: funcion que ocupa un puesto del estacionamiento
                      dada una placa y hora de llegada
*/

int agregar_vehiculo(char *placa, time_t llegada, char *llegada_str)
{
    int i = 0;
    while (i < 200)
    {
        if (PUESTOS[i].ocupado == 1)
        {
            if (strcmp(PUESTOS[i].placa, placa) == 0)
            {
                return -1;
            }
        }
        i = i + 1;
    }
    i = 0;

    while(i < 200)
    {
        if (PUESTOS[i].ocupado == 0)
        {
            strcpy(PUESTOS[i].placa, placa);
            PUESTOS[i].llegada = llegada;
            PUESTOS[i].identificador = i;
            PUESTOS[i].ocupado = 1;
            strcpy(PUESTOS[i].llegada_str, llegada_str);
            return i;
        }
        i = i + 1;
    }
    return -1;
}

/*
    escuchar: funcion que espera una llamada del cliente
              y realiza las operaciones correspondientes
              segun la solicitud
*/

void escuchar(int sockfd, char *E, char *S)
{
    FILE *entrada;
	FILE *salida;
	char HOUR[30], BUFFER[1024], llegada[30];
	char buff[MAX];
    char buff_aux[MAX];
    char buff_aux2[MAX];
    char placa[MAX];
    char estado[MAX];
    char bytes[5], monto_str[20], bytes_cmp[5];
	int clen, horas, monto, recibido, repite, pos;
    double segundos;
	struct sockaddr_in cli;
    vehiculo carro;
	clen=sizeof(cli);

	for(;;)
	{
		bzero(buff,MAX);
		recibido = recvfrom(sockfd,buff,sizeof(buff),0,(SA *)&cli,&clen);
        if (recibido > 0)
        {
            sprintf(bytes, "%d", recibido);
            strcat(bytes,"$");
        }
        repite = (int) recibido;
        if (repite == 80)
        {
            repite = 1;
        }
        strcpy(buff_aux, buff);
        strcpy(buff_aux2, buff);
        strcpy(placa, strtok(buff_aux, " "));
        strcpy(estado, strtok(buff_aux2, strcat(buff_aux, " ")));

		/* Hora actual */
        time_t now;
        struct tm *ts;
        struct tm *tsalida;
        now = time(0);
        ts = localtime(&now);
        strftime(HOUR, sizeof(HOUR), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        printf("%s\n", HOUR);

        strcpy(BUFFER, HOUR);
        strcat(BUFFER," ");
        strcat(BUFFER,buff);

		bzero(buff,MAX);

        if (repite == 1)
        {
    		if(strcmp(estado, "s") == 0){

    			if(NUM==0){
                    bzero(BUFFER,1024);
                    strcpy(BUFFER, "NO PUEDES SALIR. EL ESTACIONAMIENTO ESTA VACIO :($");
                    strcat(BUFFER, bytes);
    				sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
    			}else{
    			    if((salida = fopen(S,"a")) == NULL){
    			      printf("Error archivo para salidas no existe!");
    			      exit(1);
    			    }
    				NUM--;
                    pos = eliminar_vehiculo(placa);
                    if (pos != -1)
                    {
                        segundos = difftime(now, PUESTOS[pos].llegada);
                        horas = (int) segundos/3600;
                        if (horas > 0)
                        {
                            monto = 80 + ((horas - 1) * 30);
                        }
                        else
                        {
                            monto = 80;
                        }
                        sprintf(monto_str, "%d", monto);
                        strcat(BUFFER, " Total a Pagar: ");
                        strcat(BUFFER, monto_str);
                        strcat(BUFFER, "$");
                        strcat(BUFFER, bytes);
        				sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
        				printf("SALIDA: %s\n", BUFFER);
        				fprintf(salida,"\nPlaca: %s\nID: %d\nFecha y Hora de Llegada: %s\nFecha y Hora de Salida: %s\nTotal a Pagar: Bs. %d\n",
                                PUESTOS[pos].placa, PUESTOS[pos].identificador, PUESTOS[pos].llegada_str, HOUR, monto);
        				fclose(salida);
                    }
                    else
                    {
                        bzero(BUFFER,1024);
                        strcpy(BUFFER, "Tu Carro no se encuentra en este estacionamiento$");
                        strcat(BUFFER, bytes);
                        sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
                    }
    			}
    		}
            else
            {
    			if(NUM<200){
                    NUM++;
    				entrada = fopen(E,"a");
    			    if(entrada == NULL){
    			      printf("Error archivo para entradas no existe!");
    			      exit(1);
    			    }
                    pos = agregar_vehiculo(placa, now, HOUR);
                    if (pos != -1)
                    {
                        strcat(BUFFER, "$");
                        strcat(BUFFER, bytes);
                        sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
                        printf("NUEVO CLIENTE: %s\n", BUFFER);
                        fprintf(entrada,"\nPlaca: %s\nID: %d\nFecha y Hora de Llegada: %s\n",
                                PUESTOS[pos].placa, PUESTOS[pos].identificador, HOUR);
                        fclose(entrada);
                    }
                    else
                    {
                        bzero(BUFFER,1024);
                        strcpy(BUFFER, "Tu Carro ya se encuentra en este estacionamiento$");
                        strcat(BUFFER, bytes);
                        sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
                    }
    			}
                else
                {
                    bzero(BUFFER,1024);
                    strcpy(BUFFER, "ESTACIONAMIENTO LLENO$");
                    strcat(BUFFER, bytes);
    				sendto(sockfd,BUFFER,21,0,(SA *)&cli,clen);
    				printf("Estacionamiento lleno");
    			}
    		}
        }
        else
        {
            bzero(BUFFER,1024);
            strcpy(BUFFER, "El paquete llegó incompleto o con duplicados.$");
            strcat(BUFFER, bytes);
            sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
        }
        bzero(BUFFER,sizeof(BUFFER));

	}
}

void* doSomeThing(void* args)
{
    pthread_t id = pthread_self();

    hilos_args *actual_args = args;

    if(pthread_equal(id,tid[0]))
    {
        escuchar(actual_args->sockfd, actual_args->entrada, actual_args->salida);
    }
    else if (pthread_equal(id,tid[1]))
    {
        escuchar(actual_args->sockfd, actual_args->entrada, actual_args->salida);
    }
    else if (pthread_equal(id,tid[2]))
    {
        escuchar(actual_args->sockfd, actual_args->entrada, actual_args->salida);
    }
    else if (pthread_equal(id,tid[3]))
    {
        escuchar(actual_args->sockfd, actual_args->entrada, actual_args->salida);
    }

}

int main(int argc,char *argv[])
{
    char E[20];
    char S[20];
	int PORT, i;
    int prt, fent, fsal, err; //flags para verificacion de argumentos
	/*
       Revisa los argumentos de entrada y dependiendo del prefijo
	   de cada entrada se le asigna el valor a la variable de referencia.
	 */
	if(argc!=7)
    {
        perror("Sintaxis: ./sem_svr -l <puerto> -i <Entrada.txt> -o <Salida.txt> ");
        exit(0);
    }
    else{
    	for (i = 1; i < argc - 1 ; i = i + 2){
    		if(strncmp(argv[i], PUERTO, 2) == 0){
                // Verificacion para que no se escriba dos veces el mismo flag
                if (prt == 1)
                {
                    perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
                    exit(0);
                }
    			PORT=atoi(argv[i+1]);
                if (PORT != PORT1)
                {
                    if (PORT != PORT2)
                    {
                        printf("Solo se disponen de los puertos: %d y %d\n", PORT1, PORT2);
                        exit(0);
                    }
                }
                prt = 1;
    		}
    		else if(strncmp(argv[i], ENTR, 2) == 0){
                // Verificacion para que no se escriba dos veces el mismo flag
                if (fent == 1)
                {
                    perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
                    exit(0);
                }
    			strcpy(E,argv[i+1]);
                fent = 1;
    		}
    		else if(strncmp(argv[i], SAL, 2) == 0){
                // Verificacion para que no se escriba dos veces el mismo flag
                if (fsal == 1)
                {
                    perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
                    exit(0);
                }
    			strcpy(S,argv[i+1]);
                fsal = 1;
    		}
    		else {
                perror("Sintaxis: ./sem_cli -l <puerto> -i <Entrada.txt>  -o <Salida.txt> ");
                exit(0);
       		 }
    	}
    }

	int sockfd;
	struct sockaddr_in servaddr;

	if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
	{
    	printf("Fallo en la creación del Socket...\n");
    	exit(0);
	}
	else printf("Socket creado satisfactoriamente!\n");

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(PORT);

	if((bind(sockfd, (SA *)&servaddr, sizeof(servaddr)))!=0)
	{
        close(sockfd); //cerramos el socket antes de salir
    	printf("Fallo en el enlace del Socket...\n");
    	exit(0);
	}
	else printf("Socket enlazado exitósamente!\n");

    i = 0;
    hilos_args *args = malloc(sizeof *args);
    args->sockfd = sockfd;
    strcpy(args->salida, E);
    strcpy(args->salida, S);
	while(i < 4)
    {
        err = pthread_create(&(tid[i]), NULL, &doSomeThing, args);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));

        i++;
    }

    sleep(5);
	close(sockfd);
}
