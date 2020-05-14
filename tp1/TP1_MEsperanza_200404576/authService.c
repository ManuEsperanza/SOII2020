#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include "sqlite3.h"
#include <stdint.h>

#include "commonFunctions.h"
#include "authFunctions.h"

//definicion de estructuras
UserData userData;
UserList userList;

/* files descritor  para la comunicacion fifo con el primary server*/
int fdFifoAuthPri;
// FIFO file path
char * fifoPriAuth = "/tmp/fifopriauth";
int32_t acceso;
int existeUsuario;
int intentos = 0;
signed int cantUsers;
char  ultimoUsuario[ENTRADA];

int main (int argc, char **argv){
	int a,num;
	a = mkfifo(fifoPriAuth, READNWRITE);		//creo el fifo para recibir peticiones del primary server.
	creacionBdd();
	while(1){
		validacionUser();
		actualizaFecha();
		do{
			memset(&userData,0,sizeof(userData));
			lecturaFifo();			
			if ((num = read(fdFifoAuthPri,&userData,sizeof(userData))) == -1){	//recibo la estructura con la petición ingresada por el usuario
				perror("read");
			}
			close(fdFifoAuthPri);
			consulta(userData.peticion);
		}while(userData.peticion != salida);
	}
	return 0;
}	

void creacionBdd(){
	sqlite3 *db;
	char *error = 0;
	int res;
	char *tabla, *info;

	/* Open database */
	res = sqlite3_open("bdd.db", &db);
	if (res){
		fprintf(stderr, "No puedo abrir la base de datos: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	/* Create SQL statement */
	tabla = "CREATE TABLE usersInfo ("
	"`usuario` TEXT, "
	"`pass` TEXT, "
	"`estado` TEXT, "
	"`ultimaConex` DATETIME)";

	/* Execute SQL statement */
	res = sqlite3_exec(db, tabla, NULL, 0, &error);	//intento crear la tabla que contiene los campos de usuario, contraseña y estado.
	if (res == SQLITE_OK){		//si la tabla se pudo crear, es porque no existía entonces lleno la tabla con la info de los usuarios.
		info = "INSERT INTO usersInfo VALUES ('manu','miclave','no bloqueado',DATETIME(STRFTIME('%s','now')-10800, 'unixepoch'));"
				"INSERT INTO usersInfo VALUES ('admin','admin','no bloqueado',DATETIME(STRFTIME('%s','now')-10800, 'unixepoch'));";
		res = sqlite3_exec(db, info, NULL, 0, &error);
		if (res != SQLITE_OK){
			fprintf(stderr, "Error: %s\n", error);
			sqlite3_free(error);
		}
	}
	sqlite3_close(db);	
}

void validacionUser(){
	int num;
	int32_t opcion;
	
	do{
		existeUsuario = 0;
		memset(&userData,0,sizeof(userData));
		lecturaFifo();			
		if ((num = read(fdFifoAuthPri,&userData,sizeof(userData))) == -1){	//recibo la estructura con el user/pass ingresado por el usuario
			perror("read");
		}
		close(fdFifoAuthPri);
		if (intentos == 0){	//quiere decir que es la primera vez q se intenta loguear el usuario.
			strcpy(ultimoUsuario,userData.user);	//guardo el ultimo usuario que intenta loguearse para contarle los intentos de acceso fallido.
		}
		else{
			if (strcmp(ultimoUsuario,userData.user) != 0){	//si el usuario que se intenta loguear es otro, entonces que resetee la cuenta de intentos.
				intentos = 0;
			} 
		
		}
		consulta(userData.peticion);
		if (acceso == usuarioBloqueado){
			bloquearUsuario();
		}
		if (existeUsuario == 0){		//si no existe el usuario
			acceso = usuarioInexistente;
		}
		escrituraFifo();
		if ((num = write(fdFifoAuthPri,&acceso,sizeof(int32_t))) == -1){
      		perror("write");
    	}	
		close(fdFifoAuthPri);
	}while(acceso != AccesoOK);
}	

void consulta(int32_t peticion){
	sqlite3 *db;
	char *error = 0;
	int res;
	char *sql,*sql2,*consulta;
	char *cierre = "';";
	int num;

	/* Open database */
	res = sqlite3_open("bdd.db", &db);
	if (res){
		fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
  		exit(0);
	}
	switch(peticion){ 
		case validacion:
			sql = "SELECT * FROM usersInfo WHERE usuario = '";
			consulta = malloc(strlen(sql) + strlen(userData.user) + strlen(cierre)); 
			strcpy(consulta, sql); // concateno el select que quiero hacer en la tabla mas el nombre de usuario y mas el (';') final para completar la consulta a la bdd.
			strcat(consulta, userData.user);
			strcat(consulta,cierre);
			/* Execute SQL statement */
			res = sqlite3_exec(db, consulta, selectValid, 0, &error);
			if (res != SQLITE_OK){
				fprintf(stderr, "Error: %s\n", error);
				sqlite3_free(error);
			}
			sqlite3_close(db);
			break;
		case userLs:
		 	cuentaUsuarios();
		 	userList.cantidadUsuarios = cantUsers;  	// un miembro de la estructura que le envio al primary server, es la cant de usuarios de los cuales le voy a enviar la informacion. es para que el server sepa cuantos tiene que recibir
		 	//printf("cantidad de usuarios: %d \n",cantUsers);
		 	consulta = "SELECT usuario,estado,ultimaConex FROM usersInfo";		//consulta que lista los usuarios, si estan bloqueados y ultima conexión.
			res = sqlite3_exec(db, consulta, selectLs, 0, &error);
			if (res != SQLITE_OK){
				fprintf(stderr, "Error: %s\n", error);
				sqlite3_free(error);
			}
			sqlite3_close(db);
			break;	
		case newPass:			
			sql = "UPDATE usersInfo SET pass = '";
			sql2 = "' WHERE usuario = '";
			consulta = malloc(strlen(sql) + strlen(userData.pass) + strlen(sql2) + strlen(userData.user) + strlen(cierre)); 
			strcpy(consulta, sql); // concateno el select que quiero hacer en la tabla mas el nombre de usuario y mas el (';') final para completar la consulta a la bdd.
			strcat(consulta, userData.pass);
			strcat(consulta, sql2);
			strcat(consulta, userData.user);	
			strcat(consulta,cierre);
			res = sqlite3_exec(db, consulta, selectNewPass, 0, &error);
			if (res != SQLITE_OK){
				fprintf(stderr, "Error: %s\n", error);
				sqlite3_free(error);
			}
			sqlite3_close(db);
			break;

		case salida:
			//acceso = 0;
			break;
		default:	
			break;	
	} 
}

void bloquearUsuario(){
	char *sql,*cierre,*consulta;
	sqlite3 *db;
	char *error = 0;
	int res;
	int num;

	/* Open database */
	res = sqlite3_open("bdd.db", &db);
	if (res){
		fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
  		exit(0);
	}
	sql = "UPDATE usersInfo SET estado = 'bloqueado' WHERE usuario = '";
	cierre = "';";
	consulta = malloc(strlen(sql) + strlen(userData.user)+strlen(cierre)); // +2 para el (';)final
	strcpy(consulta, sql); // concateno el select que quiero hacer en la tabla mas el nombre de usuario y mas el (';') final para completar la consulta a la bdd.
	strcat(consulta, userData.user);
	strcat(consulta,cierre);

	res = sqlite3_exec(db, consulta, selectBloq, 0, &error);
	if (res != SQLITE_OK){
		fprintf(stderr, "Error: %s\n", error);
		sqlite3_free(error);
	}
	sqlite3_close(db);
}

void actualizaFecha(){
	char *sql,*cierre,*consulta;
	sqlite3 *db;
	char *error = 0;
	int res;
	int num;
	/* Open database */
	res = sqlite3_open("bdd.db", &db);
	if (res){
		fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
  		exit(0);
	}
	sql = "UPDATE usersInfo SET ultimaConex = DATETIME(STRFTIME('%s','now')-10800,'unixepoch') WHERE usuario = '";
	cierre = "';";
	consulta = malloc(strlen(sql) + strlen(userData.user) + strlen(cierre)); // +2 para el (';)final
	strcpy(consulta, sql); // concateno el select que quiero hacer en la tabla mas el nombre de usuario y mas el (';') final para completar la consulta a la bdd.
	strcat(consulta, userData.user);
	strcat(consulta,cierre);

	res = sqlite3_exec(db, consulta, selectFecha, 0, &error);
	if (res != SQLITE_OK){
		fprintf(stderr, "Error: %s\n", error);
		sqlite3_free(error);
	}
	sqlite3_close(db);	

}

void cuentaUsuarios(){
	char *consulta;
	sqlite3 *db;
	char *error = 0;
	int res;
	int num;
	/* Open database */
	res = sqlite3_open("bdd.db", &db);
	if (res){
		fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
  		exit(0);
	}
	consulta = "select count(*) from usersInfo;";
	res = sqlite3_exec(db, consulta, selectCuenta, 0, &error);
	if (res != SQLITE_OK){
		fprintf(stderr, "Error: %s\n", error);
		sqlite3_free(error);
	}
	sqlite3_close(db);	
}

static int selectCuenta(void *nada, int argc, char **valor, char **campo){
	//strcpy(cantUsers,valor[0]);
	cantUsers = atoi(valor[0]);
}	

static int selectValid(void *nada, int argc, char **valor, char **campo){
	int i;
	int contrasenia = 0,estado = 0;

	existeUsuario = 1;
	for(i=0; i<argc; i++){
		//printf("%s => %s\n", campo[i], valor[i]);
		if (strcmp(campo[i],"pass") == 0) {	//busco el campo donde tiene el valor de la clave del usuario.
			if ((strcmp(valor[i],userData.pass) == 0)){		//comparo la contraseña ingresada con la contraseña guardada en la bdd.
				contrasenia = 1;
			}
			else{
				acceso = passIncorrecta;
				intentos++;
				if (intentos == 3){
					acceso = usuarioBloqueado;
				}
			}
		}		
		if (strcmp(campo[i],"estado") == 0) {	//busco el campo donde tiene el valor de la clave del usuario.
			if (strcmp(valor[i],"no bloqueado") == 0){		//corroboro que el usuario no esté bloqueado.
				estado = 1;
			}
			else{
				acceso = usuarioBloqueado;	
			}
		}	
		
		if (contrasenia == 1 && estado == 1){
			acceso = AccesoOK;
		}			
	}
	return 0;
}

static int selectLs(void *nada, int argc, char **valor, char **campo){
	int num;
	memset(&userList,0,sizeof(userList));
	userList.cantidadUsuarios = cantUsers;
	strcpy(userList.usuario,valor[0]);
	strcpy(userList.estado,valor[1]);
	strcpy(userList.ultimaConex,valor[2]);		
	int i = 0;
	for (i;i<10000;i++){
		//hago este for para hacer tiempo estre las diversas escrituras en el fifo, sino cada tanto se rompe. (sí, lo se, es una negrada pero es una solución transitoria :D)
	}
	escrituraFifo();
	if ((num = write(fdFifoAuthPri,&userList,sizeof(userList))) == -1){
    perror("write");
   	}	
	close(fdFifoAuthPri);
	return 0;
}

static int selectNewPass(void *nada, int argc, char **valor, char **campo){
	//cuando hago una actualizacion de un campo, no necesito que me devuelva ninguna información
	//aun asi, la funcion sqlite3_exec necesita de una funcion de callback para funcionar.
	return 0;
}

static int selectFecha(void *nada, int argc, char **valor, char **campo){
	//cuando hago una actualizacion de un campo, no necesito que me devuelva ninguna información
	//aun asi, la funcion sqlite3_exec necesita de una funcion de callback para funcionar.
	return 0;
}	

void escrituraFifo(){
	fdFifoAuthPri = open(fifoPriAuth, O_WRONLY); //O_RDONLY, O_WRONLY, O_RDWR.
	if(fdFifoAuthPri == -1) {
    	perror("Error: El fichero fdFifoAuthPri o directorio no existe");
    	exit( 1 );
 	}
}

void lecturaFifo(){
	fdFifoAuthPri = open(fifoPriAuth, O_RDONLY); //O_RDONLY, O_WRONLY, O_RDWR.
	if(fdFifoAuthPri == -1) {
    	perror("Error: El fichero fdFifoAuthPri o directorio no existe");
    	exit( 1 );
  	}
}

static int selectBloq(void *nada, int argc, char **valor, char **campo){	
	//cuando hago una actualizacion de un campo, no necesito que me devuelva ninguna información
	//aun asi, la funcion sqlite3_exec necesita de una funcion de callback para funcionar.
	return 0;
}
