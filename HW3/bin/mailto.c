#include <mysql/mysql.h>
#include "db_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        if (strcmp(argv[2], "<") == 0 && argc >= 4) {
            /* mailto user < command  →  執行 command，捕獲其 stdout 當信件內容 */
            int pfd[2];
            if (pipe(pfd) == -1) { perror("pipe"); mysql_close(con); exit(1); }
            pid_t cpid = fork();
            if (cpid < 0) { perror("fork"); mysql_close(con); exit(1); }
            if (cpid == 0) {
                /* child: exec command，stdout → pipe write end */
                close(pfd[0]);
                dup2(pfd[1], STDOUT_FILENO);
                close(pfd[1]);
                /* argv[3], argv[4], ... 是要執行的 command 與其參數 */
                char *cmd_argv[argc - 2]; /* argc-3 個 arg + NULL */
                for (int i = 3; i < argc; i++)
                    cmd_argv[i - 3] = argv[i];
                cmd_argv[argc - 3] = NULL;
                execvp(cmd_argv[0], cmd_argv);
                exit(127);
            } else {
                /* parent: 從 pipe 讀取 command 的輸出 */
                close(pfd[1]);
                char buf[256];
                ssize_t nr;
                while ((nr = read(pfd[0], buf, sizeof(buf) - 1)) > 0) {
                    buf[nr] = '\0';
                    strncat(content, buf, sizeof(content) - strlen(content) - 1);
                }
                close(pfd[0]);
                waitpid(cpid, NULL, 0);
            }
            /* 去掉尾端換行 */
            size_t clen = strlen(content);
            while (clen > 0 && (content[clen-1] == '\n' || content[clen-1] == '\r'))
                content[--clen] = '\0';
        } else {
            /* 原本行為：把 argv[2..] 串成訊息 */
            for (int i = 2; i < argc; i++) {
                strncat(content, argv[i], sizeof(content) - strlen(content) - 1);
                if (i < argc - 1) strncat(content, " ", sizeof(content) - strlen(content) - 1);
            }
        }
    } else {
        // 從 stdin 讀訊息：mailto bob（不帶任何訊息時）
        char buf[256];
        while (fgets(buf, sizeof(buf), stdin) != NULL) {
            strncat(content, buf, sizeof(content) - strlen(content) - 1);
        }
        // 去掉尾端換行
        size_t clen = strlen(content);
        while (clen > 0 && (content[clen-1] == '\n' || content[clen-1] == '\r'))
            content[--clen] = '\0';
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
