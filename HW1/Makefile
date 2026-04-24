CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = shell
SRCS = src/shell.c src/string_utils.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
