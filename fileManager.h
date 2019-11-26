//
// Created by Olívia Rey Pellicer on 22/10/2019.
//

#ifndef PROJECTF1_1_FILEMANAGER_H

#define PROJECTF1_1_FILEMANAGER_H
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"




int getSizeOfLine(int file, char charLimit);
int readUntil(int file, char** word, char limit);
int sreadUntil(char* input, char** word, char limit); // same as above but for strings
int readUntilLimit(int file, char** line, char limit);
void skipDelimiter(int file, char delimiter);
FileData getFileData (const int file);


#endif //PROJECTF1_1_FILEMANAGER_H
