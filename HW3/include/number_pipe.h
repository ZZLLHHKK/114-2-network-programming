#ifndef NUMBER_PIPE_H
#define NUMBER_PIPE_H

#include <stdbool.h>
#include <unistd.h>

#define READ_END  0
#define WRITE_END 1

typedef struct pending_pipe {
    bool used;           // 這格目前有沒有用
    int  target_line;    // current_line + number_n
    int  fd[2];          // write : 1, read : 0
} pending_pipe_t;

int  find_ready_pending_index(int current_line, pending_pipe_t pending[]);
int  find_pending_by_target(int target, pending_pipe_t pending[]);
int  alloc_pending_slot(pending_pipe_t pending[]);
void consume_ready_pending(int ready_pending_idx, pending_pipe_t pending[]);

#endif
