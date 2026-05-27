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

int main(int argc, char **argv) { // createGroup <group_name>
    if (argc != 2) { 
        printf("Create group fail !\n");
        return 1;
    }

    // 連線資料庫
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL) {
        printf("%s\n", mysql_error(con));
        exit(1);
    }
    
    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL) 
        finish(con);

    char escaped_name[513];
    mysql_real_escape_string(con, escaped_name, argv[1], strlen(argv[1]));

    char sql[600];
    const char *owner = getenv("MY_NAME"); // 拿自己的名字當owner

    snprintf(sql, sizeof(sql), "SELECT id FROM grp WHERE name='%s' LIMIT 1", escaped_name);

    if (mysql_query(con, sql)) finish(con);  // 先執行 query
    
    MYSQL_RES *result = mysql_store_result(con);

    int num_rows = mysql_num_rows(result);
    mysql_free_result(result); // 釋放 result 佔用的記憶體，用完就要呼叫，否則記憶體會洩漏。

    if (num_rows > 0) {// group already exists
        printf("Group already exist !\n");
        mysql_close(con);
        exit(0);
    }
    snprintf(sql, sizeof(sql), "INSERT INTO grp (name, owner) VALUES ('%s', '%s')", escaped_name, owner);
    if (mysql_query(con, sql)) finish(con);

    char sql2[600];
    snprintf(sql2, sizeof(sql2), "INSERT INTO grp_member (group_name, user_name) VALUES ('%s', '%s')", escaped_name, owner);
    if (mysql_query(con, sql2)) finish(con);

    printf("Create group success !\n");
    mysql_close(con);
    exit(0);
}
