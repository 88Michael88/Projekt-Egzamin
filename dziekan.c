#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <signal.h>
#include "error.h"
#include "const.h"
#include "./headers/namedFIFO.h"
#include "./headers/ANSI.h"
#include "./headers/colorPrintf.h"
#include "./headers/semaphore.h"
#include "./headers/sharedMemory.h"
#include "./headers/list.h"
#include "./headers/dziekan-list.h"

int setupStudent(int* numberOfStudents, char** arg, int* studentPID);
int setupKomisja(int* memoryBlock, char** arg, int* komisjaPID);
void getTheFIFOData(char* FIFO_PATH, DziekanFinalGrade* dziekanFinalGrade, int komisja, int komisjaPID, int numberOfStudents, int interupt);
int semID;

void fire(int komisjaAPID, int komisjaBPID, int StudentPID, int KomisjaPID, int allStudents) {
    int* studentPIDMemoryBlock = attachMemoryBlock(shm_STUDENT_PID, allStudents);

    printf("FIRE!!!!\n");
    kill(StudentPID, SIGUSR2);
    kill(komisjaAPID, SIGUSR2);
    kill(komisjaBPID, SIGUSR2);
    kill(KomisjaPID, SIGUSR2);
    for (int i = 0; i < allStudents; i++) {
        if (studentPIDMemoryBlock[i] == 0) {
            break;
        }
        if (kill(studentPIDMemoryBlock[i], SIGUSR2) == -1) {}
    }

    destroyMemoryBlock(shm_STUDENT_PID);
    destroyMemoryBlock(shm_FILENAME);
    destroyMemoryBlock(shm_KOMISJA);

    cleanupFIFO(fifo_PATH_B);
    cleanupFIFO(fifo_PATH_A);

    destroySemaphore(semID, 0);

    exit(0);
}


int main() {
    srand(time(NULL)); 
    // Create the named FIFO, so that the KomsjaA and KomisjaB can send messages to Dziekan.
    createFIFO(fifo_PATH_A);
    createFIFO(fifo_PATH_B);
    
    // Allocate the needed array for the number of students in every faculty.
    int* numberOfStudents = malloc(sizeof(int) * SIZE_STUDENT_ARRAY);
    if (numberOfStudents == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    // Allocate the needed array to convert the numbers into a string. This will be passed to the Student.
    char** arg = malloc(sizeof(char*) * (SIZE_STUDENT_ARRAY + 1));
    if (arg == NULL) { // Erro handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    // Create and init the semaphere that will block the shared memory.
    semID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | IPC_EXCL | 0666);
    initSemaphore(semID, 0, 0);

    // Set up the student process and get its PID.
    int StudentPID = 0;
    int allStudents = setupStudent(numberOfStudents, arg, &StudentPID);
    
    // Attach to the shared memory so that the Dziekan can send and tell who has the exam.
    int* memoryBlock = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);
    if (memoryBlock == NULL) {
        printf(errors[SHARED_MEMORY]);
        return -1;
    }

    // Set up the komisja process and get its PID.
    int KomisjaPID = 0;
    int studentFacultyIndex = setupKomisja(memoryBlock, arg, &KomisjaPID);

    // Allow the Uczens to read the message. 
    signalSemaphore(semID, 0, allStudents);

    // Prepare to readthe PIDs of KomisjaA and KomisjaB.
    int komisjaAPID = 0, komisjaBPID = 0;
    int* komisjaMemoryBlock = (int*)attachMemoryBlock(shm_KOMISJA, shm_KOMISJA_SIZE);
    int semKomisjaID = allocSemaphore(sem_KOMISJA, 1, IPC_CREAT | 0666);
    waitSemaphore(semKomisjaID, 0, 0);

    // Read from the Shared Memory the PIDs of KomisjaA and KomisjaB.
    komisjaAPID = komisjaMemoryBlock[0];
    komisjaBPID = komisjaMemoryBlock[1];

    //if ((!(rand() % 20)) == 1) {
    if (0) { // Change this to the line above if you want to have the possibility of a fire. 
        sleep(10); // Wait a little, for the fire to start. :)
        fire(komisjaAPID, komisjaBPID, StudentPID, KomisjaPID, allStudents);
    }

    // Prepare to read the grades from the named FIFO.
    DziekanFinalGrade* dziekanFinalGrade = malloc(sizeof(DziekanFinalGrade));
    dziekanFinalGrade->addStudent = addStudentD;
    dziekanFinalGrade->findStudentAndGrade = findStudentAndGradeD;
    dziekanFinalGrade->calculateFinalGrades = calculateFinalGradesD;
    dziekanFinalGrade->printList = printListD;
    dziekanFinalGrade->printListFile = printListFileD;
    dziekanFinalGrade->cleanGradeList = cleanGradeListD;
    dziekanFinalGrade->statistics = statisticsD;
    dziekanFinalGrade->statisticsFile = statisticsFileD;
    dziekanFinalGrade->komisjaStatistics = komisjaStatistics;

    // Wait for the semaphore to reach the correct number of students.
    int semCountAID = allocSemaphore(sem_COUNT_KOMISJA_A, 1, IPC_CREAT | 0666);
    while (valueSemaphore(semCountAID, 0) <= (numberOfStudents[studentFacultyIndex])) {}

    // Get the data from Komisja A.
    getTheFIFOData(fifo_PATH_A, dziekanFinalGrade, 1, komisjaAPID, numberOfStudents[studentFacultyIndex], 0);

    int passed = dziekanFinalGrade->komisjaStatistics(dziekanFinalGrade, 1);
    
    // Wait for the semaphore to reach the correct number of students.
    int semCountBID = allocSemaphore(sem_COUNT_KOMISJA_B, 1, IPC_CREAT | 0666);
    while (valueSemaphore(semCountBID, 0) <= (passed)) {}

    if (valueSemaphore(semCountBID, 0) == 1 && passed == 0) {
        kill(komisjaBPID, SIGUSR1); 
    }

    // Get the data from Komisja B.
    if (numberOfStudents[studentFacultyIndex] == passed) { // If all Uczens pass KomisjaA
        getTheFIFOData(fifo_PATH_B, dziekanFinalGrade, 2, komisjaBPID, passed, 0);
    } else if (!(valueSemaphore(semCountBID, 0) == 1 && passed == 0)) { // If there is a combination of passed and failed Uczens.
        getTheFIFOData(fifo_PATH_B, dziekanFinalGrade, 2, komisjaBPID, passed, 1);
    } else { // If all Uczens don't pass KomisjaA
        cleanupFIFO(fifo_PATH_B);
    }

    dziekanFinalGrade->calculateFinalGrades(dziekanFinalGrade);
    dziekanFinalGrade->statistics(dziekanFinalGrade);
    dziekanFinalGrade->printList(dziekanFinalGrade);
    FILE* allGrades = fopen("result.txt", "w");
    dziekanFinalGrade->printListFile(dziekanFinalGrade, allGrades);
    dziekanFinalGrade->statisticsFile(dziekanFinalGrade, allGrades);
    fclose(allGrades);

    wait(NULL);
    wait(NULL);

    destroySemaphore(semID, 0);
    destroyMemoryBlock(shm_FILENAME);
    for (int i = 0; i < SIZE_STUDENT_ARRAY; i++) {
        free(arg[i]);
    }
    free(arg);
    free(numberOfStudents);
    return 0;
}

