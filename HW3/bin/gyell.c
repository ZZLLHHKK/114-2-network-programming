#include <mysql/mysql.h>
#include "db_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static const char *userFile = "/tmp/userlist";

void finish(MYSQL *con) {
    printf("%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv) { // gyell <group_name> <msg>
    if (argc < 3)  {
        return 1;
    }

    const char *my_name = getenv("MY_NAME");
    if (my_name == NULL) my_name = "unknown";

    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL) {
        printf("%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL) 
        finish(con);

    char sql[256];
    const char *grp_name = argv[1];

    // 確認群組存在
    snprintf(sql, sizeof(sql), "SELECT id FROM grp WHERE name='%s' LIMIT 1", grp_name);
    if (mysql_query(con, sql)) finish(con);
    MYSQL_RES *res = mysql_store_result(con);
    if (mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        printf("Group not found !\n");
        mysql_close(con); return 0;
    }
    mysql_free_result(res);

    // 確認自己是群組成員
    snprintf(sql, sizeof(sql),
        "SELECT id FROM grp_member WHERE group_name='%s' AND user_name='%s' LIMIT 1",
        grp_name, my_name);
    if (mysql_query(con, sql)) finish(con);
    res = mysql_store_result(con);
    if (mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        printf("Group not found !\n");
        mysql_close(con); return 0;
    }
    mysql_free_result(res);

    // 把群組人員存入陣列
    char members[128][64];
    int member_count = 0;
    snprintf(sql, sizeof(sql), "SELECT user_name FROM grp_member WHERE group_name='%s'", grp_name);
    if (mysql_query(con, sql)) finish(con);
    MYSQL_RES *result = mysql_store_result(con);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) && member_count < 128) {
        strncpy(members[member_count++], row[0], 63);
    }
    mysql_free_result(result);
    mysql_close(con);

    // 組訊息
    char content[1024] = {0};
    for (int i = 2; i < argc; i++) {
        if (i != 2) strncat(content, " ", sizeof(content) - strlen(content) - 1);
        strncat(content, argv[i], sizeof(content) - strlen(content) - 1);
    }
    char msg[2048];
    snprintf(msg, sizeof(msg), "<%s:%s>: %s\n", grp_name, my_name, content);

    // 掃描 userlist，對在線成員寫 FIFO
    FILE *fin = fopen(userFile, "r");
    if (fin == NULL) return 0;
    unsigned int uid, port; int pid;
    char name[30], ip[16];
    while (fscanf(fin, "%u %29s %15s %u %d", &uid, name, ip, &port, &pid) == 5) {
        // 確認這個 user 是群組成員
        int is_member = 0;
        for (int i = 0; i < member_count; i++) {
            if (strcmp(name, members[i]) == 0) { is_member = 1; break; }
        }
        if (!is_member) continue;

        char fifo_path[64];
        snprintf(fifo_path, sizeof(fifo_path), "/tmp/user_fifo_%u", uid);
        int fd = open(fifo_path, O_WRONLY | O_NONBLOCK);
        if (fd < 0) continue;
        write(fd, msg, strlen(msg));
        close(fd);
    }
    fclose(fin);
    return 0;
}
