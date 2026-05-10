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

int main(int argc, char **argv) { // mailto user_name msg
    // 1. check argc >= 3
    if (argc < 3) { 
        printf("Usage: mailto <user> <message>\n");
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
    // select to_user make sure whether it is exist
    char sql[100];
    char *name = argv[1]; // user_name

    snprintf(sql, sizeof(sql), "SELECT name FROM user WHERE name='%s' LIMIT 1", name);

    if (mysql_query(con, sql)) finish(con); // success return 0, or else non-zero

    MYSQL_RES *result = mysql_store_result(con);

    int num_row = mysql_num_rows(result);
    mysql_free_result(result);

    // if user not exist, exit program
    if (num_row == 0) {
        printf("User not found !\n");
        mysql_close(con);
        exit(0);
    }
    
    // construct message
    char content[1024] = {0};
    for (int i = 2; i < argc; i++) {
        strcat(content, argv[i]);
        if (i < argc - 1) strcat(content, " ");
    }

    // insert data and overwrite sql string
    const char *from_user = getenv("MY_NAME");
    if (from_user == NULL) from_user = "unknown";
    char sql_insert[2048];
    snprintf(sql_insert, sizeof(sql_insert), "INSERT INTO mail (to_user, from_user, content) VALUES ('%s', '%s', '%s')", argv[1], from_user, content);
    if (mysql_query(con, sql_insert)) finish(con);

    printf("Send accept !\n");
    mysql_close(con);
    exit(0);

}
