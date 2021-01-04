//
// Created by Olívia Rey Pellicer on 22/10/2019.
//


#include <fileManager.h>


/* Read the file char by char until it reaches the character specified in charLimit
 *  ARGS: int file
 *  ARGS: char charLimit
 *
 *  @returns-> charPosition: Position (bytes) of the char specified in charLimit
 */
int getSizeOfLine(int file, char charLimit) {
    int charPosition = 0;
    char iteratorChar;  // We use char because it's 1 byte size, so we can count the exact number of bytes
    while(1){
        if(read(file, &iteratorChar, 1) == 0)
            return 0;
        if(iteratorChar == charLimit)
            break;
        charPosition++;
    };

    return charPosition;
}

int readOnlyUntil(int file, char** word, char limit){
    int wordLength = 0;
    memset(*word, 0, strlen(*word));

    char letter[1];
    while(close_1 == 0){
        if(read(file, letter, 1) > 0){
            // reading letter by letter (one byte at time) until limit
            if(letter[0] == limit) {
                break;
            }
            *word = (char*)realloc(*word, sizeof(char) * ++wordLength);
            (*word)[wordLength-1] = letter[0];
            letter[0] = 0;
        }else
            return 0;
    }

    if(wordLength == 0) // Read nothing a.k.a empty string
        return 0;

    *word = (char*)realloc(*word, sizeof(char) * (++wordLength));
    (*word)[wordLength-1] = '\0';

    return wordLength;
}

/*
 * This function reads the information until a given character and stores it in word variable. It returns the
 * length read from the file
 */
int readUntil(int file, char** word, char limit){
    int wordLength = 0;
    memset(*word, 0, strlen(*word));
    //char *word = (char*)malloc(0);
    //word = realloc(word, 0);
    char letter[1];
    while(close_1 == 0){
        if(read(file, letter, 1) > 0){
            // reading letter by letter (one byte at time) until limit
            if(letter[0] == limit || letter[0] == '\0') {
                break;
            }
            *word = (char*)realloc(*word, sizeof(char) * ++wordLength);
            (*word)[wordLength-1] = letter[0];
            letter[0] = 0;
        }else
            return 0;
    }

    if(wordLength == 0) // Read nothing a.k.a empty string
        return 0;

    *word = (char*)realloc(*word, sizeof(char) * (++wordLength));
    (*word)[wordLength-1] = '\0';

    return wordLength;
}
/*
 * This function reads the information until a given character and stores it in word variable. It returns the
 * length read from the input
 */
int sreadUntil(char *input, char** word, char limit){
    int wordLength = 0;
    char letter[1];
    while(close_1 == 0){
        letter[0] = input[wordLength];
        myprint(letter);
        // reading letter by letter (one byte at time) until limit
        if(letter[0] == limit || letter[0] == '\n')
            break;
        *word = (char*)realloc(*word, sizeof(char) * ++wordLength);
        (*word)[wordLength-1] = letter[0];
    }
    *word = (char*)realloc(*word, sizeof(char) * ++wordLength);
    (*word)[wordLength-1] = '\0';

    return wordLength;
}
/*
 * Reads the information given two delimiters
 */
int sreadUntil2(char *input, char** word, char limit, char limit2){
    int wordLength = 0;
    char letter[1];
    while(close_1 == 0){
        letter[0] = input[wordLength];
        // reading letter by letter (one byte at time) until limit
        if(letter[0] == limit || letter[0] == limit2 || letter[0] == '\n')
            break;
        *word = (char*)realloc(*word, sizeof(char) * ++wordLength);
        (*word)[wordLength-1] = letter[0];
    }
    *word = (char*)realloc(*word, sizeof(char) * ++wordLength);
    (*word)[wordLength-1] = '\0';

    return wordLength;
}
int readUntilLimit(int file, char** line, char limit){
    int endOfLinePosition = getSizeOfLine(file, limit);  // Get the number of characters to read

    *line = realloc(*line,(endOfLinePosition+1)* sizeof(char)); // Assigning enough memory
    // Now we have to go back on the file using read and the same number of bytes from endOfLinePosition
    lseek(file, -endOfLinePosition-1, SEEK_CUR);
    read(file, *line, endOfLinePosition);
    (*line)[endOfLinePosition] = '\0';

    return endOfLinePosition;
}
/* skipDelimiter: Reads all the incomming delimiters until it reaches one non-delimiter,
 * then go back one byte
*/
void skipDelimiter(int file, char delimiter){
    char iteratorChar;

    while(read(file, &iteratorChar, 1) > 0){
        if(iteratorChar == delimiter) // if delimiter, keep going
            continue;
        else {                          // go back when isn't delimiter
            lseek(file, -1, SEEK_CUR);
            return;
        }
    }
}

