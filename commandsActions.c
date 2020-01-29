//
// Created by Olívia Rey Pellicer on 25/10/2019.
//

#include <commandsActions.h>
#include <fileManager.h>

void handle_sigpipe(){
    closeConn();
}

void closeConn(){
    close(conn_fd);
    conn_fd = 0;
}
int sendtofd(Protocol p, int fd){
    int error = 0;

    error += write (fd, &p.id, 1);
    error += write (fd, p.header, strlen(p.header));
    error += write (fd, p.length, strlen(p.length));
    error += write (fd, p.data, strlen(p.data));

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

    conn_fd = sockfd;

    return 1;
}

Protocol* readMsg(){
    Protocol *p = newProtocol();
    char * _ = calloc(0,0);             //throwable
    if(read(conn_fd, &p->id, 1) < 1 )   // reading id
        return NULL;

    readUntil(conn_fd, &_, '[');        // reading '['
    readUntil(conn_fd, &p->header, ']'); // reading header
    read(conn_fd, p->length, 2);        // reading length
    int _length = atoi(p->length);
    p->data = realloc(p->data, _length); // length bytes long (pretty obvious isn't it)
    char *_data = malloc(_length);
    read(conn_fd, _data, _length);
    strcpy(p->data, _data);

    free(_);

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
        char **ports_arr = malloc(sizeof(char**) * 1);
        ports_arr[0] = malloc(sizeof(char*) * 1);
        int i = 0;
        char _[1]; // throwable variable
        while(readUntil(fd[0], &ports_arr[i], '\n') > 0) {
            i++;
            ports_arr = realloc(ports_arr, sizeof(char*) * (i+1));
            ports_arr[i] = malloc(sizeof(char*) * 1);
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
    int j = 0, lenUsername = 0;// lenAudio  = 0;
    //char aux[BUFF_SIZE], text[BUFF_SIZE],  port_s[BUFF_SIZE], user2[BUFF_SIZE], audio[BUFF_SIZE];
    char *user2, *audio, *text, *audio_name;
    user2 = calloc(0,0);
    audio = malloc(0);
    text = malloc(0);
    Protocol * client_protocol, *server_protocol;
    // int port; ??
    switch (i) {
        case 0: // SHOW CONNECTIONS
            // child process because return value zero
           //int saved_stdout = dup(1);
            show_connections();
            break;
        case 1: // CONNECT
            if(conn_fd > 0) // checking if there is any connection already established
                break;

            j = strlen(CONNECT) + 1;
            if (user[j-1] == ' ')
                lenUsername = sreadUntil(&user[j], &user2, '\0');
            else
                break;
            if(strcmp(user2, FILEDATA.port) == 0)
                break;

            if(connectPort(atoi(user2)) < 0){
                write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));
                break;
            }

            Protocol *p = newProtocol();
            char id = '1';
            char *header = "[TR_NAME]";
            char *data = FILEDATA.user_name;
            fillProtocol(p, id, header, data); // (Protocol *_p, char _id, char * _header, char * _length, char *_data)

            sendtofd(*p, conn_fd);
            // write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));
            /* TODO: Refactor this */
            sleep(0.2);
            server_protocol = readMsg();

            if(server_protocol == NULL) {
                write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));
                closeConn();
                break;
            }
            conn_username = realloc(conn_username, strlen(server_protocol->data));
            strcpy(conn_username, server_protocol->data);
            char connectedMsg[100];
            sprintf(connectedMsg, "%s connected: %s\n", user2, conn_username);
            myprint(connectedMsg);

            freeProtocol(p);
            freeProtocol(server_protocol);
            break;
        case 2: // SAY
            client_protocol = newProtocol();
            char * _ = calloc (0,0); //thorwable variable
            j = strlen(SAY) + 1;
            if (user[j-1] == ' ')
                j+= sreadUntil(&user[j], &user2, ' ');// we're reading the name of the user
            else
                break;

            j += sreadUntil(&(user[j]), &_, '"'); //we're reading the trash until the first character of the text
            if(user[lenUsername+2] != ' '){
                sreadUntil(&(user[j]), &text, '"'); //we're text
            }else{
                write(1, "error1\n", strlen("error\n"));
                break;
            }
            free(_);
            myprint("got it\n");
            printf("text: %s\n", text);
            fillProtocol(client_protocol, '2', "[MSG]", text);
            if(sendtofd(*client_protocol, conn_fd) < 0 || conn_fd <= 0)
                myprint(COULDNTSEND);

            freeProtocol(client_protocol);
            break;
       /* case 3:
            j = strlen(BROADCAST);
            while (user[j] != '"') {
                j++;
            }
            k = 0;
            while (user[j] != '"') {
                text[k] = user[j];
                j++;
                k++;
            }
            text[k] = '\0';
            write(1, NOCONNECTIONS, strlen(NOCONNECTIONS));

            break;*/
        case 4: // SHOWAUDIOS
            client_protocol = newProtocol();
            fillProtocol(client_protocol, '4', "[SHOW_AUDIOS]", " ");
            audio_name = malloc(0);

            if(sendtofd(*client_protocol, conn_fd) < 0 || conn_fd <= 0)
                myprint(COULDNTSEND);

            freeProtocol(client_protocol);
            // waiting answer with list of audios:
            server_protocol = readMsg();
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
            printf("here\n");
            audio_name = malloc(0);
            if(lenUsername > 0 && user[lenUsername+2] != ' '){
                sreadUntil2(&(user[j+lenUsername]), &audio_name, ' ', '\0');
            }else{
                write(1, "error\n", strlen("error\n"));
            }
            printf("here2\n");
            audio_name[strlen(audio_name)-1] = 0;
            printf("audio_name: %s\n",audio_name);
            client_protocol = newProtocol();
            fillProtocol(client_protocol, '5', "[AUDIO_RQST]", audio_name);
            sendtofd(*client_protocol, conn_fd);

            int file_fd = open(audio_name, O_CREAT | O_WRONLY |  O_TRUNC, S_IRWXU);

            while(1) {
                server_protocol = readMsg();
                if(strcasecmp(server_protocol->header, "[EOF]") == 0)
                    break;
                else if(strcasecmp(server_protocol->header, "[AUDIO_KO]") == 0){
                    myprint("ERROR: Wrong audio name\n");
                    break;
                }
                printProtocol(*server_protocol);
                write(file_fd, server_protocol->data, atoi(server_protocol->length));
                resetProtocol(server_protocol);
            }
            close(file_fd);
            printf("Done\n");
            freeProtocol(client_protocol);
            freeProtocol(server_protocol);
            free(audio_name);

            break;
    }
    free(audio);
    free(user2);
    free(text);
}