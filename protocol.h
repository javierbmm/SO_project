//
// Created by javif on 12/12/2019.
//

#ifndef SO_PROJECT_PROTOCOL_H
#define SO_PROJECT_PROTOCOL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    char id;
    char * header;
    char * length;
    char * data;
} Protocol; //

Protocol *newProtocol();
void freeProtocol(Protocol *_p);
void resetProtocol(Protocol *_p);
void fillProtocol(Protocol *_p, char _id, char * _header, char *_data);
void printProtocol(Protocol p);

#endif //SO_PROJECT_PROTOCOL_H
