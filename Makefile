# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -I.

# Source files
SRCS = dziekan.c komisja.c student.c uczen.c komisjaT.c ./body/sharedMemory.c ./body/semaphore.c ./body/messageQueue.c ./body/list.c ./body/namedFIFO.c ./body/dziekan-list.c ./body/colorPrintf.c

# Header files
HEADERS = const.h ./headers/semaphore.h ./headers/sharedMemory.h ./headers/semaphore.h ./headers/messageQueue.h ./headers/list.h ./headers/namedFIFO.h ./headers/dziekan-list.h ./headers/colorPrintf.h

# Object files
OBJS = dziekan.o komisja.o student.o uczen.o komisjaT.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o

# Targets
all: dziekan komisja komisjaT student uczen

# Compile dziekan
dziekan: dziekan.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o
	$(CC) $(CFLAGS) -o $@ dziekan.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o
 
# Compile komisja
komisja: komisja.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o
	$(CC) $(CFLAGS) -o $@ komisja.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o
	
# Compile komisjaT
komisjaT: komisjaT.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o
	$(CC) $(CFLAGS) -o $@ komisjaT.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o

# Compile student
student: student.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o
	$(CC) $(CFLAGS) -o $@ student.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o

# Compile uczen
uczen: uczen.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o
	$(CC) $(CFLAGS) -o $@ uczen.o ./body/sharedMemory.o ./body/semaphore.o ./body/messageQueue.o ./body/list.o ./body/namedFIFO.o ./body/dziekan-list.o ./body/colorPrintf.o

# Pattern rule for creating object files from .c files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clear:
	rm -f $(OBJS) dziekan komisja student uczen komisjaT

