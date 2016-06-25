#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define MAX 80
#define SA struct sockaddr
#define PUERTO "-l"
#define ENTR "-i"
#define SAL "-o"

typedef struct $
{
    char placa[10];
    int identificador;
    time_t llegada;
    int ocupado;
} vehiculo;
int NUM=0;
vehiculo PUESTOS[200];

vehiculo eliminar_vehiculo(char *placa)
{
    int i = 0;
    vehiculo carro;
    while(i == 0)
    {
        if (strcmp(placa, PUESTOS[i].placa) == 0)
        {
            strcpy(carro.placa, PUESTOS[i].placa);
            carro.identificador = PUESTOS[i].identificador;
            carro.llegada = PUESTOS[i].llegada;
            PUESTOS[i].ocupado = 0;
            i = -1;
        }
        return carro;
    }
}

void agregar_vehiculo(char *placa, time_t llegada)
{
    int i = 0;
    while (i < 200)
    {
        if (PUESTOS[i].ocupado != 1)
        {
            strcpy(PUESTOS[i].placa, placa);
            PUESTOS[i].llegada = llegada;
            PUESTOS[i].identificador = i;
            PUESTOS[i].ocupado = 1;
            printf("Carro: %s, %d, %d\n", PUESTOS[i].placa, PUESTOS[i].identificador, PUESTOS[i].ocupado);
            i = 200;
        }
        i = i + 1;
    }
}

void func(int sockfd, char *E, char *S)
{
    FILE *entrada;
	FILE *salida;
	char HOUR[30],BUFFER[1024];
	char buff[MAX];
    char buff_aux[MAX];
    char buff_aux2[MAX];
    char placa[MAX];
    char estado[MAX];
	int n,clen, horas, monto;
    double segundos;
	struct sockaddr_in cli;
    vehiculo carro;
	clen=sizeof(cli);

/*	Hora actual*/
/*	time_t now;
	struct tm *ts;
	now = time(0);
	ts = localtime(&now);
	strftime(HOUR, sizeof(HOUR), "%a %Y-%m-%d %H:%M:%S %Z", ts);
	printf("%s\n", HOUR);

	strcpy(BUFFER, HOUR);
	strcat(BUFFER," ");*/

	for(;;)
	{
		bzero(buff,MAX);
		recvfrom(sockfd,buff,sizeof(buff),0,(SA *)&cli,&clen);
        strcpy(buff_aux, buff);
        strcpy(buff_aux2, buff);
        strcpy(placa, strtok(buff_aux, " "));
        strcpy(estado, strtok(buff_aux2, strcat(buff_aux, " ")));
        printf("placa :%s:\n", placa);
        printf("estado :%s:\n", estado);
		/*    Hora actual*/
        time_t now;
        struct tm *ts;
        now = time(0);
        ts = localtime(&now);
        strftime(HOUR, sizeof(HOUR), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        printf("%s\n", HOUR);

        strcpy(BUFFER, HOUR);
        strcat(BUFFER," ");
        strcat(BUFFER,buff);

		bzero(buff,MAX);

		if(strcmp(estado, "s") == 0){

			if(NUM==0){
				sendto(sockfd,"NO PUEDES SALIR. EL ESTACIONAMIENTO ESTA VACIO :(",50,0,(SA *)&cli,clen);
			}else{
			    if((salida = fopen(S,"w")) == NULL){
			      printf("Error archivo para salidas no existe!");
			      exit(1);
			    }
				NUM--;
                carro = eliminar_vehiculo(placa);
                segundos = difftime(now, carro.llegada);
                horas = (int) segundos/3600;
                if (horas > 0)
                {
                    monto = 80 + ((horas - 1) * 30);
                }
                else {
                    monto = 80;
                }
                printf("%d\n", monto);
				sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
				printf("SALIDA: %s\n", BUFFER);
				fprintf(salida,"%s",BUFFER);
				fclose(salida);
			}
		}else{
			n=0;
			if(NUM<200){
                NUM++;
				entrada=fopen(E,"w");
			    if(entrada==NULL){
			      printf("Error archivo para entradas no existe!");
			      exit(1);
			    }
                agregar_vehiculo(placa, now);
				sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
				printf("NUEVO CLIENTE: %s\n", BUFFER);
				fprintf(entrada,"%s",BUFFER);
				fclose(entrada);
			}else{
				sendto(sockfd,"ESTACIONAMIENTO LLENO",21,0,(SA *)&cli,clen);
				printf("Estacionamiento lleno");

			}
		}bzero(BUFFER,sizeof(BUFFER));

	}
}
int main(int argc,char *argv[])
{	char E[20];
    char S[20];
	int PORT, i;
	/*Revisa los argumentos de entrada y dependiendo del prefijo
	 de cada entrada se le asigna el valor a la variable de referencia.

	 */
	if(argc!=7)
        {
                perror("Sintaxis: ./sem_svr -l <puerto> -i <Entrada.txt> -o <Salida.txt> ");
                exit(0);
        }
    else{
    	for (i=1; i<argc-1;i=i+2){
    		if(strncmp(argv[i], PUERTO, 2) == 0){
    			PORT=atoi(argv[i+1]);
    		}
    		else if(strncmp(argv[i], ENTR, 2) == 0){
    			strcpy(E,argv[i+1]);
    		}
    		else if(strncmp(argv[i], SAL, 2) == 0){
    			strcpy(S,argv[i+1]);
    		}
    		else {
                perror("Sintaxis: ./sem_cli -l <puerto> -i <Entrada.txt>  -o <Salida.txt> ");
                exit(0);
       		 }

    	}

    }

	int sockfd;
	struct sockaddr_in servaddr;
	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd==-1)
		{
		printf("socket creation failed...\n");
		exit(0);
		}
	else printf("Socket successfully created..\n");
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(PORT);
	if((bind(sockfd,(SA *)&servaddr,sizeof(servaddr)))!=0)
		{
		printf("socket bind failed...\n");
		exit(0);
		}
	else	printf("Socket successfully binded..\n");
	func(sockfd, E,S);
	close(sockfd);
}
