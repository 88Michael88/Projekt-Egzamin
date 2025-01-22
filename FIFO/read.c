#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

typedef struct GradeData {
    int studentID;
    float grades[3];
    float finalGrade;
} GradeData;

#define FIFO_NAME "/home/michaelmoses/Desktop/SO/Projekt/FIFO/grade_data_fifo"

void readGradeDataFromFIFO(GradeData *data) {
    // Open FIFO for reading
    int fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd < 0) {
        perror("Error opening FIFO for reading");
        exit(EXIT_FAILURE);
    }

    // Read the struct from the FIFO
    ssize_t bytes_read = read(fifo_fd, data, sizeof(GradeData));
    if (bytes_read < 0) {
        perror("Error reading from FIFO");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    } else if (bytes_read == 0) {
        printf("No data to read (FIFO closed by writer).\n");
    }

    close(fifo_fd);
}

int main() {
    // Ensure the FIFO exists
    if (mkfifo(FIFO_NAME, 0666) < 0 && errno != EEXIST) {
        perror("Error creating FIFO");
        exit(EXIT_FAILURE);
    }
    printf("1\n");

    // Buffer to store the struct
    GradeData data;
    printf("2\n");

    // Read the struct from the FIFO
    for(int i = 0; i < 10; i++) {

        readGradeDataFromFIFO(&data);
        printf("3\n");

        // Print the received data
        printf("Received GradeData:\n");
        printf("Student ID: %d\n", data.studentID);
        printf("Grades: %.2f, %.2f, %.2f\n", data.grades[0], data.grades[1], data.grades[2]);
        printf("Final Grade: %.2f\n", data.finalGrade);
    }

    return 0;
}

