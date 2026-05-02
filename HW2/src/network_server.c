#define _POSIX_C_SOURCE 200809L // 避免 POSIX 宣告解析不完整
#include "../include/myhdr.h"

static void sigchld_handler(int sig) { // when child process is over, we have to close them
    (void)sig; // 掉 unused parameter 警告
    while (waitpid(-1, NULL, WNOHANG) > 0); // WNOHANG 表示「不要卡住，沒有就馬上回來」。
}

void start_server(int port, void (*child_main)()) {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    pid_t childpid;

    // 1. create socket 建立通訊端點
    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // SOCK_STREAM = TCP（可靠的、像打電話）。
    if (listenfd < 0) { perror("socket"); exit(1); }

    // 重啟程式時讓 OS 立刻重用同一個 port，避免 "Bind : Address already in use"
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. bind 設定連接 port
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY = 「我所有網路卡都聽」（不管你是從 WiFi 還是 localhost 打來都行）
    servaddr.sin_port = htons(port); // htons(port) = 把 port 轉成網路正確的格式。

    // bind = 把電話插上牆壁的分機插座（Port）。
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind"); 
        exit(1);
    }

    // 3. listen = 總機小姐開始上班，準備接電話，最多讓 5 個人排隊。
    if (listen(listenfd, 5) < 0) { 
        perror("listen"); 
        exit(1); 
    }

    // 4. 設定 SIGCHLD handler
    struct sigaction act = {0};
    act.sa_handler = sigchld_handler;
    if (sigaction(SIGCHLD, &act, NULL) < 0) {
        perror("sigaction");
        exit(1);
    }

    // 5. accept/fork/dup2
    while (1) { // 無窮迴圈等電話。
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        if (connfd < 0) {
            if (errno == EINTR) continue;
            perror("accept error");
            exit(1);
        }
        if ((childpid = fork()) == 0) {   // 進入 child process (生一個外送員)
            close(listenfd);              // child 不需要總機電話
            dup2(connfd, STDIN_FILENO);   // 把耳朵接到電話線
            dup2(connfd, STDOUT_FILENO);  // 把嘴巴接到電話線
            close(connfd);                // 已經複製過了，關掉原本的
            child_main();                 // 執行你寫的「服務顧客」函數
            exit(0);
        }
        close(connfd); // Parent 這邊關掉
    }
}