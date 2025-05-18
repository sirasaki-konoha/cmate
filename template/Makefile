# Compiler and flags settings
CC     := {__COMPILER__} 
CFLAGS := -Wall -Wextra -std={__CVERSION__} -O2 -Iinclude -g
LDLIBS := {__LIBRARY__}

# Project name 
PROJECT_NAME := {__PROJECT__}

# Directory settings
SRCDIR     := src
OBJDIR     := deps/object
BINDIR     := bin

# Binary output
BINARY     := $(BINDIR)/$(PROJECT_NAME)

# Auto-detect source files
SOURCES    := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c)
OBJECTS    := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SOURCES))

# Header files directory
INCLUDE_DIR := include/

# Default targets
.PHONY: all clean rebuild debug release test verbose help info

all: $(BINARY)

# Create binary executable
$(BINARY): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(LDLIBS)

# Create object files from sources
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup
clean:
	@rm -rf $(OBJDIR) $(BINDIR) deps
	@echo "Cleanup completed"

# Rebuild
rebuild: clean all

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Release build
release: CFLAGS += -O3 -DNDEBUG
release: all

# Show dependencies
deps:
	@echo "Sources:"
	@echo $(SOURCES)
	@echo ""
	@echo "Objects:"
	@echo $(OBJECTS)
	@echo ""
	@echo "Binary:"
	@echo $(BINARY)

# Test build (compile with debug info)
test: CFLAGS += -g -DDEBUG
test: all

# Verbose compilation (for debugging)
verbose: CFLAGS += -v
verbose: all

# Run the program
run: all
	./$(BINARY)

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the binary (default)"
	@echo "  clean     - Remove generated files"
	@echo "  rebuild   - Clean and then build all"
	@echo "  debug     - Debug build"
	@echo "  release   - Release build (optimized)"
	@echo "  test      - Build with debug info"
	@echo "  verbose   - Verbose compilation output"
	@echo "  run       - Build and run the program"
	@echo "  deps      - Show dependencies"
	@echo "  help      - Show this help"

# Show project structure
info:
	@echo "Project Structure:"
	@echo "  Sources:          $(SOURCES)"
	@echo "  Binary output:    $(BINARY)"
	@echo "  Project name:     $(PROJECT_NAME)"

