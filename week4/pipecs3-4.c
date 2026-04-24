#include "myhdr.h"
#define WRITE_END 1
#define READ_END 0
char *buf[] = {"Hello Linux", "pipe and fork", "stop and wait"};

void 
doChild(int fd[], char *commandArgv[]) {
    //output to parent
    close(fd[READ_END]);
    dup2(fd[WRITE_END], STDOUT_FILENO);
    close(fd[READ_END]);
    if (execvp(commandArgv[0], commandArgv) < 0) {
        perror("execvp");
        exit(EXIT_FAILURE);
    };
}

void 
doParent(int fd[], char *commandArgv[]) {
    switch(fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            //input from sibling 
            close(fd[WRITE_END]);
            dup2(fd[READ_END], STDIN_FILENO);
            close(fd[READ_END]);
            if (execvp(commandArgv[0], commandArgv) < 0) {
                perror("execvp");
                exit(EXIT_FAILURE);
            };
        default:
            close(fd[READ_END]);
            close(fd[WRITE_END]);
            break;
    }
}

int
main(int argc, char *argv[])
{
    int fd[2];

    char *exeArgv0[50];
    char *exeArgv1[50];

    exeArgv0[0] = "ls"; //argv[1]
    exeArgv0[1] = NULL;

    exeArgv1[0] = "./sbin/number";//argv[2];
    exeArgv1[1] = NULL;

    if ( (pipe(fd) == -1) )  {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch(fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            doChild(fd, exeArgv0);
            break;
        default:
            doParent(fd, exeArgv1);
            wait(NULL);
            wait(NULL);
            break;
    }
    exit(EXIT_SUCCESS);
}               
