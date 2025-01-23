#define _POSIX_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "sharedMemory.h"
#include "semaphore.h"
#include "messageQueue.h"
#include "gradeMessage.h"
#include "list.h"
#include "namedFIFO.h"
#include "const.h"


pid_t studentPID, komisjaPID;

void setUp();

int main() {
    srand(time(NULL));

    int semID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | IPC_EXCL | 0666);
    initSemaphore(semID, 0, 0);

    int* block = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);
    if (block == NULL) {
        printf("ERROR: Couldn't get block");
        return -1;
    }
    
    setUp();

    // Generate a random number and convert it into a char.
    int randomNum = rand() % num_studies;
    block[0] = randomNum;
    printf("Writing: %d\n", block[0]);

    signalSemaphore(semID, 0, 160);
    
    createFIFO(fifo_PATH_A);

    int fileDesk = openFIFOForRead(fifo_PATH_A);

    struct GradeData grade;

    while (readFIFO(fileDesk, &grade, sizeof(GradeData)) != -1) {
        printf("StudentID: %d,\n", grade.studentID);
        printf("Grades: %.1f %.1f %.1f,\n", grade.grades[0], grade.grades[1], grade.grades[2]);
        printf("Final Grade: %.2f.\n", grade.finalGrade);
    }

    cleanupFIFO(fifo_PATH_A);
    
    createFIFO(fifo_PATH_B);

    fileDesk = openFIFOForRead(fifo_PATH_B);

    while (readFIFO(fileDesk, &grade, sizeof(GradeData)) != -1) {
        printf("StudentID: %d,\n", grade.studentID);
        printf("Grades: %.1f %.1f %.1f,\n", grade.grades[0], grade.grades[1], grade.grades[2]);
        printf("Final Grade: %.2f.\n", grade.finalGrade);
    }

    cleanupFIFO(fifo_PATH_B);
    wait(NULL);
    wait(NULL);

    destroyMemoryBlock(shm_FILENAME);
    destroySemaphore(semID, 0);

    return 0;
}


void setUp() {
    studentPID = fork();
    switch(studentPID) {
        case -1:
            printf("There was an error!");
            break;
        case 0:
            // Create Student.c
            execlp("./student", "./student", NULL);
            break;
        default:
            break;
    }

    komisjaPID = fork();
    switch(komisjaPID) {
        case -1:
            printf("There was an error!");
            break;
        case 0:
            // Create Komisja.c
            execlp("./komisja", "./komisja", NULL);
            break;
        default:
            break;
    }

    if ((rand() % 20) == 0) {
        printf("Dziekanat-- FIRE ALARM!");
        kill(studentPID, SIGUSR1);
        kill(komisjaPID, SIGUSR1); 
    }
}
