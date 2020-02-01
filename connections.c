//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#include "connections.h"
#include <types.h>
#include <stdlib.h>
#include <dirent.h>

Protocol p;

void trNameFunc (Control *c_control) {
    c_control->name = realloc(c_control->name, strlen(c_control->rcv_msg->data));
    strcpy(c_control->name, c_control->rcv_msg->data);

    conOKFunc(c_control);
}
void conOKFunc (Control *c_control) {
    //resetProtocol(c_control->send_msg);
    fillProtocol(c_control->send_msg, '1', "[CONOK]", FILEDATA.user_name);
    sendMsg(c_control);
}
void conKOFunc (Control *c_control) {
    //resetProtocol(c_control->send_msg);
    fillProtocol(c_control->send_msg, '1', "[CONKO]", "");
    sendMsg(c_control);
}
void msgFunc (Control *c_control) {
    //myprint("\0337");  // Saves cursor position and attributes
    myprint("\033[1A\033[180D\n\033[K");  // Move cursor 1 line up and 180 characters left and clear line
    myprint("[");
    myprint(c_control->name);
    myprint("]:");
    write(1, c_control->rcv_msg->data, atoi(c_control->rcv_msg->length)); //print the message received
    //myprint("\n\0338");  // Restore cursor from saved position and attributes
    myprint("\033[1B\033[180D\n");  // Move cursor 1 line down and 180 characters left and clear line

    write(1, "$", strlen ("$"));
    write(1, FILEDATA.user_name, strlen(FILEDATA.user_name));
    write(1, ":", strlen (":"));
}
void showAudiosFunc (Control *c_control) {
    DIR *d;
    struct dirent *dir;
    char audio_dir[30];
    sprintf(audio_dir, "./%s", FILEDATA.audio_folder);
    d = opendir(audio_dir);
    char *audio_list;
    int str_len = 1, prev_len = 0;
    audio_list = malloc(1);
    audio_list[prev_len] = '[';
    prev_len = str_len;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] == '.')
                continue;
            str_len += strlen(dir->d_name) + 1;

            // Appending each file to the audio_list:
            audio_list = realloc(audio_list, str_len);
            strcpy(&audio_list[prev_len], &dir->d_name[0]);

            audio_list[str_len-1] = '\\';

            prev_len = str_len;
        }
        closedir(d);
    }
    audio_list[str_len-1] = ']';
    audio_list[str_len] = '\0';

    resetProtocol(c_control->send_msg);
    fillProtocol(c_control->send_msg, '4', "[LIST_AUDIOS]", audio_list);
    sendMsg(c_control);
}
/**
 * Send the checksum of a file using md5sum external program, using forks, execvp and pipes
 * @param c_control
 * @param filedir
 * @return FALSE in case of failure, TRUE in success
 *//*
int sendChcksum(Control *c_control){
    return TRUE;
}*/
/**
 * Send a file in chunks of data to the client file descriptor inside c_control struct
 * @param c_control
 * @param filename
 * @return FALSE in case of failure, TRUE in success
 **/
int sendfile(Control *c_control, char* filename){
    int error = 0;
    char filedir[100];
    printf("Sending0\n");
    sprintf(filedir, "./%s/%s", FILEDATA.audio_folder, filename);
    myprint(filedir);
    int f_fd = open(filedir, O_RDONLY);
    if(f_fd < 0)
        return FALSE;

    char BUFFER[CHUNK_SIZE];
    do{
        error = read(f_fd, BUFFER, CHUNK_SIZE);
        resetProtocol(c_control->send_msg);
        printf("rst\n");
        fillProtocol(c_control->send_msg, '5', "[AUDIO_RSPNS]", BUFFER);
        printf("fill\n");
        sendMsg(c_control);
    }while(error > 0); // Until end of file.

    if(error < 0)
        return FALSE;

    // TODO
    //sendChcksum(c_control);

    return TRUE;
}
void audioRqstFunc (Control *c_control) {
    int success = FALSE;
    //success = sendfile(c_control, c_control->rcv_msg->data);
    if (success == FALSE) {
        resetProtocol(c_control->send_msg);
        fillProtocol(c_control->send_msg, '5', "[AUDIO_KO]", "");
        sendMsg(c_control);
    }
}
////////
/* TODO: */
/*
void *broadcastFunc (Control *c_control) {

}

void *audioRqstFunc (Control *c_control) {

}
*/
////////
void endConn (Control *c_control){
    c_control->end_conn = TRUE;
}
//we parse the header to know which action we have to do
int parseHeader (Protocol p) {
    //TODO: Complete all the options/commands here and also we have to take into account
    //      the id of the protocol (otherwise, we will have a conflict with CONOK)

    char headers[][20] = {TR_NAME, CONOK, CONKO, MSG, SHOW_AUDIOS, AUDIO_RQST, EXIT_MSG};
    int i=-1, length = sizeof(headers)/sizeof(headers[0]), found = FALSE;

    for(i = 0; i < length; i++) {
        found = (strcasecmp(p.header, headers[i]) == 0)? TRUE: FALSE;
        if(found == TRUE)
            return i;
    }

    return i;
}
void sendMsg (Control *c_control) {
    write (c_control->fd_client,&c_control->send_msg->id, 1);
    write (c_control->fd_client,c_control->send_msg->header, strlen(c_control->send_msg->header));
    write (c_control->fd_client,c_control->send_msg->length, strlen(c_control->send_msg->length));
    write (c_control->fd_client,c_control->send_msg->data, strlen(c_control->send_msg->data));

}
void freeControl(Control *_control){
    free(_control->name);
    free(_control->ip);
    freeProtocol(_control->rcv_msg);
    freeProtocol(_control->send_msg);

    return;
}
void getMsg(Control *control){
    int error = read(control->fd_client, &control->rcv_msg->id, 1); // reading id
    if(error < 1){
        control->end_conn = TRUE;
        return;
    }
    char* asd_ = calloc(0, 0); // throwaway variable
    readUntil(control->fd_client, &asd_, '['); // reading '['
    //memset(control->rcv_msg->header,0,strlen(control->rcv_msg->header)); // empty-ing header
    readUntil(control->fd_client, &control->rcv_msg->header, ']'); // reading header
    read(control->fd_client, control->rcv_msg->length, 2); // reading length

    int _length = atoi(control->rcv_msg->length);
    //memset(control->rcv_msg->data,0,strlen(control->rcv_msg->data)); // empty-ing data
    control->rcv_msg->data = realloc(control->rcv_msg->data, _length); // length bytes long (pretty obvious isn't it)
    char *_data = malloc(_length);
    read(control->fd_client, _data, _length);
    strcpy(control->rcv_msg->data, _data);

    free(_data);
    free(asd_);
    return;
}
//for creating the big server (main one)/listener
void *openServer (void *_control) {
    Control *control = (Control*)_control;
    int listenfd = -1, connfd = -1;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];

    /* creates an UN-named socket inside the kernel and returns
     * an integer known as socket descriptor
     * This function takes domain/family as its first argument.
     * For Internet family of IPv4 addresses we use AF_INET
     */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(control->ip);
    serv_addr.sin_port = htons(control->port);

    /* The call to the function "bind()" assigns the details specified
     * in the structure 'serv_addr' to the socket created in the step above
     */
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    /* The call to the function "listen()" with second argument as 10 specifies
     * maximum number of client connections that server will queue for this listening
     * socket.
     */
    if(listen(listenfd, 100) == -1) {
        myprint("ERROR: listen error\n");
        close(listenfd);
        exit(EXIT_FAILURE);
    }
    //pthread_t *thread_ids = calloc(0,0);
    int num_of_threads = 0, actual_thread = 0;
