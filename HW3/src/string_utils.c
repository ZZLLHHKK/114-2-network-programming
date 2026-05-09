#include "../include/myhdr.h"

void trim_spaces(char *str) {
    char *start = str;  // ex : "   hello world   "
    while (*start == ' ' || *start == '\t') { // find a first space
        start++;
    }

    if (start != str) { // move to correct spot "hello world   "
        memmove(str, start, strlen(start) + 1);
    }

    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t')) { // remove back space
        str[--len] = '\0';
    }
    // result = "hello world"
}

void build_argv(command_t *cmd, char *argv[]) {
    int i = 0;

    // argv[0] 一定是程式名稱（execvp 約定）
    argv[i++] = cmd->command;

    /*
    char *pipe_pos = strchr(cmd->paramater, '|'); // ignore pipe parameter
    if (pipe_pos != NULL)
        *pipe_pos = '\0';
    */
    // 這裡把 "-l   /tmp" 轉成 token 序列 "-l", "/tmp"
    // 最後組成：argv = {"ls", "-l", "/tmp", NULL}
    trim_spaces(cmd->paramater);

    char *token = strtok(cmd->paramater, " \t");
    while (token != NULL) {
        argv[i++] = token;
        token = strtok(NULL, " \t");
    }

    argv[i] = NULL;

    //for (int j = 0; argv[j] != NULL; ++j)
    //    printf("argv[%d] = %s\n", j, argv[j]);
}

command_t *parser(char *commandStr) {
    // parser 目標：把「單一 command 字串」拆成
    // 1) cmd->command   (指令名稱)
    // 2) cmd->paramater (其餘參數字串，尚未切成 argv)
    //
    // 例子：
    // 輸入 commandStr = "   ls   -l   /tmp   "
    // trim 後 buffer   = "ls   -l   /tmp"
    // 切第一個空白後：
    //   cmd->command   = "ls"
    //   cmd->paramater = "-l   /tmp"
    command_t *cmd = (command_t *)calloc(1, sizeof(command_t));
    char buffer[256] = {0};
    char *space = NULL;

    strncpy(buffer, commandStr, sizeof(buffer) - 1);
    trim_spaces(buffer);

    if (buffer[0] == '\0') {
        // 空字串或全空白，回傳空 command（caller 可自行判斷略過）
        return cmd;
    }

    space = strpbrk(buffer, " \t");
    if (space == NULL) {
        // 沒有任何空白，代表只有指令名稱，沒有參數
        // 例："pwd" -> command="pwd", paramater=""
        strncpy(cmd->command, buffer, sizeof(cmd->command) - 1);
        return cmd;
    }

    // 在第一個空白處截斷，把前半段當 command
    *space = '\0';
    strncpy(cmd->command, buffer, sizeof(cmd->command) - 1);

    // 指到第一個空白後的內容，再略過連續空白
    space++;
    while (*space == ' ' || *space == '\t') {
        space++;
    }
    // 後半段整串保留到 paramater，交由 build_argv 再做 token 化
    strncpy(cmd->paramater, space, sizeof(cmd->paramater) - 1);
    trim_spaces(cmd->paramater);

    return cmd;
}

int split_commands(char *line, char *part[]) {
    int count = 0;

    // 依 '|' 切成每一段 command，供 parser 逐段處理
    // 例："ls -l | cat" -> part[0]="ls -l", part[1]="cat"
    char *token = strtok(line, "|");
    while (token != NULL) {
        trim_spaces(token);
        part[count++] = token;
        token = strtok(NULL, "|");
    }

    return count;
}

int parse_number_pipe_tail(char *line) { // eg : ls |2 -> split into "ls", "|2" and extract the number(delay line)
    char *last = strrchr(line, '|');
    if (last == NULL) return -1;      // 沒有 |
    char *p = last + 1;               // 指到 | 後面
    if (*p == '\0') return -1;        // 只有 | 沒內容

    // 必須全部是數字
    for (char *q = p; *q != '\0'; ++q) {
        if (!isdigit((unsigned char)*q)) return -1;
    }

    // 解析 N
    int n = atoi(p);
    if (n <= 0) return -1;

    // 把 |N 切掉，只留前半段給一般 parser
    *last = '\0';
    trim_spaces(line);

    return n;
}