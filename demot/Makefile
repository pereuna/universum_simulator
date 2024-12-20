# Compiler and flags
CC = gcc
CFLAGS = -Wall -std=c99 -O2
LDFLAGS = -pg -lGL -lGLU -lglut -pthread -lm

# Source files
SRCS = gl_simulation.c
THREAD_SRCS = gl_thread_simulation.c
OBJS = $(SRCS:.c=.o)
THREAD_OBJS = $(THREAD_SRCS:.c=.o)

# Output binaries
TARGET = gl_simulation
THREAD_TARGET = gl_thread_simulation

# Default target
all: $(TARGET) $(THREAD_TARGET)

# Original simulation linking
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Multithreaded simulation linking
$(THREAD_TARGET): $(THREAD_OBJS)
	$(CC) $(THREAD_OBJS) -o $@ $(LDFLAGS)

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(THREAD_OBJS) $(TARGET) $(THREAD_TARGET)

# Phony targets
.PHONY: all clean

