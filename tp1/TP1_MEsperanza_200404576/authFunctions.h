/**
  * @file authFunctions.h
  * @version 1.0
  * @date 21/03/2020
  * @author Esperanza Manuel
  * @brief prototipos de las funciones que utiliza el auth server. 
  */

/**
* @brief validacionUser Recibe los datos que le envió el primary server obtenidos del cliente, consulta la base de dato 
* 		 y determina si los datos de usuario, contraseña y estado del usuario son correctos.
* @return devuelve un entero: 0 = acceso denegado, 1 = acceso validado.
*/
void validacionUser();	

/**
* @brief creacionBdd Si no existe la base de datos de los usuarios, crea la tabla y le inserta algunos registros.
*				     Si ya existe, entonces no hace nada. 
**/
void creacionBdd();	

/**
* @brief consulta Realiza la consulta a la base de datos.
* @param opcion es el tipo de peticion que realizo el usuario, si es para validar user/pass, listar users, o cambiar la pass.
**/
void consulta(int32_t opcion);	

/**
* @brief actualizaFecha se encarga de actualizar la fecha y hora de la ultima conexion del usuario.
**/
void actualizaFecha(); 

/**
* @brief cuentaUsuarios cuenta la cantidad de usuarios registrados en la bdd, para saber cuanto tengo que enviar en el user ls.
**/
void cuentaUsuarios(); 

/**
* @brief bloquearUsuarioBdd actualiza el campo de bloqueado de la base de datos, seteando como bloqueado el usuario en cuestion.
**/
void bloquearUsuario();	

/**
* @brief selectValid Denominada funcion callback, es la que llama sqlite cuando se le hace una consulta para validar usuario y contraseña.
**/
static int selectValid(void *nada, int argc, char **valor, char **campo);	

/**
* @brief selectBloq Denominada funcion callback, es la que llama sqlite para actualizar el campo de estado y ponerlo en bloqueado.
**/
static int selectBloq(void *nada, int argc, char **valor, char **campo);

/**
* @brief selectLs Denominada funcion callback, es la que llama sqlite para hacer la consulta de la lista de usuarios.
**/
static int selectLs(void *nada, int argc, char **valor, char **campo);	

/**
* @brief selectNewPass Denominada funcion callback, es la que llama sqlite para actualizar la clave del usuario.
**/
static int selectNewPass(void *nada, int argc, char **valor, char **campo);

/**
* @brief selectFecha Denominada funcion callback, es la que llama sqlite cuando actualiza la fecha de ultima conexión.
**/
static int selectFecha(void *nada, int argc, char **valor, char **campo);	

/**
* @brief selectCuenta Denominada funcion callback, es la que llama sqlite para saber cuantos usuarios tengo registrado en la bdd.
**/
static int selectCuenta(void *nada, int argc, char **valor, char **campo);	