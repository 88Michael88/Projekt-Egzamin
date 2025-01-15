CC = gcc
CFLAGS = -Wall -g

# Object files
OBJS = dziekan.o student.o komisja.o

# Targets
all: $(OBJS) 

dziekan.o: dziekan.c
	$(CC) $(CFLAGS) dziekan.c -o $@

student.o: student.c
	$(CC) $(CFLAGS) -c student.c -o $@

komisja.o: komisja.c
	$(CC) $(CFLAGS) -c komisja.c -o $@

# Generating assembly files
%.s: %.c $(HEADERS)
	$(CC) $(CFLAGS) -S $< -o $@

# Target to produce all assembly files
assembly: $(SRC:.c=.s)

# Clean up build artifacts
clean:
	rm -f $(OBJ)

# Phony targets
.PHONY: all clean


