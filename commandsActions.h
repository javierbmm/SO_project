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

void getCommand(int i, char* user, FileData fileData);
void connectPort(int port, FileData fileData);

#endif //PROJECTF1_1_COMMANDSACTIONS_H
