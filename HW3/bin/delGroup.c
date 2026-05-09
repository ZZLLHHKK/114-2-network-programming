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

int main(int argc, char **argv) { // delGroup <group_name>
    if (argc != 2) {
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

    // 只有 owner 刪除群組
    if (strcmp(owner, my_name) != 0) {
        printf("You don't have permission !\n");
        mysql_close(con); return 0;
    }

    // 刪 grp_member 所有成員
    snprintf(sql, sizeof(sql), "DELETE FROM grp_member WHERE group_name='%s'", grp_name);
    if (mysql_query(con, sql)) finish(con);

    // 刪 grp
    snprintf(sql, sizeof(sql), "DELETE FROM grp WHERE name='%s'", grp_name);
    if (mysql_query(con, sql)) finish(con);

    printf("Group delete success !\n");
    mysql_close(con);
    return 0;
}