/*
 * It gets and stores the information stored ina  given file
 */
FileData getFileData (const int file, FileData *data) {
    int i = 0;
    char delimiter = '\n';
    char *word = calloc(1, 1);
    int size_of_word = 1;

    while (size_of_word> 0) { // 0 reached end of file
        size_of_word = readUntilLimit(file, &word, delimiter);
        switch (i) {
            case 0:
                data->user_name = malloc(size_of_word+1);
                strcpy(data->user_name, word);
                break;
            case 1:
                data->audio_folder = malloc(size_of_word+1);
                strcpy(data->audio_folder, word);
                break;
            case 2:
                data->ip = malloc(size_of_word+1);
                strcpy(data->ip, word);
                break;
            case 3:
                data->port = malloc(size_of_word+1);
                strcpy(data->port, word);
                break;
            case 4:
                data->web_ip = malloc(size_of_word+1);
                strcpy(data->web_ip, word);
                break;
            case 5:
                data->init_port = malloc(size_of_word+1);
                strcpy(data->init_port, word);
                break;
            case 6:
                data->final_port = malloc(size_of_word+1);
                strcpy(data->final_port, word);
                break;
        };

        skipDelimiter(file, delimiter);
        i++;
        // Empty-ing the buffer
        memset(word,0,strlen(word));
        word[0] = '\0';
        //free(word_cpy);
    }

    //free(word);
    //close(f);

    return *data;
}

// Print function for simple strings (NO %d %f %s etc)
void myprint(char* msg){
    char buffer[strlen(msg)+1];
    sprintf(buffer, "%s", msg);
    write(1, buffer, strlen(buffer));
}
/**
 * Utility function to get the MD5SUM of file using the md5sum linux function
 * executed using fork and execvp
 * @param filepath indicates the path of the file
 * @return string with md5sum key and file path
 */
char* get_md5sum(char* filepath){
    pid_t child_pid;
    int fd[2];
    pipe(fd);
    // fd[0] -> for using read end
    // fd[1] -> for using write end

    child_pid = fork();
    if (child_pid == 0) {
        char *args[] = {
                "md5sum",
                filepath,
                NULL
        };
        dup2(fd[1], 1);
        execvp(args[0], args);
        return NULL;
        // parent process because return value non-zero.
    }else{
        wait(NULL);
        char* checksum = calloc(1,1);
        if(readUntil(fd[0], &checksum, ' ') <= 1) {
            free(checksum);
            checksum = NULL;
        }

        close(fd[0]);
        close(fd[1]);

        return checksum;
    }
}

/***
 * utility function to compare and check the checksum input with the checksum
 * obtained for the file in filepath.
 * @param checksum: input md5sum key to check
 * @param filepath: file to check
 * @return TRUE (1) or FALSE (0) depending on whether or not the checksum is correct
 */
int check_md5sum(char* checksum, char* filepath){
    if(checksum == NULL || filepath == NULL)
        return FALSE;

    char *new_checksum = get_md5sum(filepath);
    int is_correct = FALSE;
    if(strncmp(new_checksum, checksum, strlen(new_checksum)) == 0)
        is_correct = TRUE;

    free(new_checksum);

    return is_correct;
}
/*
 * Atoi function coded by us
 */
int myAtoi(const char *str, int len){
    int i;
    int ret = 0;
    for(i = 0; i < len; ++i)
    {
        ret = ret * 10 + (str[i] - '0');
    }
    return ret;
}
