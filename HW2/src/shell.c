#include "../include/myhdr.h"

void shell_loop() {
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
    
    do {
        memset(argv_single, 0, sizeof(argv_single));
        printf("%% ");
        fflush(stdout);
        if (fgets(input_line, sizeof(input_line), stdin) == NULL)
            break; // EOF 或連線斷開，結束 shell_loop
        input_line[strcspn(input_line, "\r\n")] = '\0'; // 去掉 \n 和 telnet 送來的 \r
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

        if (part_count >= 2 || number_n > 0 || ready_pending_idx != -1) 
            run_pipeline_command(part_count, cmds, argvs, parts, ready_pending_idx, pending, number_n, target_pending_idx);
        else if (part_count == 1) {
            if (run_single_command(cmd_single, parts, argv_single, ready_pending_idx, pending) == 1)
                break; // quit 指令
        } else // 空輸入（例如只按 Enter）
            continue;
        
    } while (1);
}

int main() {
    // 依照執行檔所在目錄設定 PATH，避免在其他 cwd 啟動時找不到 bin/*
    char exe_path[4096] = {0};
    ssize_t n = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (n > 0) {
        exe_path[n] = '\0';
        char *slash = strrchr(exe_path, '/');
        if (slash != NULL) {
            *slash = '\0';
            if (chdir(exe_path) != 0)
                perror("chdir");
            char path_env[8192] = {0};
            snprintf(path_env, sizeof(path_env), "%s/bin:.", exe_path);
            setenv("PATH", path_env, 1);
        } else {
            setenv("PATH", "bin:.", 1);
        }
    } else {
        setenv("PATH", "bin:.", 1);
    }

    start_server(7001, shell_loop); // 1024-65535 任意數字, 0-1023需要root才能用
}