/*
    // setting a list of threads
    // Fist, we create a null head, and we'll link the next threads to this head
    node_t * head = NULL;
    head = malloc(sizeof(node_t));
    if (head == NULL) {
        return 1;
    }
    head->data = NULL;
    head->next = NULL;
*/
    Control * child_ctrls = calloc (0,0);
    break_listener = FALSE;
    while(break_listener == FALSE)
    {
        /* In the call to accept(), the server is put to sleep and when for an incoming
         * client request, the three way TCP handshake* is complete, the function accept()
         * wakes up and returns the socket descriptor representing the client socket.
         */
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        if(break_listener == TRUE)
            break;
        num_of_threads++;
        //thread_ids = realloc(thread_ids, num_of_threads);
        int connfd_aux = connfd; // client file descriptor
        // new child
        child_ctrls = realloc(child_ctrls, sizeof(Control)*num_of_threads);
        // Filling child settings
        child_ctrls[actual_thread].fd_client = connfd_aux;
        child_ctrls[actual_thread].port = control->port; // from parent
        child_ctrls[actual_thread].ip = control->ip;     // from parent
        child_ctrls[actual_thread].end_conn = FALSE;
        child_ctrls[actual_thread].name = calloc(0,0);

        child_ctrls[actual_thread].rcv_msg = newProtocol();
        child_ctrls[actual_thread].send_msg = newProtocol();
        pthread_create(&child_ctrls[actual_thread].th_id, NULL, newConnection, (void*)&(child_ctrls[actual_thread]) ); // HERE

        actual_thread++;

        sleep(1);
    }
    // Freeing memory:
    int i;
    for(i=0; i<num_of_threads; i++){
        pthread_kill(child_ctrls[i].th_id, SIGTERM);
        sleep(2);
        pthread_join(child_ctrls[i].th_id, NULL);
        /* TODO: instead of doing this, kill the child threads on parent (don't wait for global variables) and start
         *      the closing routine here (sending the DC packet to the client
         */
        close(child_ctrls[i].fd_client);
        freeControl(&child_ctrls[i]);
        //free(child_ctrls[i]);
    }
    free(child_ctrls);

    return NULL;
}
// this one is to create the small "servers"
void * newConnection (void *_control) {
    Control *control = (Control*)_control;

    void (*func_array[])(Control*) = {trNameFunc, conOKFunc, conKOFunc, msgFunc, showAudiosFunc, endConn};

    while(break_listener == FALSE) {
        getMsg(control);
        if(control->end_conn == TRUE){
            // TODO: start server-client disconnection routine (?
            /*   fillProtocol(control->send_msg, '6', "[CONOK]", "0", " ");
               myprint("fce1\n");
               sendMsg(control);
               myprint("fce1\n");*/
            return NULL;
        }
        int option = parseHeader(*(control->rcv_msg));
        if(option >= 0){
            if(option > 6){
                myprint("ERROR: Wrong command/input\n");
                continue;
            }
            func_array[option](control);
        }else
            control->end_conn = TRUE;

        resetProtocol(control->rcv_msg);
    }

    return NULL;
}
