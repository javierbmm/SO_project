
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

typedef struct Frame {
    char type;
    char length[2];
    char* data;
}frame;

int main()
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0',sizeof(recvBuff));

    /* a socket is created through call to socket() function */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(5000);

    /* Information like IP address of the remote host and its port is
     * bundled up in a structure and a call to function connect() is made
     * which tries to connect this socket with the socket (IP address and port)
     * of the remote host
     */
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    /* Once the sockets are connected, the server sends the data (date+time)
     * on clients socket through clients socket descriptor and client can read it
     * through normal read call on the its socket descriptor.
     */
    char sendBuff[3];
    char auxBuff[1000], aux[3];
    frame msgframe;
    msgframe.type = '0';
    msgframe.length = calloc(0, 2);
    msgframe.data = calloc(0,0);

        do {
            sprintf(auxBuff, "Enter an option\n1.\tWhat time is it?\n2.\tWhat date are we at?\n"
                             "3.\tHow much time has been the server been up?\n4.\tExit.\n");
            write(1, auxBuff, strlen(auxBuff));

            if (read(sockfd, aux, 3) < 0) {
                sprintf(auxBuff, "Error reading option\n");
                write(1, auxBuff, strlen(auxBuff));

                exit(1);
            }
            msgframe.type = *aux;
            sprintf(aux, "%ld", strlen(aux));
            msgframe.length = aux;
            write(sockfd, &msgframe.type, sizeof(msgframe.type));
            while ((n = read(sockfd, &msgframe.type, sizeof(msgframe.type))) > 0) {
                read(sockfd, msgframe.length, sizeof(msgframe.length));
                msgframe.data = realloc(msgframe.data, atoi(msgframe.length));
                //recvBuff[n] = 0;
                if (fputs(msgframe.data, stdout) == EOF) {
                    printf("\n Error : Fputs error\n");
                }
                if (msgframe.type == 4)
                    break;
            }
            if (msgframe.type == 4)
                break;
        }while(n>0);

    if(n < 0)
    {
        printf("\n Read error \n");
    }

    return 0;
}
