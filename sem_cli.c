/*
    Archivo: sem_cli.c
    Proyecto: Estacionamiento Centro Comercial Moriah
    Integrantes:
        Maria Bracamonte    10-11147
        Nelson Saturno      09-10797
*/
#include "header.h"
#define PUERTO "-p"
#define DIRECCION "-d"
#define MODO "-c"
#define SALIDA "s"
#define ENTRADA "e"
#define PLACA "-i"
#define MAXWAIT 4


int main(int argc,char *argv[])
{
    char buff[MAX];
    char *bytes_buff;
    char resto_buff[MAX];
    char copy_buff[MAX];
    char ID[100], IP[40], BUFFER[1024], MODE[2];
    int sockfd, len, i, j, PORT, recibido, enviados, revisar;
    struct sockaddr_in servaddr;
    struct timeval wait_time;
    struct hostent *he; /* para el nombre del host */
    int prt, host, mod, plac; //flags para verificacion de argumentos

    /*
        Revisa los argumentos de entrada y dependiendo del prefijo
        de cada entrada se le asigna el valor a la variable de referencia.
     */
    if(argc!=9)
    {
        perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
        exit(0);
    }
    else
    {
        for (i = 1; i < argc - 1; i = i + 2){
            if((strncmp(argv[i], PUERTO, 2) == 0)){
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
            else if(strncmp(argv[i], DIRECCION, 2) == 0){
                // Verificacion para que no se escriba dos veces el mismo flag
                if (host == 1)
                {
                    perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
                    exit(0);
                }
                if ((he=gethostbyname(argv[i+1])) == NULL) {
                    perror("Error obteniendo el host a travez del nombre.");
                    exit(1);
                }
                host = 1;
            }
            else if(strncmp(argv[i], MODO, 2) == 0){
                // Verificacion para que no se escriba dos veces el mismo flag
                if (mod == 1)
                {
                    perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
                    exit(0);
                }
                strcpy(MODE,argv[i+1]);
                mod = 1;
            }
            else if(strncmp(argv[i], PLACA, 2) == 0){
                // Verificacion para que no se escriba dos veces el mismo flag
                if (plac == 1)
                {
                    perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
                    exit(0);
                }
                strcpy(ID,argv[i+1]);
                plac = 1;
            }
            else
            {
                perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
                exit(0);
            }

        }
    }

    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) ==-1)
    {
        perror("Fallo en la creación del Socket...\n");
        exit(0);
    }

    wait_time.tv_sec = MAXWAIT;
    wait_time.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &wait_time, sizeof(wait_time)) == -1) {
       perror("Fallo al asignar un tiempo máximo de espera para el Socket.\n");
       exit(1);
    }

    bzero(&servaddr,sizeof(len));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr = *((struct in_addr *)he->h_addr);
    servaddr.sin_port=htons(PORT);
    len=sizeof(servaddr);

    // Se guarda en un buffer la placa del carro para enviarlo al servidor
    strcpy(buff, ID);
    strcat(buff," ");
    strcat(buff, MODE);
    for (j = 0; j < 3; j++)
    {
        printf("\n");
        if ((enviados = sendto(sockfd,buff,sizeof(buff),0,(SA *)&servaddr,len)) == -1)
        {
            perror("No se pudo enviar el mensaje al servidor.\n");
            exit(1);
        }
        printf("Bytes enviados al servidor: %d\n", enviados);
        bzero(buff,sizeof(buff));
        if ((recibido = recvfrom(sockfd,buff,sizeof(buff),0,(SA *)&servaddr,&len)) > 0)
        {
            strcpy(copy_buff, buff);
            strcpy(resto_buff, strtok(copy_buff, "$"));

            bzero(copy_buff,sizeof(copy_buff));
            strcpy(copy_buff, buff);
            bytes_buff = strtok(copy_buff, "$");
            bytes_buff = strtok(NULL, "$");
            printf("Bytes que recibió el servidor: %s\n", bytes_buff);
            if (atoi(bytes_buff) == enviados)
            {
                printf("Los datos enviados al servidor llegaron satisfactoriamente.\n");
                printf("Ticket de Estacionamiento: %s\n",resto_buff);
                break;
            }
            else if (atoi(bytes_buff) < enviados)
            {
                j = 0;
                printf("Hubo pérdida de información durante la comunicación, intentemos de nuevo.\n");
            }
            else
            {
                j = 0;
                printf("Hubo duplicados de la información enviada durante la comunicación, intentemos de nuevo.\n");
            }
            printf("Respuesta del Servidor: %s\n",resto_buff);
        }
        if (j == 2) {
            close(sockfd); // cerramos el socket
            printf("Tiempo de respuesta agotado.\n");
            exit(1);
        }
        else {
            printf("El servidor no responde, volveremos a intentar.\n");
        }
    }
    close(sockfd); // cerramos el socket
}

