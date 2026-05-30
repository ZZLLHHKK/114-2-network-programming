---
title: homework2 導引
tags: [2023 network programming]

---

# Server

## main()

![image](https://hackmd.io/_uploads/rySGXSZg1x.png)


:::info
1. In main function, build a listen server and
fork a connected server when listen server accepts connection from client.
Then close unused descriptors, listenfd in connected server and connfd in listen server.

2. listen server must be monitor signal SIGCHLD to wait connected server exitence.

3. dup2 connect fd to STDIN, STDOUT in connect server (modified by main of HW1)

4. server() modified from main() of HW1 as following:
:::




![image](https://hackmd.io/_uploads/B1upES-e1g.png)



# external command
## who example

```clike=
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

char userFile[20] = "/tmp/userlist";

void main(int argc, char **argv) {
        int fd;
        unsigned int uid, port, pid;
        char ip[15], name[30];
        char buf[100];

        FILE *fin = fopen(userFile, "r");
        printf("<ID>\t<name>\t<IP:port>\t\t<indicate me>\n\r");

        while(fscanf(fin, "%d %s %s %d %d", &uid, name, ip, &port, &pid)!=EOF) {
                if (pid == atoi(argv[1]))
                   printf("%4d\t%s\t%s:%d\t<-(me)\n\r", uid, name, ip, port);
                else
                   printf("%4d\t%s\t%s:%d\t\n\r", uid, name, ip, port);
        }
        fclose(fin);
}
```

```bash
$gcc who.c -o who

store who in your project bin
$who
```
![image](https://hackmd.io/_uploads/H1OyMBZeJg.png)


![image](https://hackmd.io/_uploads/S1dbGrWlJx.png)


:::info
example: connected server pid is 19398
and connected server call execv(), ```who 19398```.
:::

:::success
/tmp/userlist store uid, name, ip, port, pid

1. change name field of /tmp/userlist when the name is change.

2. every connected server can mantain a fifo to read message in /tmp.

3. then, one server send message to the other can be implement by writing the fifo of destination server.
:::
