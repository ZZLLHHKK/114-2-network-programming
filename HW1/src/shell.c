#include "../include/myhdr.h"

typedef struct pending_pipe {
    bool used; // 這格目前有沒有用
    int target_line; // current_line + number_n
    int fd[2]; // write : 1, read : 0
} pending_pipe_t;

int find_ready_pending_index(int current_line, pending_pipe_t pending[]) { 
    //在 pending 陣列裡找「現在這一行該取用」的那筆，如果沒找到，代表這一行沒有延遲輸入
    for (int i = 0; i < 128; ++i) {
        if (pending[i].used && pending[i].target_line == current_line)
            return i;
    }
    return -1;
}

int find_pending_by_target(int target, pending_pipe_t pending[]) { //找「某個目標行」對應的 pending pipe 是否已經存在
    /*
    當這一行有 |N 時，要建立一條送到 target_line 的 numbered pipe
    但如果已經有人之前也要送到同一個 target_line，就要共用同一條 pipe
    所以先找有沒有現成的
    */
    for (int i = 0; i < 128; ++i) {
         if (pending[i].used && pending[i].target_line == target)
            return i;
    }
    return -1;
}

int alloc_pending_slot(pending_pipe_t pending[]) { // 找一個空位來放新 pending pipe
    for (int i = 0; i < 128; ++i) {
        if (!pending[i].used)
            return i;
    }
    return -1;
}

void consume_ready_pending(int ready_pending_idx, pending_pipe_t pending[]) {
    if (ready_pending_idx == -1)
        return;

    if (pending[ready_pending_idx].fd[READ_END] >= 0)
        close(pending[ready_pending_idx].fd[READ_END]);
    if (pending[ready_pending_idx].fd[WRITE_END] >= 0)
        close(pending[ready_pending_idx].fd[WRITE_END]);
    pending[ready_pending_idx].fd[READ_END] = -1;
    pending[ready_pending_idx].fd[WRITE_END] = -1;
    pending[ready_pending_idx].used = false;
    pending[ready_pending_idx].target_line = 0;
}

// Unknown command 不計入行號：把所有 pending 的 target_line 往後推 1
void shift_all_pending_targets(pending_pipe_t pending[], int size) {
    for (int i = 0; i < size; ++i) {
        if (pending[i].used)
            pending[i].target_line++;
    }
}

