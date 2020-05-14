#Directorios
BINARY_DIR=bin

#Binarios
CLIENT=client
SERVER=server
SERVER_AUTH=auth
SERVER_FS=fileserv
SQLITE=sqlite3

CC=gcc
CFLAGS=-std=gnu11  #-Wall -Werror -pedantic -Wextra -Wconversion -O1
DEPS = commonFunctions.h

all : client server auth
.PHONY: all

client: cliente.c commonFunctions.h clientFunctions.h
	mkdir -p $(BINARY_DIR)
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(CLIENT).o -c cliente.c
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(CLIENT)   $(BINARY_DIR)/$(CLIENT).o

server: priServer.c commonFunctions.h serverFunctions.h
	mkdir -p $(BINARY_DIR)
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(SERVER).o -c priServer.c
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(SERVER)   $(BINARY_DIR)/$(SERVER).o

auth:	authService.c commonFunctions.h authFunctions.h sqlite3.c sqlite3.h
	mkdir -p $(BINARY_DIR)
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(SERVER_AUTH).o -c authService.c
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(SQLITE).o -c sqlite3.c
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(SERVER_AUTH)   $(BINARY_DIR)/$(SERVER_AUTH).o $(BINARY_DIR)/$(SQLITE).o -ldl -lpthread

fileserv: fileService.c commonFunctions.h fileFunctions.h
	mkdir -p $(BINARY_DIR)
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(SERVER_FS).o -c fileService.c
	$(CC) $(CFLAGS)  -o $(BINARY_DIR)/$(SERVER_FS)   $(BINARY_DIR)/$(SERVER_FS).o

.PHONY: clean
clean :
	rm  -Rf $(BINARY_DIR)
