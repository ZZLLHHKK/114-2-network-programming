#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

char userFile[20] = "/tmp/userlist";

void main(int argc, char **argv) {
        int fd;
        unsigned int uid, port, pid;
        char ip[15], name[30];
        char buf[100];

        FILE *fin = fopen(userFile, "r");
        printf("<ID>\t<name>\t<IP:port>\t\t<indicate me>\n\r");

        while(fscanf(fin, "%d %s %s %d %d", &uid, name, ip, &port, &pid)!=EOF) {
                if (pid == atoi(argv[1]))
                   printf("%4d\t%s\t%s:%d\t<-(me)\n\r", uid, name, ip, port);
                else
                   printf("%4d\t%s\t%s:%d\t\n\r", uid, name, ip, port);
        }
        fclose(fin);
}