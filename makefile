# Makefile for building the C program

# Directories
SRC_DIR = src
BIN_DIR = bin

# Files
SRC_FILE = $(SRC_DIR)/main.c
BIN_FILE = $(BIN_DIR)/main

# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Default target
all: $(BIN_FILE)

# Rule to compile the program
$(BIN_FILE): $(SRC_FILE) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_FILE) $(SRC_FILE) -lm

# Ensure the bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up binary and object files
clean:
	rm -rf $(BIN_FILE) $(BIN_DIR)

.PHONY: all clean
