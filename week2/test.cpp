// fd_test.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    printf("預設 fd： stdin=%d, stdout=%d, stderr=%d\n", 
           STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);

    int fd1 = open("test.txt", O_RDONLY);           // READ ONLY flag 輸入檔
    int fd2 = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);  // 輸出檔案
    // O_WRONLY 開寫入 write only 
    // O_CREAT  檔案不存在就建立他
    // O_TRUNC  檔案存在 就清空他

    if (fd1 != -1) printf("開 test.txt 拿到 fd = %d\n", fd1);
    if (fd2 != -1) printf("開 output.txt 拿到 fd = %d\n", fd2);

    close(fd1);
    close(fd2);
    return 0;
}