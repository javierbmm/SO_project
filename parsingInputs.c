//
// Created by Olívia Rey Pellicer on 25/10/2019.
//
#include <parsingInputs.h>

int parseInput(char *user) {
    char commands[][20] = {SHOWCONNECTIONS, CONNECT, SAY, BROADCAST, SHOWAUDIOS, DOWNLOAD, EXIT};
    int i, length = sizeof(commands)/sizeof(commands[0]), found = FALSE;

    for(i = 0; i < length; i++) {
        found = (strncasecmp(user, commands[i], strlen(commands[i])) == 0)? TRUE: FALSE;
        if(found == TRUE)
            return i;
    }

    write(1, ERRORCOMMAND, strlen(ERRORCOMMAND));
    return i;
}


