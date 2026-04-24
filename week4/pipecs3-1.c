#include "myhdr.h"
#define WRITE_END 1
#define READ_END 0
char *buf[] = {"Hello Linux", "pipe and fork", "stop and wait"};

void 
doChild(int fd[]) {
    int numReads;
    char readBuf[1024]={0};

    //read data fro parent
    close(fd[WRITE_END]);
    while(1) {
        numReads = read(fd[READ_END], readBuf, sizeof(readBuf));
        if(numReads > 0) {
            readBuf[numReads] = '\0';
            printf("child rx ==> %s\n", readBuf);
        } else if (numReads == 0) {
            break;
        }        
    }    
    close(fd[READ_END]);
}

void 
doParent(int fd[]) {
    /* parent send data to child*/
    close(fd[READ_END]);
    for (int i=0;i<3;i++) {
        write(fd[WRITE_END], buf[i], strlen(buf[i]));
        sleep(0.000001);
    }
    close(fd[WRITE_END]);
}

int
main(int argc, char *argv[])
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
            break;
    }
    exit(EXIT_SUCCESS);
}               
