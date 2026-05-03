CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
TARGET = shell
SRC_DIR = src
OBJ_DIR = object
BIN_DIR = bin

SRCS = $(SRC_DIR)/shell.c $(SRC_DIR)/string_utils.c $(SRC_DIR)/network_server.c $(SRC_DIR)/number_pipe.c $(SRC_DIR)/shell_exec.c
OBJS = $(OBJ_DIR)/shell.o $(OBJ_DIR)/string_utils.o $(OBJ_DIR)/network_server.o $(OBJ_DIR)/number_pipe.o $(OBJ_DIR)/shell_exec.o

# bin/ 底下每個 .c 各自編成同名執行檔
BIN_SRCS = $(wildcard $(BIN_DIR)/*.c)
BIN_EXES = $(BIN_SRCS:.c=)

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

clean:
	rm -f $(TARGET) $(OBJS) $(BIN_EXES)

run: $(TARGET)
	./$(TARGET)