void getTheFIFOData(char* FIFO_PATH, DziekanFinalGrade* dziekanFinalGrade, int komisja, int komisjaPID, int numberOfStudents, int interupt) {
    if (interupt) {
        kill(komisjaPID, SIGUSR1); 
    }
    // Create a grade struct so that I could read the data from the FIFO.
    struct GradeData grade;

    // Open the named FIFO, for Komisja.
    int fileDesk = openFIFOForRead(FIFO_PATH);

    // Keep track of the number of reads.
    int readCount = 0;

    // Read the data from the named FIFO.
    while (readFIFO(fileDesk, &grade, sizeof(GradeData)) != -1) {
        dziekanFinalGrade->addStudent(dziekanFinalGrade, grade.studentID);
        dziekanFinalGrade->findStudentAndGrade(dziekanFinalGrade, grade.studentID, grade.grades, grade.finalGrade, komisja);
        // If we have read all the grades then leave.
        readCount++;
        if (readCount >= numberOfStudents) {
            break;
        }
    } 
    // Send the code to the Komisja porcess, so that it leaves the while loop.
    //(void)komisjaPID;
    kill(komisjaPID, SIGUSR1); 

    cleanupFIFO(FIFO_PATH);
}

int setupKomisja(int* memoryBlock, char** arg, int* komisjaPID) {
    // Write to the shared memory. 
    int studentFacultyIndex = rand() % SIZE_STUDENT_ARRAY;
    memoryBlock[0] = studentFacultyIndex;
    colorPrintf(GREEN, "Writing: %d \x1b[0m \n", memoryBlock[0]);
    
    int newPID = fork();
    switch (newPID) {
        case -1:
            perror(errors[FORK]);
            break;
        case 0:
            // Get the number of only the selected student that are going to take the Egzam.
            execlp("./komisja", "./komisja", arg[studentFacultyIndex], NULL); 
            break;
        default:
            break;
    }
    *komisjaPID = newPID;

    return studentFacultyIndex; 
}

int setupStudent(int* numberOfStudents, char** arg, int* studentPID) {
    int allStudents = 0;
    // Generate the Students number.
    for (int i = 0; i < SIZE_STUDENT_ARRAY; i++) {
        numberOfStudents[i] = (rand() % 80) + 80; // From 80 to 160.
        allStudents += numberOfStudents[i];
        arg[i] = malloc(sizeof(char) * 4); // Allocate the individual rows to store the number. 
        if (arg[i] == NULL) { // Error handling.
            perror(errors[MEMORY_ALLOCATION]);
            for (int j=0; j < i; j++) { // Free the already allocated rows. 
                free(arg[j]);
                free(numberOfStudents);
                exit(EXIT_FAILURE);
            }
        }
        sprintf(arg[i], "%d", numberOfStudents[i]); // Convert the int to a string.
    }

    int newPID = fork();
    switch (newPID) { 
        case -1: 
            printf("there was an error!");
            perror(errors[FORK]);
            break;
        case 0:
            execvp("./student", arg);
            break;
        default:
            break;
    }
    *studentPID = newPID;

    return allStudents;
}
