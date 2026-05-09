#define _POSIX_C_SOURCE 200809L // 避免 POSIX 宣告解析不完整
#include "../include/myhdr.h"

static void sigchld_handler(int sig) { // when child process is over, we have to close them
    (void)sig; // 掉 unused parameter 警告
    while (waitpid(-1, NULL, WNOHANG) > 0); // WNOHANG 表示「不要卡住，沒有就馬上回來」。
}

static void do_login() {
    char user_name[256], password[256];

    while (1) {
        // 1. ask for login
        printf("Login: ");
        fflush(stdout);
        fgets(user_name, sizeof(user_name), stdin);
        user_name[strcspn(user_name, "\r\n")] = '\0';  // 去掉 telnet 傳來的 \r\n

        printf("Password: ");
        fflush(stdout);
        fgets(password, sizeof(password), stdin);
        password[strcspn(password, "\r\n")] = '\0';  // 去掉 telnet 傳來的 \r\n

        // 2. call bin/login
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "bin/login '%s' '%s'", user_name, password);

        FILE *fp = popen(cmd, "r"); // pipe open 開一個子程序，把它的 stdout 當作 FILE* 來讀
        int res = -1;
        fscanf(fp, "%d", &res);      // 讀 bin/login 印出來的那個數字（0/1/2）
        pclose(fp); // pipe close  關閉子程序

        // 3. three-case
        if (res == 0) { // success login
            setenv("MY_NAME", user_name, 1); // 把帳號存到環境變數，供 shell prompt 使用
            break;
        }
        else if (res == 1) { // pswd error
            printf("Password error !\n");
            fflush(stdout);
        } 
        else { // return 2 user not found
            char opt_buf[8];
            printf("User not found !\n");
            printf("Create account or login again ? <1/2> : ");
            fflush(stdout);
            fgets(opt_buf, sizeof(opt_buf), stdin);
            int option = atoi(opt_buf);

            // 1. adding a new user account
            // 2. backing to login page
            if (option == 1) {
                char new_user_name[256], new_password[256];
                while (1) {
                    printf("your user name: ");
                    fflush(stdout);
                    fgets(new_user_name, sizeof(new_user_name), stdin);
                    new_user_name[strcspn(new_user_name, "\r\n")] = '\0';  // 去掉 telnet 傳來的 \r\n

                    printf("your password: ");
                    fflush(stdout);
                    fgets(new_password, sizeof(new_password), stdin);
                    new_password[strcspn(new_password, "\r\n")] = '\0';  // 去掉 telnet 傳來的 \r\n

                    char new_account[1024];
                    snprintf(new_account, sizeof(new_account), "bin/register '%s' '%s'", new_user_name, new_password);

                    FILE *rfp = popen(new_account, "r");
                    int rres = -1;
                    fscanf(rfp, "%d", &rres);
                    pclose(rfp);

                    if (rres == 0) {
                        printf("Create success !\n");
                        fflush(stdout);
                        break;  // 建立成功，跳出 register loop，外層 while(1) 會回到 Login:
                    } else if (rres == 1) {
                        printf("User name already exist !\n");
                        fflush(stdout);
                        // 繼續問
                    }
                }
            }
        }
    }
    
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

            // === 註冊使用者到 /tmp/userlist ===
            // open() 回傳 fd（file descriptor，整數，是這個檔案的「手柄」）
            // O_RDWR  = 可讀可寫
            // O_CREAT = 如果檔案不存在就建立
            // 0644    = 檔案權限（owner 可讀寫，其他人可讀）
            int fd = open("/tmp/userlist", O_RDWR | O_CREAT, 0644);

            // flock() 鎖住這個 fd
            // LOCK_EX = exclusive lock（排他鎖）：只有我能寫，其他人等我解鎖
            // 這樣兩個 client 同時連線才不會互相覆蓋或拿到同一個 uid
            flock(fd, LOCK_EX);

            // fdopen() 把 fd 包成 FILE*，這樣可以用 fprintf/fscanf 等高階函式
            // "a+" = 追加模式（append）：新資料寫在最後一行；同時也可以讀
            FILE *fp = fdopen(fd, "a+");

            // 掃描現有 uid
            bool used[1024] = {false};
            unsigned int existing_uid;
            while (fscanf(fp, "%u %*s %*s %*u %*d", &existing_uid) == 1)
                used[existing_uid] = true;

            // 找最小未使用 uid
            unsigned int new_uid = 0;
            while (new_uid < 1024 && used[new_uid]) new_uid++;

            // 追加寫入
            fseek(fp, 0, SEEK_END);
            char *ip = inet_ntoa(cliaddr.sin_addr); // ip字串 
            unsigned int port = ntohs(cliaddr.sin_port); // port整數 (network to host short)

            // fprintf() 格式化寫入一行：uid name ip port pid
            // getpid() = 取得目前 child process 的 pid，之後用來辨識「我自己」
            fprintf(fp, "%u no_name %s %u %d\n", new_uid, ip, port, getpid());

            // fflush() 強制把 buffer 裡的資料真正寫到檔案，避免資料還卡在 buffer
            fflush(fp);

            // LOCK_UN = unlock，讓下一個在等的 process 可以進來
            flock(fd, LOCK_UN);

            // fclose() 關閉 FILE*（同時也會關閉底層的 fd）
            fclose(fp);
            // ==================================

            // === 建立自己的 FIFO ===
            char fifo_path[64];
            snprintf(fifo_path, sizeof(fifo_path), "/tmp/user_fifo_%u", new_uid);
            mkfifo(fifo_path, 0666);
            // ====================

            dup2(connfd, STDIN_FILENO);   // 把耳朵接到電話線
            dup2(connfd, STDOUT_FILENO);  // 把嘴巴接到電話線
            close(connfd);                // 已經複製過了，關掉原本的
            // 把自己的 pid/uid 存到環境變數
            char pid_str[20];
            snprintf(pid_str, sizeof(pid_str), "%d", getpid());
            setenv("MY_PID", pid_str, 1);
            char uid_str[20];
            snprintf(uid_str, sizeof(uid_str), "%u", new_uid);
            setenv("MY_UID", uid_str, 1);

            // === fork fifo_reader：專門讀自己的 FIFO 並印到畫面 ===
            pid_t reader_pid = fork();
            if (reader_pid == 0) {
                // fifo_reader subprocess
                // stdin 已經沒用了，關掉避免干擾
                close(STDIN_FILENO);
                while (1) {
                    // open 會 blocking，直到有人打開 write 端
                    int fifo_fd = open(fifo_path, O_RDONLY);
                    if (fifo_fd < 0) break;
                    char buf[512];
                    ssize_t n;
                    while ((n = read(fifo_fd, buf, sizeof(buf))) > 0)
                        write(STDOUT_FILENO, buf, n);
                    close(fifo_fd);
                    // 讀完一次（write 端關掉）就再 loop，重新 open 等下一次
                }
                exit(0);
            }
            // =====================================================

            do_login();                   // 登入／註冊流程
            child_main();                 // 執行「服務顧客」函數

            // === 斷線時從 /tmp/userlist 移除自己那行 ===
            // 避免每次重連 uid 會一直累加，舊的殘留條目也不會清除。

            // 先 kill fifo_reader，讓它結束
            kill(reader_pid, SIGTERM);
            waitpid(reader_pid, NULL, 0);
            {
                int ufd = open("/tmp/userlist", O_RDWR, 0644);
                if (ufd >= 0) {
                    flock(ufd, LOCK_EX);
                    FILE *ufin = fdopen(ufd, "r+");

                    // 把所有行讀進暫存，跳過自己那行
                    char lines[1024][128];
                    int count = 0;
                    unsigned int u_uid, u_port;
                    int u_pid;
                    char u_name[30], u_ip[16];
                    while (fscanf(ufin, "%u %29s %15s %u %d",
                                  &u_uid, u_name, u_ip, &u_port, &u_pid) == 5) {
                        if (u_pid != getpid() && count < 1024) {
                            snprintf(lines[count++], sizeof(lines[0]),
                                     "%u %s %s %u %d\n",
                                     u_uid, u_name, u_ip, u_port, u_pid);
                        }
                    }

                    // 清空並重寫
                    rewind(ufin);
                    ftruncate(ufd, 0);
                    for (int i = 0; i < count; i++)
                        fputs(lines[i], ufin);
                    fflush(ufin);
                    flock(ufd, LOCK_UN);
                    fclose(ufin);
                }
            }
            // ==========================================

            // === 刪除自己的 FIFO ===
            unlink(fifo_path);
            // ======================

            exit(0);
        }
        close(connfd); // Parent 這邊關掉
    }
}