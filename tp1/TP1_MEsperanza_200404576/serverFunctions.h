/**
  * @file serverFunctions.h
  * @version 1.0
  * @date 21/03/2020
  * @author Esperanza Manuel
  * @brief prototipos de las funciones que utiliza el Primary server. 
  */

/**
* @brief creacion_socket_serv Crea y configura el socket del lado del servidor.
*/
void creacion_socket_serv();	

/**
* @brief creacionFifos crea las fifo necesarias para la comunicacion entre el primary server y los procesos authService y fileService.
*/
void creacionFifos();	

/**
* @brief ecepcionuserData decibe la estructura con toda la informacion pertinente del cliente.
*/
void recepcionuserData(); 

/**
* @brief envioAuthService crea una fifo y le envia datos a traves de la misma al proceso authService.
* @param opcion es el tipo de peticion que le hace el primary server al auth server (validacion user/pass, users ls, newpass).
**/
void envioAuthService(int32_t opcion);

