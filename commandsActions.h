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

void getCommand(int i, char* user);
void connectPort(int port);
Protocol * readMsg();
void checkConn();

#endif //PROJECTF1_1_COMMANDSACTIONS_H
