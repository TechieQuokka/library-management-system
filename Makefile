# Makefile for Library Management System

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0
LDFLAGS =

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
TESTDIR = tests
EXAMPLEDIR = examples

# Target executable
TARGET = library_system
TEST_TARGET = test_runner

# Find all source files
CORE_SOURCES = $(wildcard $(SRCDIR)/core/*.c)
MODEL_SOURCES = $(wildcard $(SRCDIR)/models/*.c)
REPO_SOURCES = $(wildcard $(SRCDIR)/repositories/*.c)
SERVICE_SOURCES = $(wildcard $(SRCDIR)/services/*.c)
UI_SOURCES = $(wildcard $(SRCDIR)/ui/*.c)
COMMON_SOURCES = $(wildcard $(SRCDIR)/*.c)

# All source files except main
LIB_SOURCES = $(CORE_SOURCES) $(MODEL_SOURCES) $(REPO_SOURCES) $(SERVICE_SOURCES) $(UI_SOURCES) $(COMMON_SOURCES)

# Object files
LIB_OBJECTS = $(LIB_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Test source files
TEST_SOURCES = $(wildcard $(TESTDIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.c=$(OBJDIR)/test/%.o)

# Include paths
INCLUDES = -I$(INCDIR)

# Default target
all: $(TARGET)

# Create directories (Windows compatible)
$(OBJDIR):
ifeq ($(OS),Windows_NT)
	@if not exist obj mkdir obj
	@if not exist obj\core mkdir obj\core
	@if not exist obj\models mkdir obj\models
	@if not exist obj\repositories mkdir obj\repositories
	@if not exist obj\services mkdir obj\services
	@if not exist obj\ui mkdir obj\ui
	@if not exist obj\test mkdir obj\test
else
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/core
	mkdir -p $(OBJDIR)/models
	mkdir -p $(OBJDIR)/repositories
	mkdir -p $(OBJDIR)/services
	mkdir -p $(OBJDIR)/ui
	mkdir -p $(OBJDIR)/test
endif

# Build main executable
$(TARGET): $(OBJDIR) $(LIB_OBJECTS) $(OBJDIR)/main.o $(OBJDIR)/common.o
	$(CC) $(LIB_OBJECTS) $(OBJDIR)/common.o $(OBJDIR)/main.o -o $@ $(LDFLAGS)

# Build main.o
$(OBJDIR)/main.o: main.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build library object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build test runner
test: $(OBJDIR) $(LIB_OBJECTS) $(TEST_OBJECTS)
	$(CC) $(LIB_OBJECTS) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LDFLAGS)
	./$(TEST_TARGET)

# Build test object files
$(OBJDIR)/test/%.o: $(TESTDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Debug build
debug: CFLAGS += -DDEBUG -fsanitize=address -fno-omit-frame-pointer
debug: LDFLAGS += -fsanitize=address
debug: $(TARGET)

# Release build
release: CFLAGS = -Wall -Wextra -std=c11 -O2 -DNDEBUG
release: $(TARGET)

# Memory check with valgrind
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Static analysis
static-analysis:
	cppcheck --enable=all --std=c11 --suppress=missingIncludeSystem $(SRCDIR)

# Code formatting
format:
	find $(SRCDIR) $(INCDIR) -name "*.c" -o -name "*.h" | xargs clang-format -i

# Generate documentation
docs:
	doxygen Doxyfile

# Install (simple copy to /usr/local/bin)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Create tarball for distribution
dist: clean
	tar -czf library_system.tar.gz --exclude='.git' --exclude='*.tar.gz' .

# Print variables (for debugging Makefile)
print-%:
	@echo $* = $($*)

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the main executable (default)"
	@echo "  test         - Build and run tests"
	@echo "  debug        - Build with debug symbols and AddressSanitizer"
	@echo "  release      - Build optimized release version"
	@echo "  memcheck     - Run with Valgrind memory checker"
	@echo "  static-analysis - Run static code analysis with cppcheck"
	@echo "  format       - Format code with clang-format"
	@echo "  docs         - Generate documentation with Doxygen"
	@echo "  install      - Install to /usr/local/bin"
	@echo "  uninstall    - Remove from /usr/local/bin"
	@echo "  dist         - Create distribution tarball"
	@echo "  clean        - Remove build artifacts"
	@echo "  help         - Show this help message"

# Clean build artifacts
clean:
ifeq ($(OS),Windows_NT)
	@if exist obj rmdir /s /q obj
	@if exist $(TARGET) del /q $(TARGET)
	@if exist $(TEST_TARGET) del /q $(TEST_TARGET)
	@if exist *.o del /q *.o
else
	rm -rf $(OBJDIR)
	rm -f $(TARGET)
	rm -f $(TEST_TARGET)
	rm -f *.o
	rm -f core
	rm -f vgcore.*
endif

# Phony targets
.PHONY: all test debug release memcheck static-analysis format docs install uninstall dist clean help

# Dependencies (automatically generated)
-include $(LIB_OBJECTS:.o=.d)
-include $(TEST_OBJECTS:.o=.d)

# Generate dependency files
$(OBJDIR)/%.d: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -MM -MT $(@:.d=.o) $< > $@

$(OBJDIR)/test/%.d: $(TESTDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -MM -MT $(@:.d=.o) $< > $@