#ifndef STRING_UTILS_H
#define STRING_UTILS_H

typedef struct commandType {
	char command[100];
	char paramater[100];
} command_t;

void trim_spaces(char *str);
int split_commands(char *line, char *parts[]);
void build_argv(command_t *cmd, char *argv[]);
command_t *parser(char *commandStr);
int parse_number_pipe_tail(char *line);

#endif