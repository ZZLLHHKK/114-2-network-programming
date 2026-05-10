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

int main(int argc, char **argv) { // leaveGroup <group_name>
    if (argc != 2)
        return 1;
    
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

    char sql[256]; // check whether group exist
    snprintf(sql, sizeof(sql), "SELECT owner FROM grp WHERE name='%s'", argv[1]);
    
    if (mysql_query(con, sql)) finish(con);

    MYSQL_RES *result = mysql_store_result(con);
    int num_row = mysql_num_rows(result);

    if (num_row == 0) {
        printf("Group not found !\n");
    } else {
        char sql1[512]; // check whether I have joint the group or not
        snprintf(sql1, sizeof(sql1), "SELECT id FROM grp_member WHERE group_name='%s' AND user_name='%s'", argv[1], my_name);

        if (mysql_query(con, sql1)) finish(con);

        MYSQL_RES *res1 = mysql_store_result(con);
        int search_row = mysql_num_rows(res1);

        if (search_row == 0) // You are not in group
            printf("Leave fault !\n");
        else {
            char sql2[512];
            snprintf(sql2, sizeof(sql2), "DELETE FROM grp_member WHERE group_name='%s' AND user_name='%s'", argv[1], my_name);
            if (mysql_query(con, sql2)) finish(con);
            printf("Leave success !\n");
        }
        mysql_free_result(res1);
    }

    mysql_free_result(result);
    mysql_close(con);
    return 0;
}