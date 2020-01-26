//
// Created by Olívia Rey Pellicer on 25/10/2019.
//
#include <parsingInputs.h>
#include <fileManager.h>

int parseInput(char *user) {
    char commands[][20] = {SHOWCONNECTIONS, CONNECT, SAY, BROADCAST, SHOWAUDIOS, DOWNLOAD, EXIT};
    int i, length = sizeof(commands)/sizeof(commands[0]), found = FALSE;
    char *input = calloc(0,0);
    for(i = 0; i < length; i++) {
        if(i == 0 || i == 4)
            found = (strcasecmp(user, commands[i]) == 0)? TRUE: FALSE;
        else {
            sreadUntil(user, &input, ' '); // (char *input, char** word, char limit)
            found = (strcasecmp(input, commands[i]) == 0) ? TRUE : FALSE;
        }
        if(found == TRUE) {
            free(input);
            return i;
        }
    }
    write(1, ERRORCOMMAND, strlen(ERRORCOMMAND));
    free(input);
    return i;
}


