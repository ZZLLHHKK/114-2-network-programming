CC := gcc
CFLAGS := -Wall -Wextra -std=c11
TARGET := copy
SRC := copy.c

SRC_FILE ?= test.txt
DST_FILE ?= output.txt

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET) $(SRC_FILE) $(DST_FILE)

clean:
	rm -f $(TARGET)
