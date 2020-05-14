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

int a;

static int selectCb(void *nada, int argc, char **argv, char **colNames){
  int i;
  a++;
  for(i=0; i<argc; i++){
    printf("%s => %s\n", colNames[i], argv[i]);
  }
  printf("\n");
  return 0;
}

int main(int argc, char* argv[]){
  sqlite3 *db;
  char *error = 0;
  int res;
  char *sql;

  /* Open database */
  res = sqlite3_open("bdd.db", &db);
  if (res){
    fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
    exit(0);
  }
  sql = "SELECT usuario,estado,ultimaConex FROM usersInfo";
  char *nombre = "admin";
  /* Create SQL statement */
  char *result = malloc(strlen(sql) + strlen(nombre)+2); // +2 para el (';)final
  // in real code you would check for errors in malloc here
  strcpy(result, sql); // concateno el select que quiero hacer en la tabla mas el nombre de usuario y mas el (';') final para completar la consulta a la bdd.
  strcat(result, nombre);
  strcat(result,"';");


  /* Execute SQL statement */
  res = sqlite3_exec(db, sql, selectCb, 0, &error);
  if (res != SQLITE_OK){
    fprintf(stderr, "Error: %s\n", error);
    sqlite3_free(error);
  }
  sqlite3_close(db);
  return 0;
}  