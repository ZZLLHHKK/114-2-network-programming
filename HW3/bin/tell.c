#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static const char *userFile = "/tmp/userlist";

int main(int argc, char **argv) {
    if (argc < 3) return 1; // tell <uid> <msg>

    char *uid_env = getenv("MY_UID");
    unsigned int my_uid = uid_env ? (unsigned int)atoi(uid_env) : 0;

    unsigned int target_uid = (unsigned int)atoi(argv[1]);

    FILE *fin = fopen(userFile, "r");
    if (fin == NULL) return 1;

    unsigned int uid, port;
    int pid;
    char name[30], ip[16];
    int found = 0;

    while (fscanf(fin, "%u %29s %15s %u %d", &uid, name, ip, &port, &pid) == 5) {
        if (uid == target_uid)
            found = 1;
    }
    fclose(fin);

    if (!found) {
        printf("*** Error: user #%u does not exist yet. ***\n", target_uid);
        return 1;
    }

    char fifo_path[64];
    snprintf(fifo_path, sizeof(fifo_path), "/tmp/user_fifo_%u", target_uid);

    // 把 argv[2] 之後所有參數拼成完整訊息
    char full_msg[1024] = "";
    for (int i = 2; i < argc; i++) {
        if (i != 2) strncat(full_msg, " ", sizeof(full_msg) - strlen(full_msg) - 1);
        strncat(full_msg, argv[i], sizeof(full_msg) - strlen(full_msg) - 1);
    }

    char msg[2048];
    snprintf(msg, sizeof(msg), "<user(%u) told you>: %s\n", my_uid, full_msg);

    int fd = open(fifo_path, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        printf("*** Error: user #%u does not exist yet. ***\n", target_uid);
        return 1;
    }
    write(fd, msg, strlen(msg));
    close(fd);
    printf("send accept!\n");

    return 0;
}
