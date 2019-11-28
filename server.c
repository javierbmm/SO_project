//
// Created by javif on 10/20/2019.
//
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include<signal.h>

#define SIZE_BUFF 1000
int break_while = 0;
int g_listenfd = 0; // global socket info

void signal_handler(){
    break_while = 1;
    close(g_listenfd);
}

void getTime(char* buffer){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    buffer = realloc(buffer, sizeof(char)*6);
    sprintf(buffer,"%d:%d:%d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);

}

void getDate(char* buffer){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    buffer = realloc(buffer, sizeof(char)*5);
    sprintf(buffer, "%d-%d-%d\n", tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday);

}

// Creating a menu using pointers to functions because it's funny
void *menu(void * num_client){
    // Creating an array of pointers to functions
    int client = *(int*)num_client;

    void (*func_arr[])(char*) = {getTime, getDate};
    char aux[3];
    int option = 0;
    char buffer[SIZE_BUFF], *opt_buff = calloc(0,0);

    // Menu do-while:
    do{
        if(read(client, aux, 3) < 0){
            sprintf(buffer,"Error reading option\n");
            write(client, buffer, strlen(buffer));

            exit(1);
        }
        option = atoi(aux);

        if(option > 4 || option < 0){
            write(client, aux, strlen(aux));
            sprintf(opt_buff,"Wrong option. Try again\n");
            sprintf(buffer, "%ld", strlen(opt_buff));
            write(client, buffer, strlen(buffer));
            write(client, buffer, strlen(opt_buff));
            continue;
        }else if(option == 3){
            write(client, aux, strlen(aux));
            sprintf(opt_buff,"Haven't done this yet hehe \n");
            sprintf(buffer, "%ld", strlen(opt_buff));
            write(client, buffer, strlen(buffer));
            write(client, buffer, strlen(opt_buff));
            continue;
        }else if(option == 4){
            write(client, aux, strlen(aux));
            sprintf(buffer,"Bye!\n");
            sprintf(buffer, "%ld", strlen(opt_buff));
            write(client, buffer, strlen(buffer));
            write(client, buffer, strlen(opt_buff));
            continue;
        }else if(option == 0)
            continue;
        else if(break_while == 1){
            break;
        }

        func_arr[option-1](opt_buff);
        write(client, aux, strlen(aux));
        sprintf(buffer, "%ld", strlen(opt_buff));
        write(client, buffer, strlen(buffer));
        write(client, opt_buff, strlen(opt_buff));
    }while(option != 4 && break_while != 1) ;

    free(opt_buff);
    //free(func_arr);
    close(client);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
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
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    /* The call to the function "bind()" assigns the details specified
     * in the structure 'serv_addr' to the socket created in the step above
     */
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    /* The call to the function "listen()" with second argument as 10 specifies
     * maximum number of client connections that server will queue for this listening
     * socket.
     */
    listen(listenfd, 100);
    pthread_t *thread_ids = calloc(0,0);
    int num_of_threads = 0, actual_thread = 0;

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

