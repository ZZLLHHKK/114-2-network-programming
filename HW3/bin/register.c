#define _GNU_SOURCE
#include <mysql/mysql.h>
#include "db_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>

void finish(MYSQL *con) {
    printf("%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv) { // argv[1] = account, argv[2] = password
    if (argc != 3) { 
        printf("Registration fail, try again!\n");
        return 1;
    }
    /*
    建立一個 MYSQL 物件（連線的容器），傳 NULL 讓 library 自己分配記憶體，回傳的 con 就是之後所有 API 的第一個參數。
    在正式連線前設定選項：
    第 1 行：告訴 client 端用 utf8 編碼
    第 2 行：連線成功後自動執行 SET NAMES utf8，確保 server 端也用 utf8
    （順序要在 mysql_real_connect 之前）
    */
    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL) {
        printf("%s\n", mysql_error(con));
        exit(1);
    }
    /*
    真正建立 TCP 連線並登入資料庫：
    con — 剛才建立的容器
    host — DB 伺服器 IP
    user — MySQL 帳號
    password — MySQL 密碼
    db_name — 要使用的資料庫名稱
    0 — port（0 = 預設 3306）
    NULL — unix socket 路徑（不用就 NULL）
    0 — 額外的連線旗標（通常不需要，填 0）
    */
    if (mysql_real_connect(con, DB_HOST, DB_USER, DB_PASSWORD, DB_NAME, 0, NULL, 0) == NULL) 
        finish(con);

    char sql[100];
    char *name = argv[1]; // user_name: <name>

    sprintf(sql, "SELECT uid FROM user WHERE name='%s' LIMIT 1", name);

    /*
    把你組好的 SQL 字串送到 DB 執行，回傳 0 代表成功、非 0 代表失敗。
    只負責「送出去」，不管結果怎麼拿。
    */
    if (mysql_query(con, sql)) finish(con);  // 先執行 query
    
    /*
    把 DB 回傳的查詢結果整個拉回來存到記憶體，回傳一個 result 物件。
    只有 SELECT 系列的 query 之後才需要呼叫，INSERT/UPDATE/DELETE 不用。
    */
    MYSQL_RES *result = mysql_store_result(con);

    /*
    從剛才拉回來的 result 裡，取得有幾筆資料。
    你這裡用來判斷「這個帳號在 DB 裡有沒有人」。
    */
    int num_rows = mysql_num_rows(result);
    mysql_free_result(result); // 釋放 result 佔用的記憶體，用完就要呼叫，否則記憶體會洩漏。

    if (num_rows > 0) {// account already exists
        printf("1\n");
        mysql_close(con);
        exit(0);
    }
    char *raw_hash = crypt(argv[2], "$6$mysalt$");
    char hashed[200];
    strncpy(hashed, raw_hash ? raw_hash : argv[2], sizeof(hashed)-1);
    hashed[sizeof(hashed)-1] = '\0';
    char sql2[512];
    sprintf(sql2, "INSERT INTO user (name, password, grp) VALUES ('%s', '%s', 0)", argv[1], hashed);
    if (mysql_query(con, sql2)) finish(con);
    printf("0\n");
    mysql_close(con);
    exit(0);
}
