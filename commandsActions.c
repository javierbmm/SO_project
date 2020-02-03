//
// Created by Olívia Rey Pellicer on 25/10/2019.
//

#include <commandsActions.h>
#include <fileManager.h>

void handle_sigpipe(){
    return;
}

void closeConn(){
    return;
}

serverConn* newServerConn(int conn_fd, char* username, int port, int status){
    serverConn* _sc = calloc(1, sizeof(_sc));

    _sc->conn_username = calloc(strlen(username)+1, 1);
    strcpy(_sc->conn_username, username);
    _sc->conn_username[strlen(username)] = 0;
    _sc->conn_fd = conn_fd;
    _sc->port = port;
    _sc->is_connected = status;

    return _sc;
}
void closeServConn(serverConn* _sc){
    free(_sc->conn_username);
    close(_sc->conn_fd);
    _sc->conn_fd = 0;
    _sc->port = 0;
    _sc->is_connected = FALSE;
}

int searchByName(char* username){
    int i = 0;

    for(i=0; i<num_conn; i++){
        if(strcmp(username, arrConns[i]->conn_username) == 0)
            return i;
    }

    return -1;
}

int searchByPort(const int port){
    int i = 0;
    for(i=0; i<num_conn; i++){
        if(arrConns[i]->port == port)
            return i;
    }

    return -1;
}

int isConnected(const int port){
    int i = 0;
    for(i=0; i<num_conn; i++){
        if(arrConns[i]->port == port){
            if(arrConns[i]->is_connected == TRUE)
                return TRUE;
        }
    }

    return FALSE;
}
int sendtofd(Protocol p, int fd){
    int error = 0;

    error += write (fd, &p.id, 1);
    error += write (fd, p.header, strlen(p.header));
    error += write (fd, p.length, 2);
    error += write (fd, p.data, atoi(p.length));

    return error;
}

int connectPort(int port){
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    /* a socket is created through call to socket() function */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(FILEDATA.ip);
    serv_addr.sin_port = htons(port);


    /* Information like IP address of the remote host and its port is
     * bundled up in a structure and a call to function connect() is made
     * which tries to connect this socket with the socket (IP address and port)
     * of the remote host
     */
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        return -1;
    }

    /* Once the sockets are connected, the server sends the data (date+time)
     * on clients socket through clients socket descriptor and client can read it
     * through normal read call on the its socket descriptor.
     */

    return sockfd;
}

Protocol* readMsg(int conn_fd){
    Protocol *p = newProtocol();
    char * _ = calloc(1,1); //throwable

    if(read(conn_fd, &p->id, sizeof(p->id)) < 1 )   // reading id
        return NULL;

    readOnlyUntil(conn_fd, &_, '[');        // reading '['
    readOnlyUntil(conn_fd, &p->header, ']'); // reading header
    read(conn_fd, &p->length[0], 1);        // reading length
    read(conn_fd, &p->length[1], 1);        // reading length

    int _length = myAtoi(p->length,2);
    p->data = realloc(p->data, _length); // length bytes long (pretty obvious isn't it)
    memset(p->data, 0, _length);
    read(conn_fd, p->data, _length);
    //strncpy(p->data, _data, _length);

    free(_);
   // free(_data);

    return p;
}

