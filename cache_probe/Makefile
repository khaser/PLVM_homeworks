CC=gcc
CFLAGS=-O2
BUILD_DIR=build_
TARGET_NAME=cache_probe

build: $(BUILD_DIR)/$(TARGET_NAME)

$(BUILD_DIR)/$(TARGET_NAME): main.c | $(BUILD_DIR)
	$(CC) main.c $(CFLAGS) -I . -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

