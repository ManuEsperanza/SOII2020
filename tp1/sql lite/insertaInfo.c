#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *error = 0;
   int res;
   char *sql;

   /* Open database */
   res = sqlite3_open("bdd.db", &db);
   if (res)
     {
       fprintf(stderr, "No puedo abrir la base de datos: %s\n", sqlite3_errmsg(db));
       exit(0);
     }
   else
     {
       fprintf(stderr, "Base de datos OK\n");
     }
   /* Create SQL statement */
   sql = "INSERT INTO usersInfo VALUES ('manu','miclave',0,DATETIME(STRFTIME('%s','now')-10800, 'unixepoch'));"
  	"INSERT INTO usersInfo VALUES ('admin','admin',0,DATETIME(STRFTIME('%s','now')-10800, 'unixepoch'));";

   /* Execute SQL statement */
   res = sqlite3_exec(db, sql, NULL, 0, &error);
   if (res != SQLITE_OK)
     {
       fprintf(stderr, "Error: %s\n", error);
       sqlite3_free(error);
     }
   else
     {
       fprintf(stdout, "Tabla creada!\n");
     }

   sqlite3_close(db);

   return 0;
}
   
     

   	/*
     "INSERT INTO events VALUES (DATETIME(STRFTIME('%s','now')+86400, 'unixepoch'), 10, 4, 'This is a test again');"
     "INSERT INTO events VALUES (DATETIME(STRFTIME('%s','now')+86400*2, 'unixepoch'), 100, 8, 'This is a test again x2');"
     "INSERT INTO events VALUES (DATETIME(STRFTIME('%s','now')+86400*20, 'unixepoch'), 1000, 16, 'This is a test again x3');";*/