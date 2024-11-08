# Directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

# Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
BIN_FILE = $(BIN_DIR)/converter

# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Default target
all: $(BIN_FILE)

# Rule to compile the program
$(BIN_FILE): $(OBJ_FILES) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_FILE) $(OBJ_FILES) -lm

# Rule to compile each .c file to an object file in the obj directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure the bin and obj directories exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up binary, object files, and directories
clean:
	rm -rf $(BIN_FILE) $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
