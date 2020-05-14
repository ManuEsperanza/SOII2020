#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 


#include "commonFunctions.h"
#include "serverFunctions.h"

//definicion de estructuras
UserData userData;
UserList userList;

/* Variables globales para la creación del socket TCP*/
int sockfd, newsockfd, servlen, clilen, n, buf, pid;
struct sockaddr_in serv_addr, cli_addr;
/*******************************************/

/* files descritors globales para la comunicacion de fifos*/
int fdFifoPriAuth;
int fdFifoPriFile; 
/*******************************************/
// FIFO file path 
char * fifoPriAuth = "/tmp/fifopriauth"; 
char * fifoPriFile = "/tmp/fifoprifile";

int32_t acceso;
char *listaUsers;

int main (int argc, char **argv){
    creacionFifos();
    creacion_socket_serv();
  
    while(1){  
        newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
        if ( newsockfd < 0 ) {
            perror( "accept" );
            exit( 1 );
        }
        do{
            //close( sockfd );
            recepcionuserData();
            envioAuthService(userData.peticion);
            if(send(newsockfd,&acceso,sizeof(int32_t),0) == ERROR){
                perror("Error al enviar usuario y contraseña:");
            }  
        }while(acceso != AccesoOK);
    
        do{
            memset(&userData,0,sizeof(userData));    
            n = recv(newsockfd, &userData, sizeof(userData), 0);
            if ( n < 0 ) {
                perror( "lectura de socket" );
                exit( 1 );
            }
    
            if ((userData.peticion == validacion) || (userData.peticion == newPass) || (userData.peticion == userLs) || (userData.peticion == salida)){
                envioAuthService(userData.peticion); 
            }
            else{

            }

        }while(userData.peticion != salida);
    }
    remove(fifoPriAuth);  //borro los fifo files.
    remove(fifoPriFile);
    return 0;
}

void creacion_socket_serv(){
    sockfd = socket( AF_INET, SOCK_STREAM, 0);  //creacion del socket.
    if ( sockfd < 0 ) { 
        perror( " apertura de socket ");
        exit( 1 );
    }
    int true = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int));
    memset( (char *) &serv_addr, 0, sizeof(serv_addr) );    //limpia la estructura
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PUERTO);
  
    if ( bind(sockfd, ( struct sockaddr *) &serv_addr, sizeof( serv_addr ) ) < 0 ) {  //con bind asignamos una dirección al socket sockfd
        perror( "ligadura" );                         
        exit( 1 );
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
}

void creacionFifos(){
    int a,b;
    // Creating the named file(FIFO)  mkfifo(<pathname>, <permission>) 
    a = mkfifo(fifoPriAuth, READNWRITE);
    if(a == -1) {
        perror("Error en la creacion del fifo entre pri y auth server");
        exit( 1 );
    }
    b = mkfifo(fifoPriFile, READNWRITE);  
    if(b == -1) {
        perror("Error en la creacion del fifo entre pri y file server");
        exit( 1 );
    }
}

void recepcionuserData(){
    memset(&userData,0,sizeof(userData));    
    n = recv(newsockfd, &userData, sizeof(userData), 0);
    if ( n < 0 ) {
        perror( "lectura de socket" );
        exit( 1 );
    }
    /*pid = fork(); 
    if ( pid < 0 ) {
        perror( "fork" );
        exit( 1 );
    }

    if ( pid == 0 ) {  // Proceso hijo
        close( sockfd );
        memset(&userData,0,sizeof(userData));    
        n = recv(newsockfd, &userData, sizeof(userData), 0);
        if ( n < 0 ) {
            perror( "lectura de socket" );
            exit( 1 );
        }       
    }*/
}

void envioAuthService(int32_t peticion){
    int num;
    switch(peticion){
        case validacion:    //si la peticion es validacion de user/pass, entonces envio la estructura con dichos datos.
            escrituraFifo();
            if ((num = write(fdFifoPriAuth,&userData,sizeof(userData))) == -1){
                perror("write");
            }
            close(fdFifoPriAuth);
            //--------------------------------------------------------
            lecturaFifo();
            if ((num = read(fdFifoPriAuth,&acceso,sizeof(int32_t))) == -1){  //recibo la respuesta del auth server.
                perror("read");
            }
            close(fdFifoPriAuth);
            break;
        case newPass:     //si la peticion es cambiar la clave del usuario, entonces envio la clave nueva.
            escrituraFifo();
            if ((num = write(fdFifoPriAuth,&userData,sizeof(userData))) == -1){  
                perror("write");
            }
            close(fdFifoPriAuth);
            break;
        case userLs:
            escrituraFifo();
            if ((num = write(fdFifoPriAuth,&userData,sizeof(userData))) == -1){
                perror("write");
            }
            close(fdFifoPriAuth);
            //-------------------------------------------------------- 
            int i = 1;
            do{
                printf("i = %d\n",i );
                lecturaFifo();
                memset(&userList,0,sizeof(userList));
                if ((num = read(fdFifoPriAuth,&userList,sizeof(userList))) == -1){  //recibo la respuesta del auth server.
                    perror("read");
                }
                close(fdFifoPriAuth);
                if(send(newsockfd,&userList,sizeof(userList),0) == ERROR){          //envio al cliente la info del usuario.
                    perror("Error al enviar usuario y contraseña:");
                }
                i++;
            }while(i<=userList.cantidadUsuarios);    //hago un while, que reciba los datos de los usuarios uno a uno.
            break;
        case salida:
            escrituraFifo();
            if ((num = write(fdFifoPriAuth,&userData,sizeof(userData))) == -1){
                perror("write");
            }
            close(fdFifoPriAuth);  
        default:
            break;  
    }     
}


void escrituraFifo(){
    fdFifoPriAuth = open(fifoPriAuth, O_WRONLY); //O_RDONLY, O_WRONLY, O_RDWR.
    if(fdFifoPriAuth == -1) {
        perror("Error: El fichero fdFifoAuthPri o directorio no existe");
        exit( 1 );
    }
}

void lecturaFifo(){
    fdFifoPriAuth = open(fifoPriAuth, O_RDONLY); //O_RDONLY, O_WRONLY, O_RDWR.
    if(fdFifoPriAuth == -1) {
        perror("Error: El fichero fdFifoAuthPri o directorio no existe");
        exit( 1 );
    }
}

