
:::success
分解程式
1. A simple program to get line infinite until `quit` is got.
2. parser command
3. check buildin command
4. get non-buildin command and fork process
5. process pipe command `|`
6. process number pipe cammand `|n`, where n is a number. 
:::

# step 1

```c=
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char commandStr[256]={0};
    do {
        printf("MyShell%% ");
        fgets(commandStr, sizeof(commandStr), stdin);
        printf("%s\n", commandStr);
    } while (strcmp(commandStr, "quit") != 0);
}
```

:::info
以上程式 需要 debug一下，有一點點簡單的bug, 請用 vscode 練習一下

沒錯後執行畫面如下，這樣我們就有簡單的shell程式，可以接收 user的 cli command
:::

![image](https://hackmd.io/_uploads/BkNiwr32A.png)



# step 2 parser command string

```c=
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct commandType {
    char command[100];
    char paramater[100];
} command_t;

command_t *parser(char * commandStr) {
    command_t *cmd = (command_t *)malloc(sizeof(command_t));
 command_t *parser(char * commandStr) {
    command_t *cmd = (command_t *)malloc(sizeof(command_t));
    sscanf(commandStr, "%s", cmd->command);
    int x = strlen(cmd->command)+1;
    sscanf(commandStr+x, "%[^\n]", cmd->paramater);
    return cmd;
}
   

int main() {
    char commandStr[256]={0};
    command_t *cmd=NULL;
    do {
        printf("MyShell%% ");
        fgets(commandStr, sizeof(commandStr), stdin);
        cmd = parser(commandStr);
        printf("cmd->command = %s\n",cmd->command);
        printf("cmd->parameter = %s\n", cmd->paramater);
        free(cmd);
    } while (strcmp(commandStr, "quit") != 0);
}
```

:::success
parser command line then store in a data struct command_t for after processing
:::


![image](https://hackmd.io/_uploads/rySBhHhhC.png)

# Step 3 check buildin command

:::success
buildin command in HW1 are `printenv` and `setenv`

if cmd->command == "printenv"
   call a function to process `printenv` (自訂一個函數去處理)
   
if cmd->command == "setenv"
   call a function to process `setenv` (自訂一個函數去處理)
:::


# Step 4 process non-buildin command

:::success
if cmd->command is not buildin command then
    find it from PATH. PATH is string like ".:bin:sbin" etc.
    . denotes current dir, bin denotes bin dir under project, etc.
    if find it then fork a process and run it.
else
    process command not found function.
:::

:::info
參考 week3 即可完成
:::

# Step 5 process pipe command `|`

:::success
如果 cmd->parameter 第一個char 是 `|`
處理 pipe 導向問題

**參考 week4** 範例即可完成 pipe command
:::

# Step 6 process number pipe command `|n`

:::success
如果 cmd->parameter 以 |n where n is number 開頭
那就處理 number pipe 的問題
:::

:::info
參考 week4 pipe or fifo pipe 也許再加上 file 的控制就可以完成
:::

# Step 7 最後功能測試
