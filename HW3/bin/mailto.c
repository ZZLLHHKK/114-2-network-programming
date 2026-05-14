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
    // 1. check argc >= 2
    if (argc < 2) { 
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
    if (argc >= 3) {
        // 從 argv 組訊息：mailto bob hello world
        for (int i = 2; i < argc; i++) {
            strncat(content, argv[i], sizeof(content) - strlen(content) - 1);
            if (i < argc - 1) strncat(content, " ", sizeof(content) - strlen(content) - 1);
        }
    } else {
        // 從 stdin 讀訊息：mailto bob < somefile
        size_t len = 0;
        char buf[256];
        while (fgets(buf, sizeof(buf), stdin) != NULL) {
            strncat(content, buf, sizeof(content) - strlen(content) - 1);
        }
        // 去掉尾端換行
        size_t clen = strlen(content);
        while (clen > 0 && (content[clen-1] == '\n' || content[clen-1] == '\r'))
            content[--clen] = '\0';
        (void)len;
    }

    // insert data and overwrite sql string
    const char *from_user = getenv("MY_NAME");
    if (from_user == NULL) from_user = "unknown";
    char escaped_content[2048];
    mysql_real_escape_string(con, escaped_content, content, strlen(content));
    char sql_insert[4096];
    snprintf(sql_insert, sizeof(sql_insert), "INSERT INTO mail (to_user, from_user, content) VALUES ('%s', '%s', '%s')", argv[1], from_user, escaped_content);
    if (mysql_query(con, sql_insert)) finish(con);

    printf("Send accept !\n");
    mysql_close(con);
    exit(0);

}
