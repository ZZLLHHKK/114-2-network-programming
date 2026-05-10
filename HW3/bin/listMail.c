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

int main(void) { // listmail
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
    snprintf(sql, sizeof(sql),
        "SELECT id, date, from_user, content FROM mail WHERE to_user='%s' ORDER BY id DESC",
        my_name);

    if (mysql_query(con, sql)) finish(con);

    MYSQL_RES *result = mysql_store_result(con);
    int num_row = mysql_num_rows(result);

    if (num_row == 0) {
        printf("empty !\n");
    } else {
        printf("%-6s %-20s %-12s %s\n", "<id>", "<date>", "<sender>", "<message>");
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            printf("%-6s %-20s %-12s %s\n",
                row[0], row[1] ? row[1] : "", row[2], row[3]);
        }
    }

    mysql_free_result(result);
    mysql_close(con);
    return 0;
}
