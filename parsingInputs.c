//
// Created by Olívia Rey Pellicer on 25/10/2019.
//
#include <parsingInputs.h>
#include <fileManager.h>
/*
 * It parses the information introduced by the user and executes the corresponding function
 */
int parseInput(char *user) {
    char commands[][20] = {SHOWCONNECTIONS, CONNECT, SAY, BROADCAST, SHOWAUDIOS, DOWNLOAD, EXIT};
    int i, length = sizeof(commands)/sizeof(commands[0]), found = FALSE;
    char *input = malloc(sizeof(char));
    for(i = 0; i < length; i++) {
        if(i == 0 || i == 4)
            found = (strcasecmp(user, commands[i]) == 0)? TRUE: FALSE;
        else {
            sreadUntil2(user, &input, ' ', '\0'); // (char *input, char** word, char limit, char limit2)
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


