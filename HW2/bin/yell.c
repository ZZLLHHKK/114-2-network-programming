#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static const char *userFile = "/tmp/userlist";

int main(int argc, char **argv) {
    if (argc < 2) return 1; // yell <msg>

    char *uid_env = getenv("MY_UID");
    unsigned int my_uid = uid_env ? (unsigned int)atoi(uid_env) : 0;

    FILE *fin = fopen(userFile, "r");
    if (fin == NULL) return 1;

    unsigned int uid, port;
    int pid;
    char name[30], ip[16];
    // 1. 掃描所有 uid
    unsigned int uids[1024];
    int ucount = 0;
    while (fscanf(fin, "%u %29s %15s %u %d", &uid, name, ip, &port, &pid) == 5) {
        if (ucount < 1024) uids[ucount++] = uid;
    }

    // 把 argv[1] 之後所有參數拼成完整訊息
    char full_msg[1024] = "";
    for (int i = 1; i < argc; i++) {
        if (i != 1) strncat(full_msg, " ", sizeof(full_msg) - strlen(full_msg) - 1);
        strncat(full_msg, argv[i], sizeof(full_msg) - strlen(full_msg) - 1);
    }

    // 2. 組訊息
    char msg[2048];
    snprintf(msg, sizeof(msg), "<user(%u) yelled>: %s\n", my_uid, full_msg);

    fclose(fin);

    // 3. 對每個 uid open FIFO write 訊息
    for (int i = 0; i < ucount; i++) {
        char fifo_path[64];
        snprintf(fifo_path, sizeof(fifo_path), "/tmp/user_fifo_%u", uids[i]);
        int fd = open(fifo_path, O_WRONLY | O_NONBLOCK);
        if (fd < 0) continue;
        write(fd, msg, strlen(msg));
        close(fd);
    }
    return 0;
}