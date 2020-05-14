#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

int main(int argc, char* argv[])
{
   sqlite3 *db;
   int res;

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

   sqlite3_close(db);

   return 0;
}