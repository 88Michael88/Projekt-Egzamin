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
#include "./headers/sharedMemory.h"
#include "./headers/semaphore.h"
#include "./headers/messageQueue.h"
#include "./headers/gradeMessage.h"
#include "./headers/list.h"
#include "./headers/namedFIFO.h"
#include "./headers/dziekan-list.h"
#include "./headers/ANSI.h"
#include "./headers/colorPrintf.h"
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
    colorPrintf(GREEN, "Writing: %d \x1b[0m \n", block[0]);

    signalSemaphore(semID, 0, 160);
    
    createFIFO(fifo_PATH_A);

    int fileDesk = openFIFOForRead(fifo_PATH_A);

    struct GradeData grade;
    DziekanFinalGrade* dziekanFinalGrade = malloc(sizeof(DziekanFinalGrade));
    dziekanFinalGrade->addStudent = addStudentD;
    dziekanFinalGrade->findStudentAndGrade = findStudentAndGradeD;
    dziekanFinalGrade->calculateFinalGrades = calculateFinalGradesD;
    dziekanFinalGrade->printList = printListD;
    dziekanFinalGrade->printListFile = printListFileD;
    dziekanFinalGrade->cleanGradeList = cleanGradeListD;
    dziekanFinalGrade->statistics = statisticsD;
    dziekanFinalGrade->statisticsFile = statisticsFileD;

    while (readFIFO(fileDesk, &grade, sizeof(GradeData)) != -1) {
        dziekanFinalGrade->addStudent(dziekanFinalGrade, grade.studentID);
        dziekanFinalGrade->findStudentAndGrade(dziekanFinalGrade, grade.studentID, grade.grades, grade.finalGrade, 1);
    }

    cleanupFIFO(fifo_PATH_A);
    
    createFIFO(fifo_PATH_B);

    fileDesk = openFIFOForRead(fifo_PATH_B);

    while (readFIFO(fileDesk, &grade, sizeof(GradeData)) != -1) {
        dziekanFinalGrade->findStudentAndGrade(dziekanFinalGrade, grade.studentID, grade.grades, grade.finalGrade, 2);
    }

    dziekanFinalGrade->calculateFinalGrades(dziekanFinalGrade);

    FILE *result = fopen("result.txt", "w"); 
    dziekanFinalGrade->printListFile(dziekanFinalGrade, result);
    dziekanFinalGrade->statisticsFile(dziekanFinalGrade, result);
    fclose(result);

    int logSemID = allocSemaphore(LOG_FILE, 1, IPC_CREAT | 0666);
    waitSemaphore(logSemID, 0, 0);
    FILE* logs = fopen(LOG_FILE, "a");
    dziekanFinalGrade->statisticsFile(dziekanFinalGrade, logs);
    fclose(logs);
    signalSemaphore(logSemID, 0, 1);
    destroySemaphore(logSemID, 0);
     

    dziekanFinalGrade->cleanGradeList(dziekanFinalGrade);
    free(dziekanFinalGrade);

    cleanupFIFO(fifo_PATH_B);
    wait(NULL);
    wait(NULL);

    destroyMemoryBlock(shm_FILENAME);
    destroySemaphore(semID, 0);

    return 0;
}


void setUp() {
    studentPID = fork();
    if ((rand() % 20) == 0) {
        printf("Dziekanat-- FIRE ALARM!");
        kill(studentPID, SIGUSR1);
        kill(komisjaPID, SIGUSR1); 
        return;
    }
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

}
