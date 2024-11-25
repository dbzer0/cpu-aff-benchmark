CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread -lm

# Определение операционной системы
UNAME_S := $(shell uname -s)

# Специфичные флаги для разных ОС
ifeq ($(UNAME_S),Darwin)
    CFLAGS += -D__APPLE__
endif
ifeq ($(UNAME_S),SunOS)
    CFLAGS += -DSOLARIS
endif
ifeq ($(UNAME_S),AIX)
    CFLAGS += -D_AIX
    CC = xlc
endif

# Цели сборки
TARGET = cpu-aff
SRCS = cpu-aff.c
OBJS = $(SRCS:.c=.o)

# Правила сборки
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(TARGET) $(OBJS)

# Запуск с разным количеством потоков
run1: $(TARGET)
	./$(TARGET) 1

run2: $(TARGET)
	./$(TARGET) 2

run4: $(TARGET)
	./$(TARGET) 4

run8: $(TARGET)
	./$(TARGET) 8

run: $(TARGET)
	@echo "Usage examples:"
	@echo "  make run1  - run with 1 thread"
	@echo "  make run2  - run with 2 threads"
	@echo "  make run4  - run with 4 threads"
	@echo "  make run8  - run with 8 threads"

# Помощь
help:
	@echo "Available targets:"
	@echo "  make       - build the project"
	@echo "  make clean - remove build files"
	@echo "  make run   - show run options"
	@echo "  make run1  - run with 1 thread"
	@echo "  make run2  - run with 2 threads"
	@echo "  make run4  - run with 4 threads"
	@echo "  make run8  - run with 8 threads"

.PHONY: all clean run run1 run2 run4 run8 help
