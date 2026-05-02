#ifndef MYHDR_H
#define MYHDR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "string_utils.h"
#include "number_pipe.h"
#include "shell_exec.h"

#include "network_server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

// READ_END / WRITE_END 已移至 number_pipe.h

#endif