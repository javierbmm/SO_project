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
int readOnlyUntil(int file, char** word, char limit);
int readUntil(int file, char** word, char limit);
int sreadUntil(char* input, char** word, char limit); // same as above but for strings
int sreadUntil2(char *input, char** word, char limit, char limit2);
int readUntilLimit(int file, char** line, char limit);
void skipDelimiter(int file, char delimiter);
FileData getFileData (const int file, FileData *_filedata);
void myprint(char* msg);
int myAtoi(const char *str, int len);

// Utility function to get the MD5SUM from a file
char* get_md5sum(char* filepath);
int check_md5sum(char* checksum, char* filepath);

#endif //PROJECTF1_1_FILEMANAGER_H
