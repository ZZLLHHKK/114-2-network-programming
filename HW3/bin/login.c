#define _GNU_SOURCE
#include <mysql/mysql.h>
#include "db_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>

void finish(MYSQL *con) {
    printf("%d\n",-1);
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv) {
    (void)argc;
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    int res;

    if (con == NULL) finish(con); 
        
    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME,0,NULL,0) == NULL) finish(con);

    char escaped_name[513];
    mysql_real_escape_string(con, escaped_name, argv[1], strlen(argv[1]));
    char sql[600];
    snprintf(sql, sizeof(sql), "SELECT * FROM user WHERE name='%s'", escaped_name);
    if (mysql_query(con, sql)) finish(con);

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL) finish(con);

    int num_rows = mysql_num_rows(result);
    MYSQL_ROW row;
    if (num_rows > 0) {
        row = mysql_fetch_row(result);
        char *check = crypt(argv[2], row[2]);
        if (check && strcmp(row[2], check)==0)
            res = 0;
        else
            res = 1;
    } else
        res = 2;
    mysql_free_result(result);
    printf("%d\n",res);
    mysql_close(con);
    exit(0);
}
