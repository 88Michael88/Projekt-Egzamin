# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -I.

# Source files
SRCS = dziekan.c komisja.c student.c uczen.c sharedMemory.c semaphore.c

# Header files
HEADERS = const.h semaphore.h sharedMemory.h semaphore.h

# Object files
OBJS = dziekan.o komisja.o student.o uczen.o sharedMemory.o semaphore.o

# Targets
all: dziekan komisja student uczen

# Compile dziekan
dziekan: dziekan.o sharedMemory.o semaphore.o 
	$(CC) $(CFLAGS) -o $@ dziekan.o sharedMemory.o semaphore.o

# Compile komisja
komisja: komisja.o sharedMemory.o semaphore.o
	$(CC) $(CFLAGS) -o $@ komisja.o sharedMemory.o semaphore.o

# Compile student
student: student.o sharedMemory.o semaphore.o
	$(CC) $(CFLAGS) -o $@ student.o sharedMemory.o semaphore.o

# Compile uczen
uczen: uczen.o sharedMemory.o semaphore.o
	$(CC) $(CFLAGS) -o $@ uczen.o sharedMemory.o semaphore.o

# Pattern rule for creating object files from .c files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) dziekan komisja student uczen

