/**
  * @file funciones.h
  * @version 1.0
  * @date 21/03/2020
  * @author Esperanza Manuel
  * @brief prototipos de las funciones, estructuras y defines en general. 
  */

#define PUERTO 1244
#define ENTRADA 20
#define LINEA 1024
#define STDOUT_FID 1
#define	ERROR	-1
#define READNWRITE 0666

/**
 * @struct userPass
 * @brief contiene el nombre de usuario y contraseña ingresado desde el cliente.
 */
typedef struct {
	int32_t peticion;
	char user[ENTRADA];
	char pass[ENTRADA];
}UserData;

typedef struct {
	int32_t cantidadUsuarios;
	char usuario[ENTRADA];
	char estado[ENTRADA];
	char ultimaConex[ENTRADA];
}UserList;

/** enum usersInfo
*   hace referencia a los campos de la base de dato de los usuarios.
*/
enum usersInfo {
  usuario,
  contrasenia,
  bloqueado
};

enum opcionAuthServ {
	validacion,
	userLs,
	newPass,
	fileLs,
	fileDown,
	salida,
	prueba
};

enum accesType{
	usuarioInexistente,
	passIncorrecta,
	usuarioBloqueado,
	AccesoOK
};

/**
* @brief creacion_socket_cli Crea y configura el socket del lado del cliente.
*/
void creacion_socket_cli(); //cliente

/**
* @brief creacion_socket_serv Crea y configura el socket del lado del servidor.
*/
void creacion_socket_serv();	//server

/**
* @brief creacionFifos crea las fifo necesarias para la comunicacion entre el primary server y los procesos authService y fileService.
*/
void creacionFifos();	//server

/**
* @brief ingresoCredenciales Toma los datos ingresados por teclado correspondientes a usuario y contraseña y los almacena en la estructura UserPass
* 		 para enviarla al primary server.
*/
void ingresoCredenciales();	//cliente

/**
* @brief ingresoCredenciales Toma los datos ingresados por teclado correspondientes a usuario y contraseña y los almacena en la estructura UserPass
**/
void envioCredenciales();	//cliente

/**
* @brief ecepcionuserData decibe la estructura con toda la informacion pertinente del cliente.
*/
void recepcionuserData(); //server

/**
* @brief parser Parsea la entrada por teclado en un array de argumentos. 
* @param *entrada Puntero que apunta al vector entrada[], el cual contiene lo ingresado por teclado.
* @param delimitador[] es el string el cual va a buscar como delimitador para separar la entrada.
* @param *comando[] Vector de comandos parseados.
* @return cantidad de comandos parseados.
*/
int parser(char *entrada,char delimitador[],char *comando[]); //cliente

/**
* @brief concatenar une en un solo char el usuario y contraseña ingresado, además le agrega un delimitador "/" entre ambos para poder ser parseado en el servidor
* @param *s1 usuario
* @param *s2 contraseña
* @return resultado es lo que se envia al servidor y tiene este formato usuario/contraseña.
*/

/**
* @brief envioAuthService crea una fifo y le envia datos a traves de la misma al proceso authService.
* @param opcion es el tipo de peticion que le hace el primary server al auth server (validacion user/pass, users ls, newpass).
**/
void envioAuthService(int32_t opcion);	//Server

/**
* @brief validacionUser Recibe los datos que le envió el primary server obtenidos del cliente, consulta la base de dato 
* 		 y determina si los datos de usuario, contraseña y estado del usuario son correctos.
* @return devuelve un entero: 0 = acceso denegado, 1 = acceso validado.
*/
void validacionUser();	//auth

/**
* @brief creacionBdd Si no existe la base de datos de los usuarios, crea la tabla y le inserta algunos registros.
*				     Si ya existe, entonces no hace nada. 
**/
void creacionBdd();	//auth

/**
* @brief consulta Realiza la consulta a la base de datos.
* @param opcion es el tipo de peticion que realizo el usuario, si es para validar user/pass, listar users, o cambiar la pass.
**/
void consulta(int32_t opcion);	//auth

/**
* @brief actualizaFecha se encarga de actualizar la fecha y hora de la ultima conexion del usuario.
**/
void actualizaFecha(); //auth

/**
* @brief cuentaUsuarios cuenta la cantidad de usuarios registrados en la bdd, para saber cuanto tengo que enviar en el user ls.
**/
void cuentaUsuarios(); //auth

/**
* @brief selectValid Denominada funcion callback, es la que llama sqlite cuando se le hace una consulta para validar usuario y contraseña.
**/
static int selectValid(void *nada, int argc, char **valor, char **campo);	//auth

/**
* @brief selectBloq Denominada funcion callback, es la que llama sqlite para actualizar el campo de estado y ponerlo en bloqueado.
**/
static int selectBloq(void *nada, int argc, char **valor, char **campo);	//auth

/**
* @brief selectLs Denominada funcion callback, es la que llama sqlite para hacer la consulta de la lista de usuarios.
**/
static int selectLs(void *nada, int argc, char **valor, char **campo);	//auth

/**
* @brief selectNewPass Denominada funcion callback, es la que llama sqlite para actualizar la clave del usuario.
**/
static int selectNewPass(void *nada, int argc, char **valor, char **campo);

/**
* @brief selectFecha Denominada funcion callback, es la que llama sqlite cuando actualiza la fecha de ultima conexión.
**/
static int selectFecha(void *nada, int argc, char **valor, char **campo);	//auth

/**
* @brief selectCuenta Denominada funcion callback, es la que llama sqlite para saber cuantos usuarios tengo registrado en la bdd.
**/
static int selectCuenta(void *nada, int argc, char **valor, char **campo);	//auth



/**
* @brief bloquearUsuarioBdd actualiza el campo de bloqueado de la base de datos, seteando como bloqueado el usuario en cuestion.
**/
void bloquearUsuario();	//auth

/**
* @brief help imprime en pantalla la lista de comandos que acepta el servidor y sus argumentos.
**/
void help();	//cliente

