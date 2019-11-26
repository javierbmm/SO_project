//
// Created by Olívia Rey Pellicer on 25/10/2019.
//

#include <commandsActions.h>
#include <fileManager.h>



// Get the command and process it
void getCommand(int i, char * user) {
    int j = 0, k = 0, lenUsername = 0, lenAudio = 0;
    //char aux[BUFF_SIZE], text[BUFF_SIZE], port_s[BUFF_SIZE], user2[BUFF_SIZE], audio[BUFF_SIZE];
    char *user2, *audio;
    user2 = calloc(0,0);
    audio = malloc(0);
    // int port; ??
    switch (i) {
        case 0:
            write(1, TESTING, strlen(TESTING));
            write(1, NOCONNECTIONS, strlen(NOCONNECTIONS));
            break;
      /*  case 1:
            j = strlen(CONNECT) + 1;
            k = 0;
            while (user[j] > '0' && user[j] < '9') {
                port_s[k] = user[j];
                k++;
                j++;
            }
            port_s[k] = '\0';
            sprintf(aux, "%s", port_s); // ???
            write(1, COULDNTCONNECT, strlen(COULDNTCONNECT));

            break;
        case 2:
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
    free(audio);
    free(user2);
}