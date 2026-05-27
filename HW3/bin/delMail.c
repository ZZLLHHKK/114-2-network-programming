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

int main(int argc, char **argv) { // delMail <mail_id>
    if (argc < 2) {
        printf("Usage: delMail <id>\n");
        return 1;
    }

    // connect to db
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL) {
        printf("%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL)
        finish(con);

    const char *my_name = getenv("MY_NAME");
    if (my_name == NULL) my_name = "unknown";

    // argv[1] 必須是純整數，避免 SQL injection
    for (int i = 0; argv[1][i] != '\0'; i++) {
        if (argv[1][i] < '0' || argv[1][i] > '9') {
            printf("Mail id unexist !\n");
            mysql_close(con);
            return 0;
        }
    }
    long mail_id = atol(argv[1]);

    char sql[256];
    snprintf(sql, sizeof(sql),
        "DELETE FROM mail WHERE id=%ld AND to_user='%s'",
        mail_id, my_name);

    if (mysql_query(con, sql)) finish(con);

    if (mysql_affected_rows(con) == 0)
        printf("Mail id unexist !\n");
    else
        printf("Delete accept !\n");

    mysql_close(con);
    return 0;
}
