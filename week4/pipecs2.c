#include "myhdr.h"
#define WRITE_END 1
#define READ_END 0
char *buf[] = {"Hello", "pipe", "stop"};
char *buf1[] = {"Second", "buffer", "string"};

void doChild(int fd[]) {
    int numReads;
    char readBuf[1024]={0};
    
    for (int i=2;i>=0;i--) {
        write(fd[WRITE_END], "child->", 7);
        write(fd[WRITE_END], buf[i], strlen(buf[i]));
        numReads = read(fd[READ_END], readBuf, sizeof(readBuf));
        if(numReads > 0) {
            readBuf[numReads] = '\0';
            printf("%s\n", readBuf);
        }
    }
    close(fd[WRITE_END]);
    close(fd[READ_END]);
}

void doParent(int fd[]) {
    int numReads;
    char readBuf[1024]={0};

    for (int i=0;i<3;i++) {
        write(fd[WRITE_END], "parent->", 8);
        write(fd[WRITE_END], buf1[i], strlen(buf1[i]));
        numReads = read(fd[READ_END], readBuf, sizeof(readBuf));
        if(numReads > 0) {
            readBuf[numReads] = '\0';
            printf("%s\n", readBuf);
        }
    }
    close(fd[WRITE_END]);
    close(fd[READ_END]);
}

int main(int argc, char *argv[])
{
    int fd[2];
   

    if ( (pipe(fd) == -1) )  {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch(fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            doChild(fd);
            break;
        default:
            doParent(fd);
            wait(NULL);
            break;
    }
    exit(EXIT_SUCCESS);
}               
