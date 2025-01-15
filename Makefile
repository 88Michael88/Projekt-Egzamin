CC = gcc
CFLAGS = -Wall -g

# Object files
OBJS = dziekan.o student.o komisja.o uczen.o

# Targets
all: dziekan_executable student_executable komisja_executable uczen_executable

dziekan_executable: dziekan.o 
	$(CC) dziekan.o -o $@

student_executable: student.o
	$(CC) student.o -o $@

komisja_executable: komisja.o
	$(CC) komisja.o -o $@

uczen_executable: uczen.o
	$(CC) uczen.o -o $@

dziekan.o: dziekan.c
	$(CC) $(CFLAGS) -c dziekan.c -o $@

student.o: student.c
	$(CC) $(CFLAGS) -c student.c -o $@

komisja.o: komisja.c
	$(CC) $(CFLAGS) -c komisja.c -o $@

uczen.o: uczen.c
	$(CC) $(CFLAGS) -c uczen.c -o $@

# Generating assembly files
%.s: %.c $(HEADERS)
	$(CC) $(CFLAGS) -S $< -o $@

# Target to produce all assembly files
assembly: $(SRC:.c=.s)

# Clean up build artifacts
clean:
	rm -f dziekan.o student.o komisja.o uczen.o ./dziekan_executable ./student_executable ./komisja_executable ./uczen_executable


# Phony targets
.PHONY: all clean


