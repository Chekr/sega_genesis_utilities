CC=g++
CFLAGS=-I.
MKDIR_P=mkdir -p
BUILD_DIR=../bin

.PHONY: directory program all clean

all: $(BUILD_DIR)/program

directory: ${BUILD_DIR}

$(BUILD_DIR):
	${MKDIR_P} ${BUILD_DIR}

$(BUILD_DIR)/program: | directory
	$(CC) -o ${BUILD_DIR}/palette_maker.elf main.cpp

clean:
	rm -rf $(BUILD_DIR)
