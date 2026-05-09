#include "../include/myhdr.h"

// 回傳 1 表示要 quit，0 表示繼續
int run_single_command(command_t *cmd_single, char **parts, char **argv_single,
                       int ready_pending_idx, pending_pipe_t pending[]) {
    cmd_single = parser(parts[0]);

    if (cmd_single->command[0] == '\0') {
        free(cmd_single);
        return 0;
    }

    build_argv(cmd_single, argv_single);

    // built-in: quit
    if (strcmp(argv_single[0], "quit") == 0) {
        consume_ready_pending(ready_pending_idx, pending);
        free(cmd_single);
        return 1;
    }

    // built-in: printenv
    if (strcmp(argv_single[0], "printenv") == 0) {
        char *val = getenv(argv_single[1]);
        if (val != NULL)
            printf("%s\n", val);
        consume_ready_pending(ready_pending_idx, pending);
        free(cmd_single);
        return 0;
    }

    // built-in: setenv
    if (strcmp(argv_single[0], "setenv") == 0) {
        setenv(argv_single[1], argv_single[2], 1);
        consume_ready_pending(ready_pending_idx, pending);
        free(cmd_single);
        return 0;
    }

    // who/name/tell/yell 目前先走 external command（由 bin/ 下可執行檔處理）

    // external command
    pid_t pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
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
                } else {
                    if (pending[i].fd[READ_END] >= 0)
                        close(pending[i].fd[READ_END]);
                    if (pending[i].fd[WRITE_END] >= 0)
                        close(pending[i].fd[WRITE_END]);
                }
            }
        }
        if (execvp(argv_single[0], argv_single) == -1) {
            fprintf(stderr, "Unknown command: [%s].\n", argv_single[0]);
            exit(127);
        }
    } else {
        // parent
        int status;
        waitpid(pid, &status, 0);
        if (ready_pending_idx != -1 &&
            WIFEXITED(status) && WEXITSTATUS(status) == 127) {
            pending[ready_pending_idx].target_line++;
        } else {
            consume_ready_pending(ready_pending_idx, pending);
        }
    }
    free(cmd_single);
    return 0;
}

void run_pipeline_command(int part_count, command_t *cmds[], char *argvs[][256],
                          char **parts, int ready_pending_idx, pending_pipe_t pending[],
                          int number_n, int target_pending_idx) {
    // step1: reset per-line states
    for (int i = 0; i < part_count; ++i) {
        cmds[i] = NULL;
        memset(argvs[i], 0, 256 * sizeof(char *));
    }

    // step2: parse all commands
    bool need_abort = false;
    for (int i = 0; i < part_count; ++i) {
        cmds[i] = parser(parts[i]);
        if (cmds[i] == NULL || cmds[i]->command[0] == '\0') {
            need_abort = true;
            break;
        }
        build_argv(cmds[i], argvs[i]);
    }

    if (need_abort) {
        for (int i = 0; i < part_count; ++i) {
            if (cmds[i] != NULL) { free(cmds[i]); cmds[i] = NULL; }
        }
        return;
    }

    // step3: create pipes
    int fd[part_count - 1][2];
    int pipe_ok_count = 0;
    for (int i = 0; i < part_count - 1; ++i) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            need_abort = true;
            break;
        }
        pipe_ok_count++;
    }

    // step4: fork
    pid_t pids[20] = {0};
    int child_ok_count = 0;
    for (int i = 0; i < part_count; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork error");
            need_abort = true;
            break;
        } else if (pid == 0) {
            if (i == 0 && ready_pending_idx != -1)
                dup2(pending[ready_pending_idx].fd[READ_END], STDIN_FILENO);
            if (i > 0) {
                if (dup2(fd[i - 1][READ_END], STDIN_FILENO) == -1) {
                    perror("dup2 stdin"); exit(EXIT_FAILURE);
                }
            }
            if (i == part_count - 1 && number_n > 0 && target_pending_idx != -1)
                dup2(pending[target_pending_idx].fd[WRITE_END], STDOUT_FILENO);
            if (i < part_count - 1) {
                if (dup2(fd[i][WRITE_END], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout"); exit(EXIT_FAILURE);
                }
            }
            for (int k = 0; k < pipe_ok_count; ++k) {
                close(fd[k][READ_END]);
                close(fd[k][WRITE_END]);
            }
            for (int j = 0; j < 128; ++j) {
                if (pending[j].used) {
                    if (j == ready_pending_idx) {
                        if (pending[j].fd[WRITE_END] >= 0)
                            close(pending[j].fd[WRITE_END]);
                    } else if (j == target_pending_idx) {
                        if (pending[j].fd[READ_END] >= 0)
                            close(pending[j].fd[READ_END]);
                    } else {
                        if (pending[j].fd[READ_END] >= 0)  close(pending[j].fd[READ_END]);
                        if (pending[j].fd[WRITE_END] >= 0) close(pending[j].fd[WRITE_END]);
                    }
                }
            }
            if (execvp(argvs[i][0], argvs[i]) == -1) {
                fprintf(stderr, "Unknown command: [%s].\n", argvs[i][0]);
                exit(127);
            }
        } else {
            pids[child_ok_count++] = pid;
        }
    }

    // cleanup pipes
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

    if (part_count == 1 && ready_pending_idx != -1 &&
        WIFEXITED(first_status) && WEXITSTATUS(first_status) == 127) {
        pending[ready_pending_idx].target_line++;
    } else {
        consume_ready_pending(ready_pending_idx, pending);
    }

    for (int i = 0; i < part_count; ++i) {
        if (cmds[i] != NULL) { free(cmds[i]); cmds[i] = NULL; }
    }
}
