#include "myhdr.h"
#define READ_END 0
#define WRITE_END 1

int
main(int argc, char *argv[])
{
    int fd[2];
    char *buf[] = {"Hello", "pipe", "stop"};
   

    if ( (pipe(fd) == -1) )  {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int numReads;
    char readBuf[1024]={0};

    for (int i=0;i<3;i++) {
        write(fd[WRITE_END], buf[i], sizeof(buf[i]));
        numReads = read(fd[READ_END], readBuf, sizeof(readBuf));
        if(numReads > 0) {
            printf("%s\n", readBuf);
        }
    }
    close(fd[WRITE_END]);
    close(fd[READ_END]);
    exit(EXIT_SUCCESS);
}               
