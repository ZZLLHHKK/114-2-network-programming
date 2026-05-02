#ifndef SHELL_EXEC_H
#define SHELL_EXEC_H

#include "number_pipe.h"
#include "string_utils.h"

// 回傳 1 表示要 quit，0 表示繼續
int  run_single_command(command_t *cmd_single, char **parts, char **argv_single,
                        int ready_pending_idx, pending_pipe_t pending[]);

void run_pipeline_command(int part_count, command_t *cmds[], char *argvs[][256],
                          char **parts, int ready_pending_idx, pending_pipe_t pending[],
                          int number_n, int target_pending_idx);

#endif
