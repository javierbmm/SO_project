//
// Created by Olívia Rey Pellicer on 25/10/2019.
//

#ifndef PROJECTF1_1_COMMANDSACTIONS_H
#define PROJECTF1_1_COMMANDSACTIONS_H
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "types.h"

int conn_fd;
char *conn_username;

void closeConn();
void getCommand(int i, char* user);
int connectPort(int port);
Protocol* readMsg();
int sendtofd(Protocol p, int fd);
void checkConn();
void handle_sigpipe();
#endif //PROJECTF1_1_COMMANDSACTIONS_H
