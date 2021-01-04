//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#ifndef SO_PROJECT_CONNECTIONS_H
#define SO_PROJECT_CONNECTIONS_H

#include "types.h"
#include <fileManager.h>
#include <list.h>



void trNameFunc ();
void conOKFunc (Control *c_control);
void conKOFunc (Control *c_control);
void msgFunc (Control *c_control);
void broadcastFunc (Control *c_control);
void showAudiosFunc (Control *c_control);
void audioRqstFunc (Control *c_control);
int sendChcksum(Control *c_control, char *filedir);
int sendfile(Control *c_control, char* filename);
void endConn (Control *c_control);
int parseHeader (Protocol p);
void getMsg (Control *control);
void freeControl(Control *_control);
void sendMsg (Control *c_control);
//for creating the big server (main one)/listener
void *openServer (void * control);
//this one is to create the small "servers"
void * newConnection (void *control);

#endif //SO_PROJECT_CONNECTIONS_H
