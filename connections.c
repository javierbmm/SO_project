//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#include "connections.h"
#include <types.h>
#include <stdlib.h>
#include <dirent.h>

Protocol p;
/*
 * This function is used to store the name of the client in the Control
 * structure for the server to know it. It first creates the needed memory
 * dynamically and then using a strcpy copies into the control structure the name
 * stored in the data of the protocol receivd by the server.
 */
void trNameFunc (Control *c_control) {
    c_control->name = realloc(c_control->name, strlen(c_control->rcv_msg->data));
    strcpy(c_control->name, c_control->rcv_msg->data);

    conOKFunc(c_control);
}
/*
 * This function is in charge of sending the server to the client the
 * information saying that the connection was succesfull. To do so we fill a protocol,
 * the id of the message, the header and the data. After that we call sendMsg() function
 * to send the message to the client.
 */
void conOKFunc (Control *c_control) {
    fillProtocol(c_control->send_msg, '1', "[CONOK]", FILEDATA.user_name);
    sendMsg(c_control);
}
/*
 * This function is the one in charge of sending the information saying the connection wasn't succesfull.
 */
void conKOFunc (Control *c_control) {
    fillProtocol(c_control->send_msg, '1', "[CONKO]", "");
    sendMsg(c_control);
}
/*
 * This function is the one in charge of printing the message received by the client.
 * To do so it uses the \033 command in our printf function. We first move the cursor un row up with
 * the \033[1A and then we move it 180 characters to the left with the input \033[180D. After that we print the
 * user name and print the message received. After that we restore the cursor.
 */
void msgFunc (Control *c_control) {
    myprint("\033[1A\033[180D\n\033[K");  // Move cursor 1 line up and 180 characters left and clear line
    myprint("[");
    myprint(c_control->name);
    myprint("]:");
    write(1, c_control->rcv_msg->data, myAtoi(c_control->rcv_msg->length,2)); //print the message received
    //myprint("\n\0338");  // Restore cursor from saved position and attributes
    myprint("\033[1B\033[180D\n");  // Move cursor 1 line down and 180 characters left and clear line

    write(1, "$", strlen ("$"));
    write(1, FILEDATA.user_name, strlen(FILEDATA.user_name));
    write(1, ":", strlen (":"));
}
/*
 * This function is the one in charge of showing the audios of the given user (the ones it has stored in the folder
 * selected in the config file.
 */
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
 */
int sendChcksum(Control *c_control, char* filedir){
    int success = TRUE;
    char* checksum = get_md5sum(filedir);
    if(checksum == NULL)
        success = FALSE;
    resetProtocol(c_control->send_msg);
    fillProtocol(c_control->send_msg, '5', "[EOF]", checksum);
    sendMsg(c_control);

    free(checksum);

    return success;
}
/**
 * Send a file in chunks of data to the client file descriptor inside c_control struct
 * @param c_control
 * @param filename
 * @return FALSE in case of failure, TRUE in success
 **/
int sendfile(Control *c_control, char* filename){
    ssize_t error = 0;
    char filedir[100];
    sprintf(filedir, "./%s/%s", FILEDATA.audio_folder, filename);

    int f_fd = open(filedir, O_RDONLY);
    if(f_fd < 0)
        return FALSE;

    char BUFFER[CHUNK_SIZE];
    while(1){ // Until end of file.
        memset(BUFFER, 0, CHUNK_SIZE);
        error = read(f_fd, BUFFER, CHUNK_SIZE);
        fflush(stdout);
        if(error <= 0)
            break;

        resetProtocol(c_control->send_msg);
        fillProtocoln(c_control->send_msg, '5', "[AUDIO_RSPNS]", BUFFER, error);
        sendMsg(c_control);
    }
    close(f_fd);

    if(error < 0)
        return FALSE;

    if(sendChcksum(c_control, filedir) == FALSE)
        return FALSE;

    return TRUE;
}

/*
 * This function is the one in charge of requesting to the given user the audios
 */

void audioRqstFunc (Control *c_control) {
    int success = FALSE;
    success = sendfile(c_control, c_control->rcv_msg->data);
    if (success == FALSE) {
        resetProtocol(c_control->send_msg);
        fillProtocol(c_control->send_msg, '5', "[AUDIO_KO]", "");
        sendMsg(c_control);
    }else{
        resetProtocol(c_control->send_msg);
        fillProtocol(c_control->send_msg, '5', "[EOF]", "");
        sendMsg(c_control);
    }
}

/*
 *It sends the given message to the users connected
 */
void broadcastFunc (Control *c_control) {
    msgFunc(c_control);
}
/*
 * This function is the one in charge of letting others know the connection was over (by changing the value of a "flag"
 */
void endConn (Control *c_control){
    c_control->end_conn = TRUE;
}
//we parse the header to know which action we have to do
int parseHeader (Protocol p) {
    //TODO: Complete all the options/commands here and also we have to take into account
    //      the id of the protocol (otherwise, we will have a conflict with CONOK)

    char headers[][20] = {TR_NAME, CONOK, CONKO, MSG, SHOW_AUDIOS, BROADCAST, AUDIO_RQST, EXIT_MSG};
    int i=-1, length = sizeof(headers)/sizeof(headers[0]), found = FALSE;

    for(i = 0; i < length; i++) {
        found = (strcasecmp(p.header, headers[i]) == 0)? TRUE: FALSE;
        if(found == TRUE)
            return i;
    }

    return i;
}

