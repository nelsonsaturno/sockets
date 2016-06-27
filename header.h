/*
    Archivo: header.h
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

// Constantes
#define MAX 80
#define PORT1 21147
#define PORT2 20797
#define SA struct sockaddr


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
}vehiculo;
