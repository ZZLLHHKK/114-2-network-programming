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

int main(int argc, char **argv) { // listGroup : list the group you have joint
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

    char sql[256]; // join two table
    snprintf(sql, sizeof(sql),
        "SELECT m.group_name, g.owner FROM grp_member m JOIN grp g ON m.group_name=g.name WHERE m.user_name='%s'",
        my_name);

    if (mysql_query(con, sql)) finish(con);

    MYSQL_RES *result = mysql_store_result(con);
    int num_row = mysql_num_rows(result);

    if (num_row == 0) {
        printf("Empty !\n");
    } else {
        printf("%-12s %-12s\n", "<group>", "<owner>");
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            printf("%-12s %-12s\n", row[0], row[1]);
        }
    }

    mysql_free_result(result);
    mysql_close(con);
    return 0;
}
