#define _DEFAULT_SOURCE /* Get on_exit() declaration from <stdlib.h> */
#include <stdlib.h>
#include "myhdr.h"

static void
onexitFunc(int exitStatus, void *arg)
{
     printf("on_exit function called: status=%d, arg=%ld\n",
            exitStatus, (long) arg);
}

int
main(int argc, char *argv[])
{
    if (on_exit(onexitFunc, (void *) 10) != 0) {
        perror("on_exit 1\n");
        exit(1);
    }

    if (on_exit(onexitFunc, (void *) 20) != 0) {
        perror("on_exit 2\n");
        exit(1);
    }

 exit(3);
}
