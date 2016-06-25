#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX 80
//#define PORT 43454
#define SA struct sockaddr
#define PUERTO "-p"
#define DIRECCION "-d"
#define MODO "-c"
#define SALIDA "s"
#define ENTRADA "e"
#define NOMBRE "-i"
int main(int argc,char *argv[])
{
char buff[MAX], ID[100],IP[40],BUFFER[1024],MODE[2];
int sockfd,len,n,i,PORT;
struct sockaddr_in servaddr;

sockfd=socket(AF_INET,SOCK_DGRAM,0);

if(sockfd==-1)
{
printf("socket creation failed...\n");
exit(0);
}
else printf("Socket successfully created..\n");

/*Revisa los argumentos de entrada y dependiendo del prefijo
	 de cada entrada se le asigna el valor a la variable de referencia.

	 */
	if(argc!=9)
        {
                perror("Sintaxis: ./Cliente -d <IP> -p <Puerto> -c <entrada/salida> -i <ID> ");
                exit(0);
        }
    else{
    	for (i=1; i<argc-1;i=i+2){
    		if(strncmp(argv[i], PUERTO, 2) == 0){
    			PORT=atoi(argv[i+1]);

    		}
    		else if(strncmp(argv[i], DIRECCION, 2) == 0){
    			strcpy(IP,argv[i+1]);


    		}
    		else if(strncmp(argv[i], MODO, 2) == 0){
    			strcpy(MODE,argv[i+1]);
    		}
    		else if(strncmp(argv[i], NOMBRE, 2) == 0){
    			strcpy(ID,argv[i+1]);
    		}

    	}

    }

	bzero(&servaddr,sizeof(len));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(IP);
	servaddr.sin_port=htons(PORT);
	len=sizeof(servaddr);
	/*Se guarda en un buffer el ID del movil para enviarlo al servidor*/
	strcpy(buff, ID);
	strcat(buff," ");
	strcat(buff, MODE);
	printf("%s\n",buff);
	for(;;)
	{
		printf("\n");
		sendto(sockfd,buff,sizeof(buff),0,(SA *)&servaddr,len);
		bzero(buff,sizeof(buff));
		recvfrom(sockfd,buff,sizeof(buff),0,(SA *)&servaddr,&len);
		printf("Del Servidor : %s\n",buff);
		break;
	}
	close(sockfd);
}
