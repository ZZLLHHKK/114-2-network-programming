CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
TARGET = shell
SRC_DIR = src
OBJ_DIR = object
BIN_DIR = bin

SRCS = $(SRC_DIR)/shell.c $(SRC_DIR)/string_utils.c $(SRC_DIR)/network_server.c $(SRC_DIR)/number_pipe.c $(SRC_DIR)/shell_exec.c
OBJS = $(OBJ_DIR)/shell.o $(OBJ_DIR)/string_utils.o $(OBJ_DIR)/network_server.o $(OBJ_DIR)/number_pipe.o $(OBJ_DIR)/shell_exec.o

# MySQL flags
MYSQL_FLAGS = $(shell mysql_config --cflags --libs)

# bin/ 底下每個 .c 各自編成同名執行檔（login/register 需要 MySQL，單獨列規則）
MYSQL_BINS = $(BIN_DIR)/login $(BIN_DIR)/register $(BIN_DIR)/mailto $(BIN_DIR)/listMail $(BIN_DIR)/delMail $(BIN_DIR)/createGroup $(BIN_DIR)/listGroup $(BIN_DIR)/addTo $(BIN_DIR)/leaveGroup $(BIN_DIR)/remove $(BIN_DIR)/delGroup $(BIN_DIR)/gyell
BIN_SRCS = $(filter-out $(BIN_DIR)/login.c $(BIN_DIR)/register.c $(BIN_DIR)/mailto.c $(BIN_DIR)/listMail.c $(BIN_DIR)/delMail.c $(BIN_DIR)/createGroup.c $(BIN_DIR)/listGroup.c $(BIN_DIR)/addTo.c $(BIN_DIR)/leaveGroup.c $(BIN_DIR)/remove.c $(BIN_DIR)/delGroup.c $(BIN_DIR)/gyell.c, $(wildcard $(BIN_DIR)/*.c))
BIN_EXES = $(BIN_SRCS:.c=) $(MYSQL_BINS)

.PHONY: all clean run

all: $(TARGET) $(BIN_EXES)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# bin/*.c → bin/* （不需要 -Iinclude，who/name/tell/yell 只用標準函式庫）
$(BIN_DIR)/%: $(BIN_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

# MySQL 指令（login/register）
$(BIN_DIR)/login: $(BIN_DIR)/login.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS) -lcrypt

$(BIN_DIR)/register: $(BIN_DIR)/register.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS) -lcrypt

$(BIN_DIR)/mailto: $(BIN_DIR)/mailto.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/listMail: $(BIN_DIR)/listMail.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/delMail: $(BIN_DIR)/delMail.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/createGroup: $(BIN_DIR)/createGroup.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/listGroup: $(BIN_DIR)/listGroup.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/addTo: $(BIN_DIR)/addTo.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/leaveGroup: $(BIN_DIR)/leaveGroup.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/remove: $(BIN_DIR)/remove.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/delGroup: $(BIN_DIR)/delGroup.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

$(BIN_DIR)/gyell: $(BIN_DIR)/gyell.c
	$(CC) $(CFLAGS) $< -o $@ $(MYSQL_FLAGS)

clean:
	rm -f $(TARGET) $(OBJS) $(BIN_EXES)

run: $(TARGET)
	-fuser -k 7001/tcp 2>/dev/null; true
	> /tmp/userlist
	rm -f /tmp/user_fifo_*
	./$(TARGET)
