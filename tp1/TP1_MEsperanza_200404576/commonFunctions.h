/**
  * @file commonFunctions.h
  * @version 1.0
  * @date 21/03/2020
  * @author Esperanza Manuel
  * @brief prototipos de las funciones, estructuras y defines de uso comun para todos los procesos. 
  */

#define PUERTO 1244
#define ENTRADA 100
#define LINEA 1024
#define STDOUT_FID 1
#define	ERROR	-1
#define READNWRITE 0666
#define TAM 256
#define BUFFSIZE 1

/**
 * @struct userData
 * @brief contiene el nombre de usuario y contraseña ingresado desde el cliente. Tambien contiene el tipo de peticion que hace el cliente al servidor.
 */
typedef struct {
	int32_t peticion;
	char user[ENTRADA];
	char pass[ENTRADA];
}UserData;

/**
 * @struct userList
 * @brief contiene la informacion pertinente que devuelve el comando user ls.
 */
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

/** enum opcionCliente
*   hace referencia a los tipos de peticion que puede realizar el cliente.
*/
enum opcionCliente {
	validacion,
	userLs,
	newPass,
	fileLs,
	fileDown,
	salida
};

/** enum accesType
*   hace referencia a la respuesta que emite el auth server con respecto a la validacion de usuario y contraseña.
*/
enum accesType{
	usuarioInexistente,
	passIncorrecta,
	usuarioBloqueado,
	AccesoOK
};

/**
* @brief lecturaFifo abre el file descriptor del fifo como lectura para leer lo que otro proceso haya escrito en el mismo.
**/
void lecturaFifo();	

/**
* @brief escrituraFifo abre el file descriptor del fifo como escritura para escribir en la fifo la informacion deseada.
**/
void escrituraFifo();	//todos