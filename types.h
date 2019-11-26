//
// Created by Olívia Rey Pellicer and Javier Merida on 25/10/2019.
//

#ifndef TYPES_H
#define TYPES_H

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define SHOWCONNECTIONS_ID "0"
#define CONNECT_ID "1"
#define SAY_ID "2"
#define BROADCAST_ID "3"
#define SHOWAUDIOS_ID "4"
#define DOWNLOAD_ID "5"
#define EXIT_ID "6"
#define SHOWCONNECTIONS "SHOW CONNECTIONS"
#define CONNECT "CONNECT"
#define SAY "SAY"
#define BROADCAST "BROADCAST"
#define SHOWAUDIOS "SHOW AUDIOS"
#define DOWNLOAD "DOWNLOAD"
#define EXIT "EXIT"
#define ERRORCOMMAND "ERROR! Not valid command.\n"
#define TESTING "Testing..."
#define NOCONNECTIONS "No connections available.\n"
#define COULDNTCONNECT "Could not connect to the port.\n"
#define BUFF_SIZE 180
#define TRUE 1
#define FALSE 0

volatile int close_1; // Variable to control the system when a killing signal is received (SIGINT, SIGTERM)

typedef struct  {
    char * user_name;
    char * audio_folder;
    char * ip;
    char * port;
    char * web_ip;
    char * init_port;
    char * final_port;
} FileData;

typedef struct {
    int id;
    char * user;
    char * data;
} Input;

#endif //TYPES_H
