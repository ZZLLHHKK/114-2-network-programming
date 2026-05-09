#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *userFile    = "/tmp/userlist";
static const char *userFileTmp = "/tmp/userlist.tmp";

int main(int argc, char **argv) {
    if (argc < 2) return 1; // 沒有傳入新名字就結束

    char *pid_env = getenv("MY_PID");
    int mypid = pid_env ? atoi(pid_env) : -1;

    // === 第一輪：掃描是否重名 ===
    FILE *fin = fopen(userFile, "r");
    if (fin == NULL) return 1;

    unsigned int uid, port;
    int pid;
    char name[30], ip[16];

    while (fscanf(fin, "%u %29s %15s %u %d", &uid, name, ip, &port, &pid) == 5) {
        if (strcmp(name, argv[1]) == 0 && pid != mypid) {
            printf("User %s already exists !\n", argv[1]);
            fclose(fin);
            return 1;
        }
    }
    fclose(fin);

    // === 第二輪：搬運並更新自己那行 ===
    fin  = fopen(userFile,    "r");
    FILE *fout = fopen(userFileTmp, "w");
    if (fin == NULL || fout == NULL) return 1;

    while (fscanf(fin, "%u %29s %15s %u %d", &uid, name, ip, &port, &pid) == 5) {
        if (pid == mypid)
            fprintf(fout, "%u %s %s %u %d\n", uid, argv[1], ip, port, pid); // 換名字
        else
            fprintf(fout, "%u %s %s %u %d\n", uid, name,    ip, port, pid); // 原樣
    }
    fclose(fin);
    fclose(fout);

    // === 把暫存檔換掉原檔 ===
    rename(userFileTmp, userFile);

    printf("name change accept!\n");
    return 0;
}
