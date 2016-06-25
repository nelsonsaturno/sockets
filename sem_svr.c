#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX 80
#define SA struct sockaddr
#define PUERTO "-l"
#define ENTR "-i"
#define SAL "-o"

typedef struct $
{
    char placa[10]
    char identificador[20]
    time_t datetime
} vehiculo;
int NUM=0;
int PUESTOS[200]

void func(int sockfd, char *E, char *S)
{
    FILE *entrada;
	FILE *salida;
	char HOUR[30],BUFFER[1024];
	char buff[MAX];
	int n,clen;
	struct sockaddr_in cli;
	clen=sizeof(cli);
/*	Hora actual*/
	time_t now;
	struct tm *ts;
	now = time(0);
	ts = localtime(&now);
	strftime(HOUR, sizeof(HOUR), "%a %Y-%m-%d %H:%M:%S %Z", ts);
	printf("%s\n", HOUR);

	strcpy(BUFFER, HOUR);
	strcat(BUFFER," ");

	for(;;)
	{
		bzero(buff,MAX);
		recvfrom(sockfd,buff,sizeof(buff),0,(SA *)&cli,&clen);
		/*    Hora actual*/
         time_t   now;
         struct tm *ts;
         now = time(0);
         ts = localtime(&now);
         strftime(HOUR, sizeof(HOUR), "%a %Y-%m-%d %H:%M:%S %Z", ts);
         printf("%s\n", HOUR);

        strcpy(BUFFER, HOUR);
        strcat(BUFFER," ");
        strcat(BUFFER,buff);
		bzero(buff,MAX);
		char *salir = strstr(BUFFER, "salida");
		if(salir!=NULL){

			if(NUM==0){
				sendto(sockfd,"NO PUEDES SALIR. EL ESTACIONAMIENTO ESTA VACIO :(",50,0,(SA *)&cli,clen);
			}else{
				salida=fopen(S,"w");
				    if(salida=NULL){
				      printf("Error archivo para salidas no existe!");
				      exit(1);
				    }
				NUM--;
				sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
				printf("SALIDA: %s\n", BUFFER);
				fprintf(salida,"%s",BUFFER);
				fclose(salida);
			}
		}else{
			n=0;
			if(NUM<201){
                NUM++;
				entrada=fopen(E,"w");
			    if(entrada==NULL){
			      printf("Error archivo para entradas no existe!");
			      exit(1);
			    }
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
{	char E[20],S[20];
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
