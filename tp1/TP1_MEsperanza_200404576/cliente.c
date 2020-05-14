#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>

#include "commonFunctions.h"
#include "clientFunctions.h"

/* Variables globales para el uso de socket*/
int sockfd, n;
struct sockaddr_in serv_addr;
struct hostent *server;
/*******************************************/
//estructuras:
UserData userData;
UserList userList;		

int main (int argc, char **argv){
	int32_t respuesta;
	char entrada[ENTRADA];
	char *comando[5];
	
	creacion_socket_cli();
	
	if ( connect( sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr ) ) < 0 ) {
		perror( "conexion" );
		exit(1);
	}
	while(1){
		envioCredenciales();

		printf("Acceso Válido, bienvenido al servidor.\n");
		printf("Para más información ingrese el comando --help.\n\n");
		do {
			printf("%s@prompt: ",userData.user);
			memset(entrada,0,100);			//vacio el vector donde se almacena la entrada por teclado
			fgets (entrada, 100, stdin);
			int cantComandos = parser(entrada," \n\t",comando);
			if ((strcmp(comando[0],"exit") == 0) && (cantComandos == 1) ){
				userData.peticion = salida;
				if(send(sockfd,&userData,sizeof(userData),0) == ERROR){
					perror("Error al enviar la información al servidor:");
				}
			}
			else if ((strcmp(comando[0],"user") == 0) && (strcmp(comando[1],"ls") == 0) && (cantComandos == 2)){
				userData.peticion = userLs;
				if(send(sockfd,&userData,sizeof(userData),0) == ERROR){
					perror("Error al enviar la información al servidor:");
				}
				int i = 1;
				do{
					memset(&userList,0,sizeof(userList));
					n = recv(sockfd, &userList, sizeof(userList), 0);
  					if ( n < 0 ) {
    					perror( "lectura de socket" );
    					exit( 1 );
  					}
  					i++;
  					printf("usuario: %s\nestado: %s\nultima Conexion: %s\n",userList.usuario,userList.estado,userList.ultimaConex);
				}while(i<=userList.cantidadUsuarios);
			}
	
			else if ((strcmp(comando[0],"user") == 0) && (strcmp(comando[1],"passwd") == 0) && (cantComandos == 3)){
				printf("Clave actualizada.\n");
				userData.peticion = newPass;
				strcpy(userData.pass, comando[2]);
				if(send(sockfd,&userData,sizeof(userData),0) == ERROR){
					perror("Error al enviar la información al servidor:");
				}
			}

			else if ((strcmp(comando[0],"file") == 0) && (strcmp(comando[1],"ls") == 0) && (cantComandos == 2)){
				userData.peticion = fileLs;
				if(send(sockfd,&userData,sizeof(userData),0) == ERROR){
					perror("Error al enviar la información al servidor:");
				}
			}

			else if ((strcmp(comando[0],"file") == 0) && (strcmp(comando[1],"down") == 0) && (cantComandos == 3)){
				userData.peticion = fileDown;
				if(send(sockfd,&userData,sizeof(userData),0) == ERROR){
					perror("Error al enviar la información al servidor:");
				}
			}
	
			else if ((strcmp(comando[0],"--help") == 0) && (cantComandos == 1)){ 
				help();
			}
	
			else{
				printf("Comando inexistente, ingrese --help para mas información\n");
			}
		}while (userData.peticion != salida);	
	}
	return 0;
}

void creacion_socket_cli(){	
	sockfd = socket( AF_INET, SOCK_STREAM, 0 );		//creacion del socket.
	if ( sockfd < 0 ) {
		perror( "ERROR apertura de socket" );
		exit( 1 );
	}

	server = gethostbyname("localhost");
	if (server == NULL) {
		fprintf( stderr,"Error, no existe el host\n" );
		exit( 0 );
	}

	memset( (char *) &serv_addr, '0', sizeof(serv_addr) );	//limpia la estructura
	serv_addr.sin_family = AF_INET;
	bcopy( (char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length );
	serv_addr.sin_port = htons(PUERTO);

	
}

void envioCredenciales(){
	int32_t acceso;
	do{
		ingresoCredenciales();

		if(send(sockfd,&userData,sizeof(userData),0) == ERROR){
			perror("Error al enviar la información al servidor:");
		}
		n = recv(sockfd, &acceso, sizeof(int32_t), 0);
  		if ( n < 0 ) {
    		perror( "lectura de socket" );
    		exit( 1 );
  		}
  		if (acceso == usuarioInexistente){
			printf("el nombre de usuario ingresado no existe, por favor ingrese un usuario válido.\n");
		}
		if (acceso == passIncorrecta){
			printf("Contraseña incorrecta, ingrese usuario y contraseña nuevamente. Recuerde que luego de 3 intentos fallidos, el usuario será bloqueado.\n");	
		}
		if (acceso == usuarioBloqueado){
			printf("El usuario ha sido bloqueado.\n");
			//exit(1);
		}
	}while (acceso != AccesoOK);	
}

void ingresoCredenciales(){
	char entrada[ENTRADA];
	memset(&userData,0,sizeof(userData)); 
	userData.peticion = validacion;
	printf("Usuario: ");
	fgets (entrada, ENTRADA, stdin); 				//entrada por teclado
	strcpy(userData.user, entrada);
	userData.user[strlen(userData.user)-1]='\0'; 	//elimino el salto de linea al final
	memset(entrada, 0, ENTRADA);					//vacio el vector donde se almacena la entrada por teclado
	printf("Contraseña: ");
	fgets (entrada, ENTRADA, stdin); 				//entrada por teclado
	strcpy(userData.pass, entrada);
	userData.pass[strlen(userData.pass)-1]='\0';	//elimino el salto de linea al final
}

void help(){
	printf("funcion aun no desarrollada, lamentamos los inconvenientes.\n");
}

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