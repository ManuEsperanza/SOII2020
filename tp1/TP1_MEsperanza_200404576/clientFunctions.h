/**
  * @file clientFunctions.h
  * @version 1.0
  * @date 21/03/2020
  * @author Esperanza Manuel
  * @brief prototipos de las funciones que utiliza el cliente. 
  */

/**
* @brief creacion_socket_cli Crea y configura el socket del lado del cliente.
*/
void creacion_socket_cli(); 

/**
* @brief ingresoCredenciales Toma los datos ingresados por teclado correspondientes a usuario y contraseña y los almacena en la estructura UserPass
* 		 para enviarla al primary server.
*/
void ingresoCredenciales();	

/**
* @brief ingresoCredenciales Toma los datos ingresados por teclado correspondientes a usuario y contraseña y los almacena en la estructura UserPass
**/
void envioCredenciales();	

/**
* @brief parser Parsea la entrada por teclado en un array de argumentos. 
* @param *entrada Puntero que apunta al vector entrada[], el cual contiene lo ingresado por teclado.
* @param delimitador[] es el string el cual va a buscar como delimitador para separar la entrada.
* @param *comando[] Vector de comandos parseados.
* @return cantidad de comandos parseados.
*/
int parser(char *entrada,char delimitador[],char *comando[]); 

/**
* @brief help imprime en pantalla la lista de comandos que acepta el servidor y sus argumentos.
**/
void help();	//cliente