# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -pedantic -std=c++11

# Executable names
EXEC = athena
TEST_EXEC = tests

# Source files
SRC = athena.cpp board.cpp move_generator.cpp tests.cpp
OBJ = athena.o board.o move_generator.o tests.o

# Default target
all: $(EXEC) $(TEST_EXEC)

# Link the object files to create the executable
$(EXEC): athena.o board.o move_generator.o
	$(CXX) $(CXXFLAGS) -o $(EXEC) athena.o board.o move_generator.o

# Build the tests executable
$(TEST_EXEC): tests.o board.o move_generator.o
	$(CXX) $(CXXFLAGS) -o $(TEST_EXEC) tests.o board.o move_generator.o

# Compilation rules
athena.o: athena.cpp board.h move_generator.h move.h
	$(CXX) $(CXXFLAGS) -c athena.cpp -o athena.o

board.o: board.cpp board.h move.h
	$(CXX) $(CXXFLAGS) -c board.cpp -o board.o

move_generator.o: move_generator.cpp move_generator.h board.h move.h
	$(CXX) $(CXXFLAGS) -c move_generator.cpp -o move_generator.o

tests.o: tests.cpp board.h move_generator.h move.h
	$(CXX) $(CXXFLAGS) -c tests.cpp -o tests.o

# Clean up object files and executables
clean:
	rm -f $(OBJ) $(EXEC) $(TEST_EXEC)

# Phony targets
.PHONY: all clean
