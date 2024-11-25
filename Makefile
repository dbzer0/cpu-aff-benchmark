# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lpthread -lm

# Detect operating system
UNAME_S := $(shell uname -s)

# OS-specific settings
ifeq ($(UNAME_S),Darwin)
    CFLAGS += -D__APPLE__
endif
ifeq ($(UNAME_S),Linux)
    CFLAGS += -D_LINUX
endif
ifeq ($(UNAME_S),SunOS)
    CFLAGS += -DSOLARIS
endif
ifeq ($(UNAME_S),AIX)
    CFLAGS += -D_AIX
    CC = xlc
endif

# Build targets
TARGET = cpu-aff
SRCS = cpu-aff.c
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run targets for different thread counts
run1: $(TARGET)
	./$(TARGET) 1

run2: $(TARGET)
	./$(TARGET) 2

run4: $(TARGET)
	./$(TARGET) 4

run8: $(TARGET)
	./$(TARGET) 8

run16: $(TARGET)
	./$(TARGET) 16

# Run with custom thread count (usage: make runN THREADS=N)
runN: $(TARGET)
	./$(TARGET) $(THREADS)

# Show run options
run:
	@echo "Available run commands:"
	@echo "  make run1    - run with 1 thread"
	@echo "  make run2    - run with 2 threads"
	@echo "  make run4    - run with 4 threads"
	@echo "  make run8    - run with 8 threads"
	@echo "  make run16   - run with 16 threads"
	@echo "  make runN THREADS=N - run with N threads"

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJS)

# Show help
help:
	@echo "CPU Affinity and Memory Access Performance Benchmark"
	@echo ""
	@echo "Build targets:"
	@echo "  make        - build the project"
	@echo "  make clean  - remove build files"
	@echo ""
	@echo "Run targets:"
	@echo "  make run    - show available run options"
	@echo "  make run1   - run with 1 thread"
	@echo "  make run2   - run with 2 threads"
	@echo "  make run4   - run with 4 threads"
	@echo "  make run8   - run with 8 threads"
	@echo "  make run16  - run with 16 threads"
	@echo "  make runN THREADS=N - run with custom number of threads"
	@echo ""
	@echo "Example:"
	@echo "  make && make run8   - build and run with 8 threads"
	@echo "  make runN THREADS=12 - run with 12 threads"

# Mark targets that don't create files
.PHONY: all clean run run1 run2 run4 run8 run16 runN help
