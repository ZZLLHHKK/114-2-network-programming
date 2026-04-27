CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
TARGET = shell
SRC_DIR = src
OBJ_DIR = object

SRCS = $(SRC_DIR)/shell.c $(SRC_DIR)/string_utils.c
OBJS = $(OBJ_DIR)/shell.o $(OBJ_DIR)/string_utils.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)
