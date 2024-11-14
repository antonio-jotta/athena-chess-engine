# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -pedantic -std=c++11 -Iinclude

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TEST_DIR = tests

# Executable names
EXEC = athena
TEST_EXEC = move_generation

# Source files
SRC_FILES := $(filter-out $(SRC_DIR)/athena.cpp, $(wildcard $(SRC_DIR)/*.cpp))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))
ATHENA_OBJ := $(BUILD_DIR)/athena.o

TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJ_FILES := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_FILES))

# Default target
all: $(EXEC) $(TEST_EXEC)

# Build the main executable
$(EXEC): $(OBJ_FILES) $(ATHENA_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_FILES) $(ATHENA_OBJ)

# Build the tests executable
$(TEST_EXEC): $(OBJ_FILES) $(TEST_OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_FILES) $(TEST_OBJ_FILES)

# Compile athena.cpp separately (contains main)
$(ATHENA_OBJ): $(SRC_DIR)/athena.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rules for compiling source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rules for compiling test files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up object files and executables
clean:
	rm -f $(BUILD_DIR)/*.o $(EXEC) $(TEST_EXEC)

# Phony targets
.PHONY: all clean