int main() {
    char input_line[5002]={0};   // 原始輸入字串（整行指令）
    char *argv_single[256] = {0};
    char line_copy[5002] = {0};  // split_commands 用的可修改副本
    char *parts[1024] = {0};      // 以 '|' 切開後的每個 command 片段
    int part_count = 0;         // 管線中 command 的數量

    int number_n = 0;
    int current_line = 0;
    int target_line = 0; // current_line + number_n for number pipe delay

    // 把「現在還不能送出去的 pipe」先記住，等未來某一行到了再接上去。
    pending_pipe_t pending[512] = {0};
    int ready_pending_idx = -1;
    int target_pending_idx = -1;

    command_t *cmd_single = NULL;
    command_t *cmds[512] = {0}; // 每個 command 解析後的結構
    char *argvs[1024][256] = {0};  // [第幾個command][第幾個參數]
    
    // 固定使用作業路徑，避免混入系統 PATH 造成測試結果不一致
    setenv("PATH", "bin:.", 1);

    do {
        memset(argv_single, 0, sizeof(argv_single));
        printf("%% ");
        fflush(stdout);
        fgets(input_line, sizeof(input_line), stdin);
        input_line[strcspn(input_line, "\n")] = '\0';
        number_n = -1;

        memset(line_copy, 0, sizeof(line_copy));
        memset(parts, 0, sizeof(parts));

        strncpy(line_copy, input_line, sizeof(line_copy) - 1);

        number_n = parse_number_pipe_tail(line_copy);
        part_count = split_commands(line_copy, parts);

        // 空白行不計入行號
        if (part_count == 0) {
            continue;
        }

        // 目前先用「非空行計數」推進 numbered pipe 的行號
        current_line++;
        ready_pending_idx = find_ready_pending_index(current_line, pending);
        if (number_n > 0) {
            target_line = current_line + number_n;
            target_pending_idx = find_pending_by_target(target_line, pending);
            if (target_pending_idx == -1) {
                int slot = alloc_pending_slot(pending);
                if (slot == -1) {
                    fprintf(stderr, "No available pending pipe slot.\n");
                    continue;
                }
                if (pipe(pending[slot].fd) == -1) {
                    perror("pipe");
                    continue;
                }
                pending[slot].used = true;
                pending[slot].target_line = target_line;
                target_pending_idx = slot;
            }
        }
        else 
            target_pending_idx = -1;

        // Parent must not keep writer open on a ready numbered pipe, otherwise reader never sees EOF.
        if (ready_pending_idx != -1 && pending[ready_pending_idx].fd[WRITE_END] >= 0) {
            close(pending[ready_pending_idx].fd[WRITE_END]);
            pending[ready_pending_idx].fd[WRITE_END] = -1;
        }
        

        if (part_count >= 2 || number_n > 0 || ready_pending_idx != -1) {
            /* Rule 5/6: quit 永遠是 builtin，即使有 pending numbered pipe 也不例外 */
            if (part_count == 1) {
                cmd_single = parser(parts[0]);
                if (cmd_single != NULL && cmd_single->command[0] != '\0') {
                    build_argv(cmd_single, argv_single);
                    if (strcmp(argv_single[0], "quit") == 0) {
                        free(cmd_single); cmd_single = NULL;
                        break;
                    }
                }
                free(cmd_single); cmd_single = NULL;
                memset(argv_single, 0, sizeof(argv_single));
            }
            // step1 : reset per-line states
            for (int i = 0; i < part_count; ++i) {
                cmds[i] = NULL;
                memset(argvs[i], 0, sizeof(argvs[i]));
            }

            // step2 : parse all commands
            bool need_abort = 0;
            for (int i = 0; i < part_count; ++i) {
                cmds[i] = parser(parts[i]);
                if (cmds[i] == NULL || cmds[i]->command[0] == '\0') {
                    need_abort = 1;
                    break;
                }
                build_argv(cmds[i], argvs[i]);
            }

            if (need_abort) {
                for (int i = 0; i < part_count; ++i) {
                    if (cmds[i] != NULL) {
                        free(cmds[i]);
                        cmds[i] = NULL;
                    }
                }
                continue;
            }

            // step3 : create pipes
            int fd[part_count - 1][2]; // 1 : write, 0 : read (create part_count - 1 pipe)
            int pipe_ok_count = 0;

            for (int i = 0; i < part_count - 1; ++i) {
                if (pipe(fd[i]) == -1) {
                    perror("pipe");
                    need_abort = 1;
                    break;
                }
                pipe_ok_count++;
            }

            // step4 : fork 
            pid_t pids[20] = {0};
            int child_ok_count = 0;

            for (int i = 0; i < part_count; ++i) {
                pid_t pid = fork();

                if (pid < 0) {
                    perror("fork error");
                    need_abort = 1;
                    break;
                }
                else if (pid == 0) { // child process
                    if (i == 0 && ready_pending_idx != -1) // for number pipe read_end connection
                        dup2(pending[ready_pending_idx].fd[READ_END], STDIN_FILENO);
                    if (i > 0) { // 不是第一個，stdin 接上當前pipe的讀端
                        if (dup2(fd[i - 1][READ_END], STDIN_FILENO) == -1) { // 用左邊pipe的read_end
                            perror("dup2 stdin");
                            exit(EXIT_FAILURE);
                        }
                    }
                    if (i == part_count - 1 && number_n > 0 && target_pending_idx != -1) // for number pipe write_end connection
                        dup2(pending[target_pending_idx].fd[WRITE_END], STDOUT_FILENO);
                    if (i < part_count - 1) { // 不是最後一個，stdout接上當前pipe的寫端
                        if (dup2(fd[i][WRITE_END], STDOUT_FILENO) == -1) { // 用右邊pipe的write_end
                            perror("dup2 stdout");
                            exit(EXIT_FAILURE);
                        }
                    }
                    for (int k = 0; k < pipe_ok_count; ++k) { // close all child pipe
                        close(fd[k][READ_END]);
                        close(fd[k][WRITE_END]);
                    }
                    // close pending pipe
                    for (int i = 0; i < 128; ++i) {
                        if (pending[i].used) {
                            if (i == ready_pending_idx) { // 我(child)讀了這個 pending，要關寫端
                                if (pending[i].fd[WRITE_END] >= 0)
                                    close(pending[i].fd[WRITE_END]);
                            }
                            else if (i == target_pending_idx) { // 我(child)寫了這個 pending，要關讀端
                                if (pending[i].fd[READ_END] >= 0)
                                    close(pending[i].fd[READ_END]);
                            }
                            else { // 這個 pending 我沒用到，兩端都關
                                if (pending[i].fd[READ_END] >= 0)
                                    close(pending[i].fd[READ_END]);
                                if (pending[i].fd[WRITE_END] >= 0)
                                    close(pending[i].fd[WRITE_END]);
                            }
                        }
                    }
                    if (execvp(argvs[i][0], argvs[i]) == -1) {
                        fprintf(stderr, "Unknown command: [%s].\n", argvs[i][0]); // 錯誤訊息走 stderr，不會跟一般輸出同一條管線。
                        exit(127); // 127 = command not found (POSIX standard)
                    }
                }
                else { // pid > 0 parent process
                    pids[child_ok_count++] = pid;
                }
            }
            // final step : unified cleanup for both success/failure before fork is added
            for (int i = 0; i < pipe_ok_count; ++i) {
                close(fd[i][0]);
                close(fd[i][1]);
            }

            int first_status = 0;
            for (int i = 0; i < child_ok_count; ++i) {
                int s;
                waitpid(pids[i], &s, 0);
                if (i == 0) first_status = s;
            }

            // Unknown command 不計入行號：所有 pending target 往後推 1
            if (part_count == 1 &&
                WIFEXITED(first_status) && WEXITSTATUS(first_status) == 127) {
                shift_all_pending_targets(pending, 128);
            } else {
                consume_ready_pending(ready_pending_idx, pending);
            }
            
            for (int i = 0; i < part_count; ++i) {
                if (cmds[i] != NULL) {
                    free(cmds[i]);
                    cmds[i] = NULL;
                }
            }

            continue;
        }
        else if (part_count == 1) {

            cmd_single = parser(parts[0]);

            if (cmd_single->command[0] == '\0') {
                free(cmd_single);
                continue;
            }

            build_argv(cmd_single, argv_single);

            // build-in
            if (strcmp(argv_single[0], "quit") == 0) {
                consume_ready_pending(ready_pending_idx, pending);
                free(cmd_single);
                break;
            }

            if (strcmp(argv_single[0], "printenv") == 0) {
                char *val = getenv(argv_single[1]);
                if (val != NULL) 
                    printf("%s\n", val);
                consume_ready_pending(ready_pending_idx, pending);
                free(cmd_single);
                continue;
            }

            if (strcmp(argv_single[0], "setenv") == 0) {
                setenv(argv_single[1], argv_single[2], 1);
                consume_ready_pending(ready_pending_idx, pending);
                free(cmd_single);
                continue;
            }

            // external command
            pid_t pid = fork();

            if (pid < 0) {
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
                // child
                if (ready_pending_idx != -1) {
                    if (dup2(pending[ready_pending_idx].fd[READ_END], STDIN_FILENO) == -1) {
                        perror("dup2 stdin");
                        exit(EXIT_FAILURE);
                    }
                }
                for (int i = 0; i < 128; ++i) {
                    if (pending[i].used) {
                        if (i == ready_pending_idx) {
                            if (pending[i].fd[WRITE_END] >= 0)
                                close(pending[i].fd[WRITE_END]);
                        }
                        else {
                            if (pending[i].fd[READ_END] >= 0)
                                close(pending[i].fd[READ_END]);
                            if (pending[i].fd[WRITE_END] >= 0)
                                close(pending[i].fd[WRITE_END]);
                        }
                    }
                }
                if (execvp(argv_single[0], argv_single) == -1) {
                    fprintf(stderr, "Unknown command: [%s].\n", argv_single[0]);
                    exit(127); // 127 = command not found (POSIX standard)
                }
            }
            else {
                // parent (avoid zombie process)
                int status;
                waitpid(pid, &status, 0);
                // Unknown command 不計入行號：所有 pending target 往後推 1
                if (WIFEXITED(status) && WEXITSTATUS(status) == 127) {
                    shift_all_pending_targets(pending, 128);
                } else {
                    consume_ready_pending(ready_pending_idx, pending);
                }
            }
            free(cmd_single);
        }
        else {
            // 空輸入（例如只按 Enter）
            continue;
        }
    } while (1);
}
