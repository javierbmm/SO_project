//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#include "connections.h

Protocol p;

//for creating the big server (main one)/listener
void *openServer (Control * control) {
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    time_t ticks;


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
    serv_addr.sin_addr.s_addr = inet_addr(control.ip);
    serv_addr.sin_port = htons(control.port);

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
    pthread_t *thread_ids = calloc(0,0);
    int num_of_threads = 0, actual_thread = 0;
    Control * childs_ctrls = calloc (0,0);
    while(break_while == 0)
    {
        /* In the call to accept(), the server is put to sleep and when for an incoming
         * client request, the three way TCP handshake* is complete, the function accept()
         * wakes up and returns the socket descriptor representing the client socket.
         */
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        if(break_while != 0)
            break;
        num_of_threads++;
        thread_ids = realloc(thread_ids, num_of_threads);
        void *connfd_aux = &connfd;
        pthread_create(&thread_ids[actual_thread], NULL, menu, connfd_aux);

        actual_thread++;

        sleep(1);
    }

    int i;
    for(i=0; i<num_of_threads; i++){
        pthread_kill(thread_ids[i], SIGTERM);
        sleep(2);
        pthread_join(thread_ids[i], NULL);
    }

    close(listenfd);
}

//this one is to create the small "servers"
void * newConnection (Control control) {
    char *header = calloc(0, 0);
    char *aux = calloc(0, 0);
    char type;
    char *length = malloc(2);
    void (*func_array[]) = {trNameFunc, conOkFunc, conKOFunc}
    read(control.fd_client, &x, 1);
    readUntil(control.fd_client, &aux, '[');
    readUntil(control.fd_client, &header, ']');
    read(control.fd_client, &length, 2);
    char * data = malloc (length);
    read (0, data, length);
    p.id = type;
    p.data = data;
    p.header = header;
    p.length = length;
    int i = parseHeader(p);
    (*func_array[i]);
}

void *trNameFunc () {

}
void *conOkFunc (Control c_control) {
    write(c_control.fd_client, '1', 1);
    write(c_control.fd_client, "[CONOK]", strlen(["CONOK]"));
    int length = strlen(c_control.name);
    write(c_control.fd_client, itoa(length), 2); // CAREFUL
    write(c_control.fd_client, c_control.name, strlen(c_control.name));

}
void *conKOFunc (Control c_control) {
    Protocol p_new;
    p_new.header = "[CONKO]";
    sendMsg(c_control);
}
void *msgFunc (Control c_control) {

}
void *msgOKFunc (Control c_control) {

}
void *broadcastFunc (Control c_control) {

}
void *showAudiosFunc (Control c_control) {

}
void *listAudiosFunc (Control c_control) {

}
void *audioRqstFunc (Control c_control) {

}
//we parse the header to know which action we have to do
int parseHeader (Protocol p) {
    //TODO COMPLETAR
    char commands[][20] = {TR_NAME, CONOK, CONKO, MSG, MSGOK, BROADCAST, SHOW_AUDIOS, LIST_AUDIOS, AUDIO_RQST};
    int i, length = sizeof(commands)/sizeof(commands[0]), found = FALSE;

    for(i = 0; i < length; i++) {
        found = (strcasecmp(user, commands[i]) == 0)? TRUE: FALSE;
        if(found == TRUE)
            return i;
        }
    }
}
char* getMsg () {

}

void * sendMsg (Control c_control) {
    write (c_control.fd_client,c_control.send_message, strlen(c_control.send_message));
}