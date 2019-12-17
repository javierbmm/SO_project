//
// Created by javif on 12/12/2019.
//

#ifndef SO_PROJECT_PROTOCOL_H
#define SO_PROJECT_PROTOCOL_H

#include <stdlib.h>
typedef struct {
    char id;
    char * header;
    char * length;
    char * data;
} Protocol; //

Protocol *newProtocol();
void freeProtocol(Protocol *_p);
void fillProtocol(Protocol *_p, char _id, char * _header, char * _length, char *_data);


#endif //SO_PROJECT_PROTOCOL_H
