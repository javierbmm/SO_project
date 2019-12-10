//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#ifndef SO_PROJECT_CONNECTIONS_H
#define SO_PROJECT_CONNECTIONS_H

#include "types.h"
#include <fileManager.h>
#include <list.h>

Protocol *newProtocol();
void fillProtocol(Protocol *_p, char _id, char * _header, char * _length, char *_data);
void *trNameFunc ();
void *conOkFunc (Control *c_control);
void *conKOFunc (Control *c_control);
void *msgFunc (Control *c_control);
void *msgOKFunc (Control *c_control);
void *broadcastFunc (Control *c_control);
void *showAudiosFunc (Control *c_control);
void *listAudiosFunc (Control *c_control);
void *audioRqstFunc (Control *c_control);
void *endConn (Control *c_control);
int parseHeader (Protocol p);
void getMsg (Control *control);
void freeProtocol(Protocol *_p);
void freeControl(Control *_control);
void * sendMsg (Control *c_control);
//for creating the big server (main one)/listener
void *openServer (void * control);
//this one is to create the small "servers"
void * newConnection (void *control) ;

#endif //SO_PROJECT_CONNECTIONS_H
