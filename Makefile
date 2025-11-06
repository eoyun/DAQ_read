# Directories
SRC_DIR := src
INC_DIR := include
LIB_DIR := lib

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -I$(INC_DIR) -fPIC
LDFLAGS := -shared

# Target library
LIB_NAME := libmylibrary.so
LIB_PATH := $(LIB_DIR)/$(LIB_NAME)

# Source files and objects
SRC_FILES := $(wildcard $(SRC_DIR)/*.cc)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cc, $(SRC_DIR)/%.o, $(SRC_FILES))

# Default target
all: $(LIB_PATH)

# Compile the shared library
$(LIB_PATH): $(OBJ_FILES)
	mkdir -p $(LIB_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^

# Compile source files to object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(SRC_DIR)/*.o $(LIB_DIR)/$(LIB_NAME)

.PHONY: all clean