/*
 * This function is in charge of sending the given message to the given user using the Control structure
 */
void sendMsg (Control *c_control) {
    write (c_control->fd_client,&c_control->send_msg->id, sizeof(c_control->send_msg->id));
    write (c_control->fd_client,c_control->send_msg->header, strlen(c_control->send_msg->header));
    write (c_control->fd_client,c_control->send_msg->length, sizeof(c_control->send_msg->length));
    write (c_control->fd_client,c_control->send_msg->data, myAtoi(c_control->send_msg->length,2));
}

/*
 * Function used to free the memory used for the Control structures
 */
void freeControl(Control *_control){
    free(_control->name);
    free(_control->ip);
    freeProtocol(_control->rcv_msg);
    freeProtocol(_control->send_msg);

    return;
}
//when server sends a message
void getMsg(Control *control){
    int error = read(control->fd_client, &control->rcv_msg->id, 1); // reading id
    if(error < 1){
        control->end_conn = TRUE;
        return;
    }
    char* asd_ = calloc(1, 1); // throwaway variable
    readUntil(control->fd_client, &asd_, '['); // reading '['
    //memset(control->rcv_msg->header,0,strlen(control->rcv_msg->header)); // empty-ing header
    readUntil(control->fd_client, &control->rcv_msg->header, ']'); // reading header
    read(control->fd_client, control->rcv_msg->length, 2); // reading length

    int _length = myAtoi(control->rcv_msg->length,2);
    //memset(control->rcv_msg->data,0,strlen(control->rcv_msg->data)); // empty-ing data
    control->rcv_msg->data = realloc(control->rcv_msg->data, _length); // length bytes long (pretty obvious isn't it)
    char *_data = calloc(1, _length+1);
    read(control->fd_client, _data, _length);
    strcpy(control->rcv_msg->data, _data);

    free(_data);
    free(asd_);
    return;
}
//for creating the big server (main one)/listener
void *openServer (void *_control) {
    Control *control = (Control*)_control;
    int connfd = -1;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];

    /* creates an UN-named socket inside the kernel and returns
     * an integer known as socket descriptor
     * This function takes domain/family as its first argument.
     * For Internet family of IPv4 addresses we use AF_INET
     */
    control->fd_server = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(control->ip);
    serv_addr.sin_port = htons(control->port);

    /* The call to the function "bind()" assigns the details specified
     * in the structure 'serv_addr' to the socket created in the step above
     */
    bind(control->fd_server, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    /* The call to the function "listen()" with second argument as 10 specifies
     * maximum number of client connections that server will queue for this listening
     * socket.
     */
    if(listen(control->fd_server, 100) == -1) {
        sleep(2);
        if(listen(control->fd_server, 100) == -1) {
            myprint("ERROR: listen error\n");
            close(control->fd_server);
            exit(EXIT_FAILURE);
        }
    }
    int num_of_threads = 0, actual_thread = 0;

    Control * child_ctrls = calloc (0,0);
    break_listener = FALSE;
    sleep(1);
    while(break_listener == FALSE)
    {
        /* In the call to accept(), the server is put to sleep and when for an incoming
         * client request, the three way TCP handshake* is complete, the function accept()
         * wakes up and returns the socket descriptor representing the client socket.
         */
        connfd = accept(control->fd_server, (struct sockaddr*)NULL, NULL);
        if(break_listener == TRUE) {
            break;
        }
        num_of_threads++;

        // new child
        child_ctrls = realloc(child_ctrls, sizeof(Control)*num_of_threads);
        // Filling child settings
        child_ctrls[actual_thread].fd_client = connfd;
        child_ctrls[actual_thread].port = control->port; // from parent
        child_ctrls[actual_thread].ip = calloc(1, strlen(control->ip)+1);     // from parent
        strcpy(child_ctrls[actual_thread].ip, control->ip);
        child_ctrls[actual_thread].end_conn = FALSE;
        child_ctrls[actual_thread].name = calloc(1,1);

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
        child_ctrls[i].end_conn = TRUE;
        close(child_ctrls[i].fd_client);
        sleep(2);
        pthread_join(child_ctrls[i].th_id, NULL);

        freeControl(&child_ctrls[i]);
    }
    free(child_ctrls);

    return NULL;
}

// this one is to create the small "servers"
void * newConnection (void *_control) {
    Control *control = (Control*)_control;

    void (*func_array[])(Control*) = {
            trNameFunc,
            conOKFunc,
            conKOFunc,
            msgFunc,
            showAudiosFunc,
            broadcastFunc,
            audioRqstFunc,
            endConn
    };

    while(break_listener == FALSE) {
        getMsg(control);
        if(control->end_conn == TRUE){
         
            return NULL;
        }
        int option = parseHeader(*(control->rcv_msg));
        if(option >= 0){
            if(option > 7){
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
