//
// Created by Olívia Rey Pellicer on 25/10/2019.
//

#include <commandsActions.h>
#include <fileManager.h>

void sendtofd(Protocol p, int fd){
    write (1, &p.id, 1);
    write (1, p.header, strlen(p.header));
    write (1, p.length, strlen(p.length));
    write (1, p.data, strlen(p.data));

    write (fd, &p.id, 1);
    write (fd, p.header, strlen(p.header));
    write (fd, p.length, strlen(p.length));
    write (fd, p.data, strlen(p.data));
}

void connectPort(int port){
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    /* a socket is created through call to socket() function */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        myprint("\n Error : Could not create socket \n");
        return;
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
        myprint("\n Error : Connect Failed \n");
        return;
    }

    /* Once the sockets are connected, the server sends the data (date+time)
     * on clients socket through clients socket descriptor and client can read it
     * through normal read call on the its socket descriptor.
     */

    conn_fd = sockfd;

}

Protocol * readMsg(){
    Protocol *p = newProtocol();
    char * _;//throwable
    read(conn_fd, p->id, 1); // reading id
    readUntil(conn_fd, &_, '['); // reading '['
    readUntil(conn_fd, p->header, ']'); // reading header
    read(conn_fd, p->length, 2); // reading length

    int _length = atoi(p->length);
    p.->data = realloc(p->data, p->length); // length bytes long (pretty obvious isn't it)
    read(conn_fd, p->data, p->length);

    free(_);

    return p;
}
// Get the command and process it
void getCommand(int i, char * user) {
    int j = 0, lenUsername = 0, lenAudio = 0;
    //char aux[BUFF_SIZE], text[BUFF_SIZE],  port_s[BUFF_SIZE], user2[BUFF_SIZE], audio[BUFF_SIZE];
    char *user2, *audio;
    user2 = calloc(0,0);
    audio = malloc(0);
    // int port; ??
    switch (i) {
        case 0:
            myprint("Testing...\n");
            // child process because return value zero
            if (fork() == 0) {
                char *args[] = {FILEDATA.ip, FILEDATA.init_port, FILEDATA.final_port};
                execvp("show_connections.sh", args);
                // parent process because return value non-zero.
            }else
                break;
            break;
        case 1:
            j = strlen(CONNECT) + 1;
            if (user[j-1] == ' ')
                lenUsername = sreadUntil(&user[j], &user2, '\n');
            else
                break;

            int port = atoi(user2);
            connectPort(port);
            Protocol *p = newProtocol();
            int id = 1;
            char *header = "[TR_NAME]";
            char *data = FILEDATA.user_name;
            char length[2];
            int _len = strlen(data);
            if(_len >= 10)
                sprintf(length, "%ld", strlen(data));
            else if(_len < 10)
                sprintf(length, "0%ld", strlen(data));

            fillProtocol(p, 1, header, length, data); // (Prototcol *_p, char _id, char * _header, char * _length, char *_data)
            sendtofd(*p, conn_fd);
            write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));
            /* TODO: Refactor this */
            Protocol rcv_msg = readMsg();
            conn_username = realloc (conn_username, strlen(rcv_msg->data));
            strcpy(conn_username, rcv_msg->data);
            freeProtocol(rcv_msg);
            break;
      /*  case 2:
            j = strlen(SAY) + 1;
            k = 0;
            while (user[j] > '0' && user[j] < '9') {
                user2[k] = user[j];
                k++;
                j++;
            }
            user2[k] = '\0';
            while (user[j] != '"') {
                j++;
            }
            j++;
            k = 0;
            while (user[j] != '"') {
                text[k] = user[j];
                j++;
                k++;
            }
            text[k] = '\0';
            write(1, NOCONNECTIONS, strlen(NOCONNECTIONS));

            break;
        case 3:
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
    printf("freeing memory\n");
    free(audio);
    free(user2);
}