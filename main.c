
#include "fileManager.h"
#include "commandsActions.h"
#include "parsingInputs.h"
#include "connections.h"

#include <fcntl.h>
#include <unistd.h>

#define FILENAME_ERROR "Error! You must introduce the name of the configuration file.\n\n"
#define START "Starting Trinity...\n\n"
#define PERSEFONE "$Persefone:  "


void writeUser(char * username) {
    write(1, "$", strlen ("$"));
    write(1, username, strlen(username));
    write(1, ":", strlen (":"));
}

void kctrlc() {
    write (1, "\nDISCONNECTING TRINITY...\n", strlen("DISCONNECTING TRINITY...\n\n\n"));
    close_1 = TRUE;
    /* the next command is to change the locking behaviour of system call. My idea is to unblock read() so I can
     * interrupt the program whenever the user press ctrl+c
    */
    close(STDIN_FILENO);
    //fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK); // TODO: check this function because can be dangerous (see the comment above)
}
int main(int arg, const char* argv[]) {
//    fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_NONBLOCK);
    close_1 = FALSE;
    signal(SIGINT, kctrlc);
    signal(SIGTERM, kctrlc);
    char *user = calloc(0,0);
    int command, file;
    char buffer[BUFF_SIZE];

    if (arg != 2) {
        write(1, FILENAME_ERROR, strlen(FILENAME_ERROR));
        exit(1);
    }

    write(1, START, strlen(START));
    file = open (argv[1], O_RDONLY);
    if(file < 0) {
        sprintf(buffer,"<%s>: Error opening file. Check it and try again\n", argv[1]);
        write(1, &buffer, strlen(buffer));

        exit(1);
    }

    FILEDATA = getFileData(file);
    Control ctrl_server;
    ctrl_server.th_id = NULL;
    ctrl_server.port = atoi(FILEDATA.port); // from parent
    ctrl_server.ip = FILEDATA.ip;     // from parent
    ctrl_server.end_conn = FALSE;
    ctrl_server.rcv_msg = NULL;
    ctrl_server.send_msg = NULL;

    pthread_t listener_id;
    pthread_create(&listener_id, NULL, openServer, (void*)&ctrl_server);
    //user = realloc(user, (BUFF_SIZE)*sizeof(user));

    while (close_1 == FALSE) {
        writeUser(FILEDATA.user_name);
        readUntil(STDIN_FILENO, &user,'\n');
        if(close_1 == TRUE)
            break;
        command = parseInput(user);

        if (command >= 0 && command != 6) {
            getCommand(command, user);
        }
        else if(command == 6){
            break;
        }
        // Empty-ing the buffer
        //printf("%ld",sizeof(user));
        //memset(user,0, sizeof(user));
        //user[0] = '\0';
        printf("finished\n");

    }

    free(user);
    close(file);

    return 0;
}