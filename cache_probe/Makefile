CC=gcc
BUILD_DIR=build
TARGET_NAME=cache_probe

$(BUILD_DIR)/$(TARGET_NAME): main.c | $(BUILD_DIR)
	$(CC) main.c -I . -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)


