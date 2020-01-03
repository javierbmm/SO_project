//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#include "connections.h"
#include <types.h>
#include <stdlib.h>
Protocol p;

void *trNameFunc (Control *c_control) {
    fflush(stdout);
    c_control->name = realloc(c_control->name, atoi(c_control->rcv_msg->length));
    fflush(stdout);
    strcpy(c_control->name, c_control->rcv_msg->data);
    conOKFunc(c_control);
}
void *conOKFunc (Control *c_control) {
    char* header = c_control->rcv_msg->header;

    char id = c_control->rcv_msg->id;
    char _length [2];
    sprintf(_length,"%ld",strlen(FILEDATA.user_name));
    fillProtocol(c_control->send_msg, '1', "[CONOK]", FILEDATA.user_name);
    fflush(stdout);
    sendMsg(c_control);
}
void *conKOFunc (Control *c_control) {

}

void *msgFunc (Control *c_control) {
    myprint("\0337");  // Saves cursor position and attributes

    myprint("\033[1A\033[180D");  // Move cursor 1 line up and 180 characters left

    myprint("[");
    myprint(c_control->name);
    myprint("]:");
    write(1, c_control->rcv_msg->data, strlen(c_control->rcv_msg->data)); //print the message received

    myprint("\0338");  // Restore cursor from saved position and attributes
}
////////
/* TODO: */
/*
void *broadcastFunc (Control *c_control) {

}
void *showAudiosFunc (Control *c_control) {

}
void *listAudiosFunc (Control *c_control) {

}
void *audioRqstFunc (Control *c_control) {

}*/
////////

void *endConn (Control *c_control){
    c_control->end_conn = TRUE;
}
//we parse the header to know which action we have to do
int parseHeader (Protocol p) {
    //TODO: Complete all the options/commands here and also we have to take into account
    //      the id of the protocol (otherwise, we will have a conflict with CONOK)

    char headers[][20] = {TR_NAME, CONOK, CONKO, MSG,EXIT_MSG};
    int i=-1, length = sizeof(headers)/sizeof(headers[0]), found = FALSE;

    for(i = 0; i < length; i++) {
        found = (strcasecmp(p.header, headers[i]) == 0)? TRUE: FALSE;
        if(found == TRUE)
            return i;
    }

    return i;
}
void * sendMsg (Control *c_control) {
    write (c_control->fd_client,&c_control->send_msg->id, 1);
    write (c_control->fd_client,c_control->send_msg->header, strlen(c_control->send_msg->header));
    write (c_control->fd_client,c_control->send_msg->length, strlen(c_control->send_msg->length));
    write (c_control->fd_client,c_control->send_msg->data, strlen(c_control->send_msg->data));
    write (c_control->fd_client,c_control->send_msg->data, strlen(c_control->send_msg->data));

}
void freeControl(Control *_control){
    free(_control->name);
    free(_control->ip);
    free(_control->th_id);
    freeProtocol(_control->rcv_msg);
    freeProtocol(_control->send_msg);

    return;
}
void getMsg(Control *control){
    char* asd_ = calloc(0, 0); // throwaway variable
    int error = read(control->fd_client, &control->rcv_msg->id, 1); // reading id
    if(error < 1){
        control->end_conn = TRUE;
        return;
    }

    readUntil(control->fd_client, &asd_, '['); // reading '['
    memset(control->rcv_msg->header,0,strlen(control->rcv_msg->header)); // empty-ing header

    readUntil(control->fd_client, &control->rcv_msg->header, ']'); // reading header
    read(control->fd_client, control->rcv_msg->length, 2); // reading length

    int _length = atoi(&control->rcv_msg->length[0]) + 10*atoi(&control->rcv_msg->length[1]);
    memset(control->rcv_msg->data,0,strlen(control->rcv_msg->data)); // empty-ing data
    control->rcv_msg->data = realloc(control->rcv_msg->data, _length); // length bytes long (pretty obvious isn't it)
    read(control->fd_client, control->rcv_msg->data, _length);

    free(asd_);
    return;
}
//for creating the big server (main one)/listener
void *openServer (void *_control) {
    Control *control = (Control*)_control;
    int listenfd = 0, connfd = 0, g_listenfd;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];

    /* creates an UN-named socket inside the kernel and returns
     * an integer known as socket descriptor
     * This function takes domain/family as its first argument.
     * For Internet family of IPv4 addresses we use AF_INET
     */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    g_listenfd = listenfd;
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
    while(1)
    {
        /* In the call to accept(), the server is put to sleep and when for an incoming
         * client request, the three way TCP handshake* is complete, the function accept()
         * wakes up and returns the socket descriptor representing the client socket.
         */
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        // TODO: Add something here to stop looping when getting a signal. "break_while" could be reused
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
        pthread_create(&(child_ctrls[actual_thread].th_id), NULL, newConnection, (void*)&(child_ctrls[actual_thread]) );
        actual_thread++;

        sleep(1);
    }

    // Freeing memory:
    int i;
    for(i=0; i<num_of_threads; i++){
        pthread_kill(*child_ctrls[i].th_id, SIGTERM);
        sleep(2);
        pthread_join(*child_ctrls[i].th_id, NULL);
        /* TODO: instead of doing this, kill the child threads on parent (don't wait for global variables) and start
         *      the closing routine here (sending the DC packet to the client
         */
        close(child_ctrls[i].fd_client);
        freeControl(&child_ctrls[i]);
        //free(child_ctrls[i]);
    }
    free(child_ctrls);

    // Close socket
}
// this one is to create the small "servers"
void * newConnection (void *_control) {
    Control *control = (Control*)_control;

    void (*func_array[])(control) = {trNameFunc, conOKFunc, conKOFunc, msgFunc, endConn};

    while(break_listener == FALSE) {
        getMsg(control);
        int option = parseHeader(*(control->rcv_msg));
        if(option >= 0){
            if(option > 4){
                myprint("ERROR: Wrong command/input\n");
                continue;
            }
            func_array[option](control);
        }else
            control->end_conn = TRUE;

        if(control->end_conn == TRUE){
            myprint("exit child\n");
            // TODO: start server-client disconnection routine (?
         /*   fillProtocol(control->send_msg, '6', "[CONOK]", "0", " ");
            myprint("fce1\n");
            sendMsg(control);
            myprint("fce1\n");*/
            break;
        }
        resetProtocol(control->rcv_msg);
    }

    myprint("exiting th child \n");
}