void show_connections(){
    pid_t child_pid;
    int fd[2];
    pipe(fd);
    // fd[0] -> for using read end
    // fd[1] -> for using write end

    child_pid = fork();
    if (child_pid == 0) {
        char *args[] = {
            "show_connections.sh",
            FILEDATA.ip,
            FILEDATA.init_port,
            FILEDATA.final_port,
            NULL
        };
        dup2(fd[1], 1);
        execvp(args[0], args);
        myprint("EXECVP ERROR\n");
        // parent process because return value non-zero.
    }else{
        wait(NULL);
        char **ports_arr = calloc(1, sizeof(char**));
        ports_arr[0] = calloc(1, sizeof(char*));
        int i = 0;
        char _[1]; // throwable variable
        while(readUntil(fd[0], &ports_arr[i], '\n') > 0) {
            i++;
            ports_arr = realloc(ports_arr, sizeof(char*) * (i+1));
            ports_arr[i] = calloc(1, sizeof(char*));
            //skipDelimiter(fd[0], '\n');
            read(fd[0], _, 1);  // reading that '\n' since skipDelimiter doesn't work because it uses lseek function
                                // which doesn't work with pipes.
        }
        int j = 0;
        char *port = malloc(1);
        char available_connections_msg[30];
        sprintf(available_connections_msg, "%d connections available\n", i-1);
        myprint(available_connections_msg);
        while(j < i){
            sreadUntil(&ports_arr[j][5], &port, ' ');
            if(strcmp(port, FILEDATA.port) != 0){ // print only if is not my own port
                myprint(port);
                myprint("\n");
            }
            free(ports_arr[j]);
            j++;
        }

        free(ports_arr);
        free(port);
        close(fd[0]);
        close(fd[1]);
    }
}
// Get the command and process it
void getCommand(int i, char * user) {
    int j = 0, lenUsername = 0, lenText = 0, bytesSent = 0, bytesToSend = 0;
    int it=0;

    //char aux[BUFF_SIZE], text[BUFF_SIZE],  port_s[BUFF_SIZE], user2[BUFF_SIZE], audio[BUFF_SIZE];
    char *user2, *audio, *text, *audio_name, buffer[100] = {0};
    user2 = calloc(1, 1);
    audio = calloc(1, 1);
    text = calloc(1, 1);
    audio_name = calloc(1,1);
    Protocol * client_protocol, *server_protocol;
    // int port; ??
    switch (i) {
        case 0: // SHOW CONNECTIONS
            // child process because return value zero
           //int saved_stdout = dup(1);
            show_connections();
            break;
        case 1: // CONNECT
            j = strlen(CONNECT) + 1;
            if (user[j-1] == ' ')
                lenUsername = sreadUntil(&user[j], &user2, '\0');
            else
                break;
            if(strcmp(user2, FILEDATA.port) == 0){
                write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));
                break;
            }
            if(isConnected(atoi(user2)) == TRUE){
                write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));
                break;
            }
            int socket_fd = 0;
            if((socket_fd = connectPort(atoi(user2)) )< 0){
                write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));
                break;
            }

            Protocol *p = newProtocol();
            char id = '1';
            char *header = "[TR_NAME]";
            char *data = FILEDATA.user_name;
            fillProtocol(p, id, header, data); // (Protocol *_p, char _id, char * _header, char * _length, char *_data)

            sendtofd(*p, socket_fd);
            sleep(0.2);
            server_protocol = readMsg(socket_fd);

            if(server_protocol == NULL) {
                write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));
                closeConn();
                break;
            }

            num_conn++;
            arrConns = realloc(arrConns, num_conn);
            arrConns[num_conn] = newServerConn(socket_fd, server_protocol->data, atoi(user2), TRUE);
            printf("data: %s | name: %s\n", server_protocol->data, arrConns[num_conn]->conn_username);
            fflush(stdout);

            char connectedMsg[100];
            sprintf(connectedMsg, "%s connected: %s\n", user2, arrConns[num_conn]->conn_username);
            myprint(connectedMsg);

            freeProtocol(p);
            freeProtocol(server_protocol);
            break;
        case 2: // SAY
            j = strlen(SAY) + 1;
            char * _ = calloc (1,1); //thorwable variable
            if (user[j-1] == ' ')
                j+= sreadUntil(&user[j], &user2, ' ');// we're reading the name of the user
            else
                break;

            j += sreadUntil(&(user[j]), &_, '"'); // we're reading the trash until the first character of the text
            if(user[lenUsername+2] != ' '){
                lenText = sreadUntil(&(user[j]), &text, '"'); //we're text
            }else{
                write(1, "error1\n", strlen("error\n"));
                break;
            }
            free(_);

            it=0;

            if((it=searchByName(user2)) < 1){
                if(arrConns[it]->is_connected == FALSE) {
                    myprint(COULDNTSEND);
                    break;
                }
            }
            client_protocol = newProtocol();
            text = realloc(text, ++lenText);
            text[lenText-1] = '\0';

            while(bytesSent < lenText) {
                bytesToSend = lenText - bytesSent;

                // (bytesSent == 0? 0: bytesSent-1) => checking init of bytesSent
                fillProtocol(client_protocol, '2', "[MSG]", &text[bytesSent]);
                if (sendtofd(*client_protocol, arrConns[it]->conn_fd) < 0) {
                    myprint(COULDNTSEND);
                    freeProtocol(client_protocol);
                    break;
                }
                if(bytesToSend > 99){ // maximum size of buffer (2 digits of length = 99 characters)
                    bytesSent += 99;
                }else{
                    bytesSent = lenText;
                }
            }

            freeProtocol(client_protocol);
            break;
        case 3:
            j = strlen(BROADCAST) + 1;
            char * aux_ = calloc (0,0); //thorwable variable
            j += sreadUntil(&(user[j]), &aux_, '"'); //we're reading the trash until the first character of the text
            if(user[lenUsername+2] != ' '){
                lenText = sreadUntil(&(user[j]), &text, '"'); //we're text
            }else{
                write(1, "error1\n", strlen("error\n"));
                free(aux_);
                break;
            }
            free(aux_);

            client_protocol = newProtocol();
            text = realloc(text, ++lenText);
            text[lenText-1] = '\0';

            for (int i = 0; i< num_conn; i++) {
                if(arrConns[i]->is_connected == FALSE)
                    continue;
                while (bytesSent < lenText) {
                    bytesToSend = lenText - bytesSent;
                    // (bytesSent == 0? 0: bytesSent-1) => checking init of bytesSent
                    fillProtocol(client_protocol, '3', "[BROADCAST]", &text[bytesSent]);
                    if (sendtofd(*client_protocol, arrConns[i]->conn_fd) < 0) {
                        myprint(COULDNTSEND);
                        break;
                    }

                    if (bytesToSend > 99) { // maximum size of buffer (2 digits of length = 99 characters)
                        bytesSent += 99;
                    } else {
                        bytesSent = lenText;
                    }
                }
            }

            freeProtocol(client_protocol);
            write(1, NOCONNECTIONS, strlen(NOCONNECTIONS));

            break;
        case 4: // SHOWAUDIOS
            j = strlen(SAY) + 1;
            char * aux2_ = calloc (1,1); //thorwable variable
            if (user[j-1] == ' ')
                j+= sreadUntil(&user[j], &user2, ' ');// we're reading the name of the user
            else
                break;

            j += sreadUntil(&(user[j]), &aux2_, '"'); //we're reading the trash until the first character of the text
            if(user[lenUsername+2] != ' '){
                lenText = sreadUntil(&(user[j]), &text, '"'); //we're text
            }else{
                write(1, "error1\n", strlen("error\n"));
                break;
            }
            free(aux2_);
            it = 0;
            if((it=searchByName(user2))< 1){
                if(arrConns[it]->is_connected == FALSE) {
                    myprint(COULDNTSEND);
                    break;
                }
            }
            client_protocol = newProtocol();
            fillProtocol(client_protocol, '4', "[SHOW_AUDIOS]", " ");

            if(sendtofd(*client_protocol, arrConns[it]->conn_fd) < 0)
                myprint(COULDNTSEND);

            freeProtocol(client_protocol);
            // waiting answer with list of audios:
            server_protocol = readMsg(arrConns[it]->conn_fd);
            if(server_protocol == NULL)
                break;

            // Now we have to read the list of audios stored as [audio_name1\n audio_name2\n ... audio_nameN]
            int readLen = 1;
            audio_name = malloc(0);
            while(atoi(server_protocol->length)> readLen){
                readLen += sreadUntil2(&server_protocol->data[readLen], &audio_name, '\\', ']');
                myprint(audio_name);
                myprint("\n");
            }
            freeProtocol(server_protocol);
            free(audio_name);

            break;
        case 5:
            printf("Download\n");
            j = strlen(DOWNLOAD) + 1;
            //printf()
            if (user[j-1] == ' ')
                lenUsername = sreadUntil(&user[j], &user2, ' ');
            else
                break;

            if((it=searchByName(user2))< 1){
                if(arrConns[it]->is_connected == FALSE) {
                    myprint("ERROR: Wrong user\n");
                    break;
                }
            }
            audio_name =  calloc(1, 1);
            if(lenUsername > 0 && user[lenUsername+2] != ' '){
                sreadUntil2(&(user[j+lenUsername]), &audio_name, ' ', '\0');
            }else{
                write(1, "error\n", strlen("error\n"));
            }
            //audio_name[strlen(audio_name)-1] = 0;
            client_protocol = newProtocol();
            fillProtocol(client_protocol, '5', "[AUDIO_RQST]", audio_name);
            sendtofd(*client_protocol, arrConns[it]->conn_fd);

            int file_fd = open(audio_name, O_CREAT | O_WRONLY |  O_TRUNC, S_IRWXU);
            printf("audio: %s\n",audio_name);
            fflush(stdout);
            int i = 0;
            while(1) {
                i++;
                server_protocol = readMsg(arrConns[it]->conn_fd);
                if(strcasecmp(server_protocol->header, "EOF") == 0){
                    close(file_fd);
                    memset(buffer,0, sizeof(buffer));
                    // Checking file integrity with md5sum keys
                    if( check_md5sum(server_protocol->data, audio_name) == FALSE) { // file ok
                        sprintf(buffer, "[%s] %s downloaded\n", arrConns[it]->conn_username, audio_name);
                        myprint(buffer);
                    }else{  // file corrupted
                        sprintf(buffer, "[%s] %s error. Try again\n", arrConns[it]->conn_username, audio_name);
                        myprint(buffer);
                    }
                    break;
                }
                else if(strcasecmp(server_protocol->header, "AUDIO_KO") == 0){
                    close(file_fd);
                    remove(audio_name);
                    myprint("ERROR: Wrong audio name\n");
                    break;
                }
                int written  = write(file_fd, server_protocol->data, myAtoi(server_protocol->length, 2));
                // This code is only for testing purposes. Please do not erase it.
                if(CHUNK_SIZE != written) {
                    myprint("hey!\n");
                    printf("this <%s>|<%d> should be %d\n In line %d: ",
                             server_protocol->length, myAtoi(server_protocol->length, 2),
                             CHUNK_SIZE, i);
                    fflush(stdout);
                    printProtocol(*server_protocol);
                }
                freeProtocol(server_protocol);
            }

            freeProtocol(client_protocol);
            freeProtocol(server_protocol);

            break;
    }

    memset(audio, 0, strlen(audio));
    memset(text, 0, strlen(text));
    memset(user2, 0, strlen(user2));
    memset(audio_name, 0, strlen(audio_name));
    free(audio);
    free(user2);
    free(text);
    free(audio_name);

}