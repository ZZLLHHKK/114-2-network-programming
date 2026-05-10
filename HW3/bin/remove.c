#include <mysql/mysql.h>
#include "db_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void finish(MYSQL *con) {
    printf("%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv) { // remove <group_name> <user_name> (only owner can remove group members)
    if (argc < 3) {
        printf("Usage: remove <group_name> <user1> ...\n");
        return 1;
    }

    const char *my_name = getenv("MY_NAME");
    if (my_name == NULL) my_name = "unknown";

    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");
    if (con == NULL) { printf("%s\n", mysql_error(con)); exit(1); }
    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL)
        finish(con);

    char sql[256];
    const char *grp_name = argv[1];

    // 確認群組存在並拿 owner
    snprintf(sql, sizeof(sql), "SELECT owner FROM grp WHERE name='%s' LIMIT 1", grp_name);
    if (mysql_query(con, sql)) finish(con);
    MYSQL_RES *res = mysql_store_result(con);
    if (mysql_num_rows(res) == 0) {
        mysql_free_result(res);
        printf("Group not found !\n");
        mysql_close(con); return 0;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    char owner[256];
    strncpy(owner, row[0], sizeof(owner) - 1);
    mysql_free_result(res);

    // 只有 owner 可以踢人
    if (strcmp(owner, my_name) != 0) {
        printf("You don't have permission !\n");
        mysql_close(con); return 0;
    }

    // 對每個 user 處理
    char success[32][256], notfound[32][256], already[32][256];
    int n_success = 0, n_notfound = 0, n_already = 0;

    for (int i = 2; i < argc; i++) {
        const char *target = argv[i];

        // 查 user 是否存在
        snprintf(sql, sizeof(sql), "SELECT uid FROM user WHERE name='%s' LIMIT 1", target);
        if (mysql_query(con, sql)) finish(con);
        res = mysql_store_result(con);
        if (mysql_num_rows(res) == 0) {
            mysql_free_result(res);
            strncpy(notfound[n_notfound++], target, 255);
            continue;
        }
        mysql_free_result(res);

        // 查是否在群組（不在的話不能踢）
        snprintf(sql, sizeof(sql),
            "SELECT id FROM grp_member WHERE group_name='%s' AND user_name='%s' LIMIT 1",
            grp_name, target);
        if (mysql_query(con, sql)) finish(con);
        res = mysql_store_result(con);
        if (mysql_num_rows(res) == 0) {
            mysql_free_result(res);
            strncpy(already[n_already++], target, 255);
            continue;
        }
        mysql_free_result(res);

        // DELETE
        snprintf(sql, sizeof(sql),
            "DELETE FROM grp_member WHERE group_name='%s' AND user_name='%s'",
            grp_name, target);
        if (mysql_query(con, sql)) finish(con);
        strncpy(success[n_success++], target, 255);
    }

    // 印結果
    if (n_notfound > 0) {
        for (int i = 0; i < n_notfound; i++) printf("%s ", notfound[i]);
        printf("not found !\n");
    }
    if (n_already > 0) {
        for (int i = 0; i < n_already; i++) {
            printf("%s", already[i]);
            if (i < n_already - 1) printf(", ");
        }
        printf(" is not in group.\n");
    }
    if (n_success > 0) {
        for (int i = 0; i < n_success; i++) printf("%s ", success[i]);
        printf("remove success !\n");
    }

    mysql_close(con);
    return 0;
}
