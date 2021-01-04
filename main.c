
#include "fileManager.h"
#include "commandsActions.h"
#include "parsingInputs.h"
#include "connections.h"

#include <fcntl.h>
#include <unistd.h>

#define FILENAME_ERROR "Error! You must introduce the name of the configuration file.\n\n"
#define START "Starting Trinity...\n\n"
#define PERSEFONE "$Persefone:  "

void writeUser() {
    write(1, "$", strlen ("$"));
    write(1, FILEDATA.user_name, strlen(FILEDATA.user_name));
    write(1, ":", strlen (":"));
}
void dc_routine(){
    close_1 = TRUE;
    break_listener = TRUE;
}
void kctrlc() {
    dc_routine();
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
    signal(SIGPIPE, handle_sigpipe);
    CHUNK_SIZE = 99;
    char *user = calloc(1, 1);
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
    getFileData(file, &FILEDATA);
    Control ctrl_server;
    ctrl_server.port = atoi(FILEDATA.port); // from parent
    ctrl_server.ip = FILEDATA.ip;     // from parent
    ctrl_server.end_conn = FALSE;
    ctrl_server.rcv_msg = NULL;
    ctrl_server.send_msg = NULL;

    pthread_create(&ctrl_server.th_id, NULL, openServer, (void*)&ctrl_server);
    //user = realloc(user, (BUFF_SIZE)*sizeof(user));


    // initialazing variables from commandsActions.h
    conn_fd = 0;
    conn_username = calloc(1,1);
    while (close_1 == FALSE) {
        writeUser();
        readUntil(STDIN_FILENO, &user,'\n');
        if(close_1 == TRUE)
            break;
        command = parseInput(user);

        if (command >= 0 && command < 6) {
            getCommand(command, user);
        }
        else if(command == 6) {
            dc_routine();
            break;
        }
    }
    write (1, "\nDISCONNECTING TRINITY...\n", strlen("DISCONNECTING TRINITY...\n\n\n"));
    free(user);
    free(conn_username);
    shutdown(ctrl_server.fd_server, SHUT_RD);
    FILEDATA.user_name? free(FILEDATA.user_name): NULL;
    FILEDATA.audio_folder? free(FILEDATA.audio_folder): NULL;
    FILEDATA.ip? free(FILEDATA.ip): NULL;
    FILEDATA.port? free(FILEDATA.port): NULL;
    FILEDATA.web_ip? free(FILEDATA.web_ip): NULL;
    FILEDATA.final_port? free(FILEDATA.final_port): NULL;
    FILEDATA.init_port? free(FILEDATA.init_port): NULL;
    close(file);
    pthread_join(ctrl_server.th_id, NULL);

   // close(listener_id);

    return 0;
}