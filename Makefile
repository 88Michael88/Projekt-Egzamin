# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -I.

# Source files
SRCS = dziekan.c komisja.c student.c uczen.c komisjaT.c sharedMemory.c semaphore.c messageQueue.c list.c namedFIFO.c dziekan-list.c

# Header files
HEADERS = const.h semaphore.h sharedMemory.h semaphore.h messageQueue.h list.h namedFIFO.h dziekan-list.h

# Object files
OBJS = dziekan.o komisja.o student.o uczen.o komisjaT.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o

# Targets
all: dziekan komisja komisjaT student uczen

# Compile dziekan
dziekan: dziekan.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o
	$(CC) $(CFLAGS) -o $@ dziekan.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o
 
# Compile komisja
komisja: komisja.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o
	$(CC) $(CFLAGS) -o $@ komisja.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o
	
# Compile komisjaT
komisjaT: komisjaT.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o
	$(CC) $(CFLAGS) -o $@ komisjaT.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o

# Compile student
student: student.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o
	$(CC) $(CFLAGS) -o $@ student.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o

# Compile uczen
uczen: uczen.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o
	$(CC) $(CFLAGS) -o $@ uczen.o sharedMemory.o semaphore.o messageQueue.o list.o namedFIFO.o dziekan-list.o

# Pattern rule for creating object files from .c files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clear:
	rm -f $(OBJS) dziekan komisja student uczen komisjaT

