CC = cc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = main
SOURCES = arraylist.c hashmap.c json.c main.c
BUILD_DIR = build
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
HEADERS = arraylist.h hashmap.h json.h thirdparty/jimp.h thirdparty/nob.h

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BUILD_DIR)
	$(CC) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

.PHONY: all
