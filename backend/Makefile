# Makefile for Spotify DSA Recommendation System
# Academic Project - C++ Implementation

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
RELEASE_FLAGS = -O2
DEBUG_FLAGS = -g -DDEBUG
SANITIZE_FLAGS = -fsanitize=address -fsanitize=undefined

# Target executable
TARGET = recommendation_engine
DEBUG_TARGET = recommendation_engine_debug
SANITIZE_TARGET = recommendation_engine_asan

# Source files
SOURCES = main.cpp

# Default target
all: release

# Release build (optimized)
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_TARGET)

# Sanitizer build (for memory leak detection)
sanitize: CXXFLAGS += $(SANITIZE_FLAGS) $(DEBUG_FLAGS)
sanitize: $(SANITIZE_TARGET)

# Build target
$(TARGET): $(SOURCES)
	@echo "Compiling $(TARGET) (Release)..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
	@echo "✓ Build complete: $(TARGET)"

$(DEBUG_TARGET): $(SOURCES)
	@echo "Compiling $(DEBUG_TARGET) (Debug)..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(DEBUG_TARGET)
	@echo "✓ Build complete: $(DEBUG_TARGET)"

$(SANITIZE_TARGET): $(SOURCES)
	@echo "Compiling $(SANITIZE_TARGET) (Sanitizers)..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(SANITIZE_TARGET)
	@echo "✓ Build complete: $(SANITIZE_TARGET)"

# Run the program
run: $(TARGET)
	@echo "Running $(TARGET)..."
	@./$(TARGET)

# Run debug version
run-debug: $(DEBUG_TARGET)
	@echo "Running $(DEBUG_TARGET)..."
	@./$(DEBUG_TARGET)

# Run with sanitizers
run-sanitize: $(SANITIZE_TARGET)
	@echo "Running $(SANITIZE_TARGET) with sanitizers..."
	@./$(SANITIZE_TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -f $(TARGET) $(DEBUG_TARGET) $(SANITIZE_TARGET)
	@rm -f *.o *.out
	@echo "✓ Clean complete"

# Test compilation (compile without running)
test-compile: $(TARGET)
	@echo "✓ Compilation test passed"

# Full test (compile + run)
test: $(TARGET)
	@echo "Running full test..."
	@./$(TARGET) > test_output.txt 2>&1
	@if [ -s test_output.txt ]; then \
		echo "✓ Test passed - output generated"; \
		rm test_output.txt; \
	else \
		echo "✗ Test failed - no output"; \
		exit 1; \
	fi

# Help target
help:
	@echo "Spotify DSA Recommendation System - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build release version (optimized)"
	@echo "  make release      - Build release version (optimized)"
	@echo "  make debug        - Build debug version with symbols"
	@echo "  make sanitize     - Build with address sanitizer"
	@echo "  make run          - Build and run release version"
	@echo "  make run-debug    - Build and run debug version"
	@echo "  make run-sanitize - Build and run sanitizer version"
	@echo "  make clean        - Remove all build artifacts"
	@echo "  make test         - Compile and run basic test"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make && ./recommendation_engine"
	@echo "  make debug run-debug"
	@echo "  make clean all"

# Phony targets (not actual files)
.PHONY: all release debug sanitize run run-debug run-sanitize clean test-compile test help
