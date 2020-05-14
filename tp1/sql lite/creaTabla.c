#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

int main(int argc, char* argv[])
{
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
    "`estado` NUMBER, "
    "`ultimaConex` DATETIME)";

  /* Execute SQL statement */
  res = sqlite3_exec(db, tabla, NULL, 0, &error);
  if (res == SQLITE_OK){
    info = "INSERT INTO usersInfo VALUES ('manu','miclave',0,DATETIME(STRFTIME('%s','now')-10800, 'unixepoch'));"
          "INSERT INTO usersInfo VALUES ('admin','admin',0,DATETIME(STRFTIME('%s','now')-10800, 'unixepoch'));";
    res = sqlite3_exec(db, info, NULL, 0, &error);
    if (res != SQLITE_OK){
      fprintf(stderr, "Error: %s\n", error);
      sqlite3_free(error);
    }
  }
  sqlite3_close(db);
  return 0;
}