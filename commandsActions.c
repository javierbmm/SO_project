//
// Created by Olívia Rey Pellicer on 25/10/2019.
//

#include <commandsActions.h>
#include <fileManager.h>

void handle_sigpipe(){
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
    read(conn_fd, &p->id, 1);            // reading id
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

// Get the command and process it
void getCommand(int i, char * user) {
    int j = 0, lenUsername = 0, lenAudio = 0;
    //char aux[BUFF_SIZE], text[BUFF_SIZE],  port_s[BUFF_SIZE], user2[BUFF_SIZE], audio[BUFF_SIZE];
    char *user2, *audio, *port_s, *text;
    user2 = calloc(0,0);
    audio = malloc(0);
    text = malloc(0);
    Protocol * client_protocol;
    pid_t child_pid;
    // int port; ??
    switch (i) {
        case 0: // SHOW CONNECTIONS
            // child process because return value zero
           //int saved_stdout = dup(1);
            child_pid = fork();
            if (child_pid == 0) {
                char *args[] = {"show_connections.sh", FILEDATA.ip, FILEDATA.init_port, FILEDATA.final_port, NULL};
                /*char *args[5];
                args[0] = "show_connections.sh";
                args[1] = "127.0.0.1";
                args[2] = "8010";
                args[3] = "8020";
                args[4] = NULL;*/
                int ports_fd = open("ports.txt", O_CREAT | O_WRONLY |  O_TRUNC, S_IRWXU);
                dup2(ports_fd, STDOUT_FILENO);
                execv(args[0], args);
                myprint("EXECVP ERROR\n");
                // parent process because return value non-zero.
            }else{
                waitpid(child_pid, NULL, 0);
                char **ports_arr = malloc(1);
                ports_arr[0] = malloc(1);
                int ports_fd = open("ports.txt", O_RDONLY);
                int i = 0;
                while(readUntil(ports_fd, &ports_arr[i], '\n') > 0) {
                    i++;
                    ports_arr = realloc(ports_arr, i);
                    ports_arr[i] = malloc(1);
                    skipDelimiter(ports_fd, '\n');
                }
                int j = 0;
                char *port = malloc(1);
                char available_connections_msg[30];
                sprintf(available_connections_msg, "%d connections available\n", i);
                myprint(available_connections_msg);
                while(j < i){
                    sreadUntil(&ports_arr[j][5], &port, ' ');
                    myprint(port);
                    myprint("\n");
                    free(ports_arr[j]);
                    j++;
                }
                free(ports_arr);
                free(port);
                close(ports_fd);
            }
            break;
        case 1: // CONNECT
            j = strlen(CONNECT) + 1;
            if (user[j-1] == ' ')
                lenUsername = sreadUntil(&user[j], &user2, '\0');
            else
                break;

            conn_fd = atoi(user2);
            if(connectPort(conn_fd) < 0){
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
            Protocol * rcv_msg = readMsg();
            conn_username = realloc(conn_username, strlen(rcv_msg->data));
            strcpy(conn_username, rcv_msg->data);
            freeProtocol(rcv_msg);
            break;
        case 2: // SAY
            client_protocol = newProtocol();
            char * _ = calloc (0,0); //thorwable variable
            int lenText = 0;
            j = strlen(SAY) + 1;
            if (user[j-1] == ' ')
                j+= sreadUntil(&user[j], &user2, ' ');// we're reading the name of the user
            else
                break;
            j += sreadUntil(&(user[j]), &_, '"'); //we're reading the trash until the first character of the text
            if(user[lenUsername+2] != ' '){
                lenText = sreadUntil(&(user[j]), &text, '"'); //we're text
            }else{
                write(1, "error1\n", strlen("error\n"));
                break;
            }
            free(_);
            fillProtocol(client_protocol, '2', "[MSG]", text);
            if(sendtofd(*client_protocol, conn_fd) < 0 || conn_fd <= 0)
                myprint(COULDNTSEND);

            myprint("after sending msg\n");
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

            break;
        case 4:
            j = strlen(SHOWAUDIOS) + 1;
            k = 0;
            while (user[j] > '0' && user[j] < '9') {
                user2[k] = user[j];
                k++;
                j++;
            }
            aux[k] = '\0';
            write(1, NOCONNECTIONS, strlen(NOCONNECTIONS));

            break;*/
        case 5:
            j = strlen(DOWNLOAD) + 1;
            //printf()
            if (user[j-1] == ' ')
                lenUsername = sreadUntil(&user[j], &user2, ' ');
            else
                break;

            if(lenUsername > 0 && user[lenUsername+2] != ' '){
                lenAudio = sreadUntil(&(user[j+lenUsername+1]), &audio, '\0');
            }else{
                write(1, "error\n", strlen("error\n"));
            }
            user2 = realloc(user2, ++lenUsername);
            user2[lenUsername-1] = '\0';
            audio = realloc(audio, ++lenAudio);
            audio[lenAudio-1] = '\0';
            write(1, NOCONNECTIONS, strlen(NOCONNECTIONS));

            break;
    }
    free(audio);
    free(user2);
}