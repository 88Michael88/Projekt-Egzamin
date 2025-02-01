#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include "error.h"
#include "const.h"
#include "./headers/namedFIFO.h"
#include "./headers/ANSI.h"
#include "./headers/colorPrintf.h"
#include "./headers/semaphore.h"
#include "./headers/sharedMemory.h"

int setupStudent(int* numberOfStudents, char** arg);

int main() {
    srand(time(NULL)); 
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
    int semID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | IPC_EXCL | 0666);
    initSemaphore(semID, 0, 0);

    // Set up the student process.
    int allStudents = setupStudent(numberOfStudents, arg);
    
    // Attach to the shared memory so that the Dziekan can send and tell who has the exam.
    int* memoryBlock = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);
    if (memoryBlock == NULL) {
        printf(errors[SHARED_MEMORY]);
        return -1;
    }

    // Write to the shared memory. 
    int randomNum = rand() % SIZE_STUDENT_ARRAY;
    memoryBlock[0] = randomNum;
    colorPrintf(GREEN, "Writing: %d \x1b[0m \n", memoryBlock[0]);
    
    // Allow the Uczens to read the message. 
    signalSemaphore(semID, 0, allStudents);

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

int setupStudent(int* numberOfStudents, char** arg) {
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

    switch (fork()) { 
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

    return allStudents;
}
