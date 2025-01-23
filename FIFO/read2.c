#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

typedef struct GradeData {
    int studentID;
    float grades[3];
    float finalGrade;
} GradeData;

#define FIFO_NAME "/home/michaelmoses/Desktop/SO/Projekt/FIFO/grade_data_fifo"

void readGradeDataFromFIFO(GradeData *data, int count) {
    // Open FIFO for reading
    int fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd < 0) {
        perror("Error opening FIFO for reading");
        exit(EXIT_FAILURE);
    }

    // Read each struct from the FIFO
    for (int i = 0; i < count; i++) {
        ssize_t bytes_read = read(fifo_fd, &data[i], sizeof(GradeData));
        if (bytes_read != sizeof(GradeData)) {
            perror("Error reading from FIFO");
            close(fifo_fd);
            exit(EXIT_FAILURE);
        }
        printf("Read GradeData for studentID %d\n", data[i].studentID);
        printf("Grades: %.2f, %.2f, %.2f | Final: %.2f\n", 
                data[i].grades[0], data[i].grades[1], data[i].grades[2], data[i].finalGrade);
    }

    close(fifo_fd);
}

int main() {
    // Ensure the FIFO exists
    if (mkfifo(FIFO_NAME, 0666) < 0 && errno != EEXIST) {
        perror("Error creating FIFO");
        exit(EXIT_FAILURE);
    }

    // Buffer to hold the 10 structs
    GradeData grades[10];

    // Read the 10 structs from the FIFO
    readGradeDataFromFIFO(grades, 10);

    return 0;
}

