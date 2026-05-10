#define _GNU_SOURCE
#include <mysql/mysql.h>
#include "db_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *userFile    = "/tmp/userlist";
static const char *userFileTmp = "/tmp/userlist.tmp";

int main(int argc, char **argv) {
    if (argc < 2) return 1; // 沒有傳入新名字就結束

    char *pid_env = getenv("MY_PID");
    int mypid = pid_env ? atoi(pid_env) : -1;

    // === 第一輪：掃描是否重名，同時記住自己的舊名字 ===
    FILE *fin = fopen(userFile, "r");
    if (fin == NULL) return 1;

    unsigned int uid, port;
    int pid;
    char name[30], ip[16];
    char oldname[30] = "";

    while (fscanf(fin, "%u %29s %15s %u %d", &uid, name, ip, &port, &pid) == 5) {
        if (pid == mypid)
            strncpy(oldname, name, sizeof(oldname) - 1);
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

    // === 同步更新資料庫（所有相關表） ===
    if (oldname[0] != '\0') {
        MYSQL *con = mysql_init(NULL);
        mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
        mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");
        if (con != NULL && mysql_real_connect(con, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) != NULL) {
            char sql[300];
            // user 表
            snprintf(sql, sizeof(sql), "UPDATE user SET name='%s' WHERE name='%s'", argv[1], oldname);
            mysql_query(con, sql);
            // grp_member 表（成員名稱）
            snprintf(sql, sizeof(sql), "UPDATE grp_member SET user_name='%s' WHERE user_name='%s'", argv[1], oldname);
            mysql_query(con, sql);
            // grp 表（群主名稱）
            snprintf(sql, sizeof(sql), "UPDATE grp SET owner='%s' WHERE owner='%s'", argv[1], oldname);
            mysql_query(con, sql);
            // mail 表（收件人）
            snprintf(sql, sizeof(sql), "UPDATE mail SET to_user='%s' WHERE to_user='%s'", argv[1], oldname);
            mysql_query(con, sql);
            // mail 表（寄件人）
            snprintf(sql, sizeof(sql), "UPDATE mail SET from_user='%s' WHERE from_user='%s'", argv[1], oldname);
            mysql_query(con, sql);
        }
        if (con) mysql_close(con);
    }

    printf("name change accept!\n");
    return 0;
}
