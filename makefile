# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -pedantic -Wextra -Iinclude

# Directories
SRC_DIR = src
BUILD_DIR = build
TESTS_DIR = tests
INCLUDE_DIR = include

# Source files
MAIN_SRC = $(SRC_DIR)/athena.cpp
MAIN_OBJ = $(BUILD_DIR)/athena.o
SRCS = $(filter-out $(MAIN_SRC), $(wildcard $(SRC_DIR)/*.cpp))
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TEST_SRCS = $(wildcard $(TESTS_DIR)/*.cpp)
TEST_OBJS = $(patsubst $(TESTS_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

# Executables
EXEC = athena
TEST_EXEC = move_generation_test evaluation_test

# Default target
all: $(EXEC) $(TEST_EXEC)

# Build main executable
$(EXEC): $(MAIN_OBJ) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build move_generation_test executable
move_generation_test: $(BUILD_DIR)/move_generation_test.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build evaluation_test executable
evaluation_test: $(BUILD_DIR)/evaluation_test.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile main source file
$(MAIN_OBJ): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(BUILD_DIR)/%.o: $(TESTS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(BUILD_DIR)/*.o $(EXEC) $(TEST_EXEC)

.PHONY: all clean
