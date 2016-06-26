/*
    Archivo: sem_svr.c
    Proyecto: Estacionamiento Centro Comercial Moriah
    Integrantes:
        Maria Bracamonte    10-11147
        Nelson Saturno      09-10797
*/

#define PUERTO "-l"
#define ENTR "-i"
#define SAL "-o"
#include "header.h"

// Variables Globales
int NUM = 0;                // numero de puestos ocupados en el estacionamiento
vehiculo PUESTOS[200];      // arreglo de puestos en el estacionamiento

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
{   //verifica si se encuentra el carro en el estacionamiento.
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
    //Si no se encuentra, se agrega el carro
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
            strcat(bytes," ");
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
        strcpy(BUFFER, HOUR);
        strcat(BUFFER," Placa: ");
        strcat(BUFFER,placa);
		bzero(buff,MAX);

        if (repite == 1)
        {
    		if(strcmp(estado, "s") == 0){
                /*Si quiere salir un carro pero no hay nadie en el estacionamiento*/
    			if(NUM==0){
                    bzero(BUFFER,sizeof(BUFFER));
                    strcpy(BUFFER, "NO PUEDES SALIR. EL ESTACIONAMIENTO ESTA VACIO :($");
                    strcat(BUFFER, bytes);
    				sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
    			}else{
                    /*Si hay carros en el estacionamiento, verifica que la placa este
                    dentro de los carros ingresados, de ser asi lo elimina del estacionamiento
                    y calcula el precio a cobrar.De lo contrario no hace la salida, pues no esta 
                    dentro del estacionamiento*/
    			    if((salida = fopen(S,"a")) == NULL){
    			      printf("Error en abrir el archivo de salida");
    			      exit(1);
    			    }
                    pos = eliminar_vehiculo(placa);
                    if (pos != -1)
                    {   
                        NUM--;
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
                        printf("SALIDA: %s\n", BUFFER);
                        strcat(BUFFER, "$");
                        strcat(BUFFER, bytes);
        				sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
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
            {   /*Si quiere entrar debe de haber menos de 200 carros, de ser asi 
                verifica si esa placa ingresada esta dentro del estacionamiento, de lo contrario
                lo ingresa ocupando un lugar en el mismo. */

    			if(NUM<200){
    				entrada = fopen(E,"a");
    			    if(entrada == NULL){
    			      printf("No se pudo abrir el archivo de entrada");
    			      exit(1);
    			    }
                    pos = agregar_vehiculo(placa, now, HOUR);
                    if (pos != -1)
                    {   NUM++;
                        printf("ENTRADA: %s\n", BUFFER);
                        strcat(BUFFER, "$");
                        strcat(BUFFER, bytes);
                        sendto(sockfd,BUFFER,sizeof(BUFFER),0,(SA *)&cli,clen);
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

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                        FUNCION PRINCIPAL                              */
/*                                                                       */
/*-----------------------------------------------------------------------*/
int main(int argc,char *argv[])
{
    char E[20];
    char S[20];
	int PORT, i;
    int prt, fent, fsal; //flags para verificacion de argumentos
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
	else printf("Conectado\n");
        //printf("Socket enlazado exitósamente!\n");

	escuchar(sockfd, E,S);
	close(sockfd);
}
