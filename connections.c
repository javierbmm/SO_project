//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#include "connections.h"
#include <types.h>
Protocol p;
void fillProtocol(Protocol *_p, char _id, char * _header, char * _length, char *_data){
    // empty-ing
    memset(_p->header,0,strlen(_p->header)); // empty-ing header
    memset(_p->header,0,strlen(_p->length)); // empty-ing header
    memset(_p->header,0,strlen(_p->data)); // empty-ing header

    _p->header = realloc(_p->header, strlen(_header));
    _p->data   = realloc(_p->data, strlen(_data));
    _p->length = realloc(_p->length, strlen(_length));

    // filling
    _p->id = _id;
    strcpy(_p->header, _header);
    strcpy(_p->data, _data);
    strcpy(_p->length, _length);/*
    *(_p->header) = _header;
    *(_p->data) = _data;
    *(_p->length) = _length;*/

}
void *trNameFunc (Control *c_control) {
    c_control->name = realloc(c_control->name, c_control->rcv_msg->length);
    strcpy(c_control->name, c_control->rcv_msg->data);
    msgOKFunc(c_control);
}

void *conOkFunc (Control *c_control) {
    write(c_control->fd_client, "1", 1);
    write(c_control->fd_client, "[CONOK]", strlen("[CONOK]"));
    int length = strlen(c_control->name);
    char clength[3];
    sprintf(clength, "%d", length);
    write(c_control->fd_client, clength, 2); // CAREFUL
    write(c_control->fd_client, c_control->name, strlen(c_control->name));

}
void *conKOFunc (Control *c_control) {
    Protocol p_new;
    p_new.header = "[CONKO]";
    sendMsg(c_control);
}

////////
/* TODO: */
void *msgFunc (Control *c_control) {
    //sendMsg(c_control);
}
void *msgOKFunc (Control *c_control) {
    Protocol *msgOk = newProtocol();
    header = c_control->rcv_msg->header;
    data = c_control->rcv_msg->data;
    id = c_control->rcv_msg->id;
    fillProtocol(msgOk, id, header, length, data);
    freeProtocol(c_control->send_msg);
    c_control->send_msg = msgOk;
    sendMsg(c_control);
}
void *broadcastFunc (Control *c_control) {

}
void *showAudiosFunc (Control *c_control) {

}
void *listAudiosFunc (Control *c_control) {

}
void *audioRqstFunc (Control *c_control) {

}
void *endConn (Control *c_control){
    c_control->end_conn = TRUE;
}
////////

//we parse the header to know which action we have to do
int parseHeader (Protocol p) {
    //TODO COMPLETAR
    char headers[][20] = {TR_NAME, CONOK, CONKO, MSG, MSGOK, EXIT_MSG};
    int i, length = sizeof(headers)/sizeof(headers[0]), found = FALSE;

    for(i = 0; i < length; i++) {
        found = (strcasecmp(p.header, headers[i]) == 0)? TRUE: FALSE;
        if(found == TRUE)
            return i;
    }
}
void * sendMsg (Control *c_control) {
    write (c_control->fd_client,&c_control->send_msg->id, 1);
    write (c_control->fd_client,c_control->send_msg->header, strlen(c_control->send_msg->header));
    write (c_control->fd_client,c_control->send_msg->length, strlen(c_control->send_msg->length));
    write (c_control->fd_client,c_control->send_msg->data, strlen(c_control->send_msg->data));
}

void freeProtocol(Protocol *_p){
    free(_p->header);
    free(_p->length);
    free(_p->data);
    free(_p);
}

void freeControl(Control *_control){
    free(_control->name);
    free(_control->ip);
    free(_control->th_id);
    freeProtocol(_control->rcv_msg);
    freeProtocol(_control->send_msg);

    return;
}

Protocol *newProtocol(){
    Protocol *_protocol;
    printf("sizeofprotocol: %d\n", sizeof(_protocol));
    _protocol = (Protocol*)malloc(sizeof(Protocol));
    printf("made a new protocol\n");
    _protocol->header = calloc(0,0);
    _protocol->data = (char*)malloc(1);
    _protocol->id = 0;
    _protocol->length = malloc(2);
    return _protocol;
}

void getMsg(Control *control){
    char* asd_ = calloc(0, 0); // throwaway variable

    read(control->fd_client, &control->rcv_msg->id, 1); // reading id
    myprint("1\n");

    readUntil(control->fd_client, &asd_, '['); // reading '['
    myprint("2\n");

    memset(control->rcv_msg->header,0,strlen(control->rcv_msg->header)); // empty-ing header
    myprint("3\n");
    readUntil(control->fd_client, &control->rcv_msg->header, ']'); // reading header
    read(control->fd_client, control->rcv_msg->length, 2); // reading length
    myprint("4\n");

    int _length = atoi(control->rcv_msg->length);
    memset(control->rcv_msg->data,0,strlen(control->rcv_msg->data)); // empty-ing data
    myprint("5\n");
    control->rcv_msg->data = realloc(control->rcv_msg->data, _length); // length bytes long (pretty obvious isn't it)
    myprint("6\n");
    read(control->fd_client, control->rcv_msg->data, _length);
    myprint("freeing\n");

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
    /*if (head == NULL) {
        return 1;
    }
    head->data = NULL;
    head->next = NULL;
*/
    myprint("im here\n");
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
        myprint("oh hey\n");

        child_ctrls[actual_thread].rcv_msg = newProtocol();
        child_ctrls[actual_thread].send_msg = newProtocol();
        myprint("creating a thread\n");
        pthread_create(&(child_ctrls[actual_thread].th_id), NULL, newConnection, (void*)&(child_ctrls[actual_thread]) );
        myprint("created anther thread\n");
        actual_thread++;

        sleep(1);
    }

    // Freeing memory:
    int i;
    for(i=0; i<num_of_threads; i++){
        pthread_kill(*child_ctrls[i].th_id, SIGTERM);
        sleep(2);
        pthread_join(*child_ctrls[i].th_id, NULL);

        freeControl(&child_ctrls[i]);
        //free(child_ctrls[i]);
    }
    free(child_ctrls);

    // Close socket
    close(listenfd);
}


// this one is to create the small "servers"
void * newConnection (void *_control) {
    myprint("inside newConnection()\n");
    Control *control = (Control*)_control;
    myprint("oh hey2\n");

    void (*func_array[])(control) = {trNameFunc, conOkFunc, conKOFunc, endConn};
    myprint("func array\n");
    while(break_listener == FALSE) {
        getMsg(control);
        myprint("gotmsg\n");
        printf("- header: %s\n- data: %s\n",control->rcv_msg->header, control->rcv_msg->data);
        int option = parseHeader(*(control->rcv_msg));
        printf("option: [%d]\n", option);
        if(option >= 0)
            *func_array[option];

        if(control->end_conn == TRUE){
            // TODO: start server-client disconnection routine (?
            break;
        }
    }
}
