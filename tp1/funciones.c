#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define TAM 256
#define BUFFSIZE 1

#include "funciones.h"

int parser(char *entrada,char delimitador[],char *comando[]){ 
	char *ptr;
	char *copia_entrada;
	copia_entrada = strdup(entrada); //copio la entrada porq la funcion strtok rompe el array entrada
	ptr = malloc(sizeof (char) * BUFFSIZE);
	ptr = strtok(copia_entrada, delimitador);    // Primera llamada => Primer token
   	comando[0] = ptr;				//la posicion 0 apunta al primer argumento pasado
   	int i = 1;
   	while( (ptr = strtok( NULL, delimitador )) != NULL ){  // Posteriores llamadas
    	comando[i] = ptr;
    	i++;
    }
    return i;
}	
