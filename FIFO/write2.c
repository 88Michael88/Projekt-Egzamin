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

void writeGradeDataToFIFO(const GradeData *data, int count) {
    // Open FIFO for writing
    int fifo_fd = open(FIFO_NAME, O_WRONLY);
    if (fifo_fd < 0) {
        perror("Error opening FIFO for writing");
        exit(EXIT_FAILURE);
    }

    // Write each struct to the FIFO
    for (int i = 0; i < count; i++) {
        sleep(2);
        ssize_t bytes_written = write(fifo_fd, &data[i], sizeof(GradeData));
        if (bytes_written != sizeof(GradeData)) {
            perror("Error writing to FIFO");
            close(fifo_fd);
            exit(EXIT_FAILURE);
        }
        printf("Wrote GradeData for studentID %d\n", data[i].studentID);
    }

    close(fifo_fd);
}

int main() {
    // Create the FIFO if it doesn't exist
    if (mkfifo(FIFO_NAME, 0666) < 0 && errno != EEXIST) {
        perror("Error creating FIFO");
        exit(EXIT_FAILURE);
    }

    // Create 10 GradeData structs
    GradeData grades[10];
    for (int i = 0; i < 10; i++) {
        grades[i].studentID = i + 1;
        grades[i].grades[0] = 85.0 + i;
        grades[i].grades[1] = 90.0 + i;
        grades[i].grades[2] = 88.0 + i;
        grades[i].finalGrade = (grades[i].grades[0] + grades[i].grades[1] + grades[i].grades[2]) / 3;
    }

    // Write the 10 structs to the FIFO
    writeGradeDataToFIFO(grades, 10);

    return 0;
}

