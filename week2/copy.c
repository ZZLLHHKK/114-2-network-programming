#include <sys/stat.h>     // S_IRUSR 等權限
#include <fcntl.h>        // open()
#include <stdio.h>        // printf, perror
#include <stdlib.h>       // exit()
#include <unistd.h>       // read, write, close
#include <errno.h>        // errno
#include <string.h>       // ←←← 這行最重要！加上 strerror

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[])
{
    int inputFd, outputFd, openFlags;
    mode_t filePerms;   // file permission and type
    ssize_t numRead;
    char buf[BUF_SIZE];

    /* ==================== 最重要的防崩潰檢查 ==================== */
    if (argc != 3) {
        fprintf(stderr, "用法: %s 來源檔 目標檔\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    /* ========================================================== */

    /* 開來源檔 */
    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1) {
        fprintf(stderr, "%s: 無法開啟來源檔 %s: %s\n", 
                argv[0], argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* 準備目標檔旗標與權限 */
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                S_IROTH | S_IWOTH;          // 0666 權限

    /* 開目標檔 */
    outputFd = open(argv[2], openFlags, filePerms);
    if (outputFd == -1) {
        fprintf(stderr, "%s: 無法開啟目標檔 %s: %s\n", 
                argv[0], argv[2], strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("fd1=%d, fd2=%d\n", inputFd, outputFd);   // 方便你看到 fd

    /* 核心複製迴圈 */
    while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, numRead) != numRead) {
            perror("write 失敗或只寫了一部分");
            exit(EXIT_FAILURE);
        }
    }

    if (numRead == -1)
        perror("read 失敗");

    close(inputFd);
    close(outputFd);

    printf("複製完成！\n");
    exit(EXIT_SUCCESS);
}