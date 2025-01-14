CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Source and header files
SRC = dziekan.c student.c komisja.c 

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: dziekan student komisja 

# Generate dziekan file
dziekan: 
	$(CC) $(CFLAGS) dziekan.c -o dziekan 

# Generate student file
student:
	$(CC) $(CFLAGS) student.c -o student

# Generate komisja file
komisja:
	$(CC) $(CFLAGS) komisja.c -o komisja

# Generating assembly files
%.s: %.c $(HEADERS)
	$(CC) $(CFLAGS) -S $< -o $@

# Target to produce all assembly files
assembly: $(SRC:.c=.s)

# Clean up build artifacts
clean:
	rm -f $(OBJ) $(OUTPUT)

# Phony targets
.PHONY: all clean


