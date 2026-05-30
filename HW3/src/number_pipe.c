#include "../include/myhdr.h"

int find_ready_pending_index(int current_line, pending_pipe_t pending[]) {
    // 在 pending 陣列裡找「現在這一行該取用」的那筆
    for (int i = 0; i < 128; ++i) {
        if (pending[i].used && pending[i].target_line == current_line)
            return i;
    }
    return -1;
}

int find_pending_by_target(int target, pending_pipe_t pending[]) {
    // 找「某個目標行」對應的 pending pipe 是否已經存在
    for (int i = 0; i < 128; ++i) {
        if (pending[i].used && pending[i].target_line == target)
            return i;
    }
    return -1;
}

int alloc_pending_slot(pending_pipe_t pending[]) {
    // 找一個空位來放新 pending pipe
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
    pending[ready_pending_idx].fd[READ_END]   = -1;
    pending[ready_pending_idx].fd[WRITE_END]  = -1;
    pending[ready_pending_idx].used           = false;
    pending[ready_pending_idx].target_line    = 0;
}

/* Unknown command 不計入行號：把所有 pending 的 target_line 往後推 1 */
void shift_all_pending_targets(pending_pipe_t pending[], int size) {
    for (int i = 0; i < size; ++i) {
        if (pending[i].used)
            pending[i].target_line++;
    }
}
