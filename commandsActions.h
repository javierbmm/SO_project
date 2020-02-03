//
// Created by Olívia Rey Pellicer on 25/10/2019.
//

#ifndef PROJECTF1_1_COMMANDSACTIONS_H
#define PROJECTF1_1_COMMANDSACTIONS_H
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "types.h"

typedef struct{
    int conn_fd;
    char *conn_username;
    int port;
    int is_connected;
}serverConn;

serverConn** arrConns;
int num_conn;

void closeConn();
void getCommand(int i, char* user);
int connectPort(int port);
Protocol* readMsg(int conn_fd);
void show_connections();
int sendtofd(Protocol p, int fd);
void checkConn();
void handle_sigpipe();
serverConn* newServerConn(int conn_fd, char* username, int port, int status);
void closeServConn(serverConn* _sc);
int searchByName(char* username);
int searchByPort(const int port);
int isConnected(const int port);

#endif //PROJECTF1_1_COMMANDSACTIONS_H
