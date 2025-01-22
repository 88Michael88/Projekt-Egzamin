#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

typedef struct GradeData {
    int studentID;
    float grades[3];
    float finalGrade;
} GradeData;

#define FIFO_NAME "/home/michaelmoses/Desktop/SO/Projekt/FIFO/grade_data_fifo"

void writeGradeDataToFIFO(const GradeData *data) {
    // Open FIFO for writing
    printf("4\n");
    int fifo_fd = open(FIFO_NAME, O_WRONLY);
    printf("5\n");
    if (fifo_fd < 0) {
        perror("Error opening FIFO for writing");
        exit(EXIT_FAILURE);
    }
    printf("5\n");

    // Write the struct to the FIFO
    ssize_t bytes_written = write(fifo_fd, data, sizeof(GradeData));
    if (bytes_written < 0) {
        perror("Error writing to FIFO");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }
    printf("6\n");

    close(fifo_fd);
    printf("7\n");
}

int main() {
    // Create the FIFO if it doesn't already exist
    if (mkfifo(FIFO_NAME, 0666) < 0 && errno != EEXIST) {
        perror("Error creating FIFO");
        exit(EXIT_FAILURE);
    }
    printf("1\n");

    // Example GradeData struct
    GradeData data = {12345, {90.5, 85.0, 88.0}, 87.8};
    printf("2\n");

    // Write the struct to the FIFO
    for(int i = 0; i < 10; i++) {
        sleep(2);
    writeGradeDataToFIFO(&data);
    printf("3\n");
    }


    return 0;
}

