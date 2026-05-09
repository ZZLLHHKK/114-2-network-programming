#include <mysql/mysql.h>
#include "db_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void finish(MYSQL *con) {
    printf("%d\n",-1);
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv) {
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    int res;

    if (con == NULL) finish(con); 
        
    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME,0,NULL,0) == NULL) finish(con);

    char sql[100];
    
    sprintf(sql, "SELECT * FROM user WHERE name='%s'", argv[1]);
    if (mysql_query(con, sql)) finish(con);

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL) finish(con);

    int num_rows = mysql_num_rows(result);
    MYSQL_ROW row;
    if (num_rows > 0) {
        row = mysql_fetch_row(result);
        if (strcmp(row[2], argv[2])==0)
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
