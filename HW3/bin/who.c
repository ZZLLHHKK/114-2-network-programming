#include <stdio.h>
#include <stdlib.h>

static const char *userFile = "/tmp/userlist";

int main(void) {
        char *pid_env = getenv("MY_PID");
        int mypid = pid_env ? atoi(pid_env) : -1;
        unsigned int uid, port;
        int pid;
        char ip[16], name[30];

        FILE *fin = fopen(userFile, "r");
        if (fin == NULL) return 1;
        printf("%-4s %-20s %-22s %s\n", "<ID>", "<name>", "<IP:port>", "<indicate me>");

        while (fscanf(fin, "%u %29s %15s %u %d", &uid, name, ip, &port, &pid) == 5) {
                char ip_port[32];
                snprintf(ip_port, sizeof(ip_port), "%s:%u", ip, port);
                if (pid == mypid)
                        printf("%-4u %-20s %-22s %s\n", uid, name, ip_port, "<-(me)");
                else
                        printf("%-4u %-20s %-22s\n", uid, name, ip_port);
        }
        fclose(fin);
        return 0;
}