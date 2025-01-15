#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include "sharedMemory.h"
#include "semaphore.h"
#include "const.h"

void setUp();

int main() {
    srand(time(NULL));

    int semID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | IPC_EXCL | 0666);
    printf("%d", semID);
    initSemaphore(semID, 0, 0);

    char* block = attachMemoryBlock(shm_FILENAME, shm_SIZE);
    if (block == NULL) {
        printf("ERROR: Couldn't get block");
        return -1;
    }
    
    setUp();

    // Generate a random number and convert it into a char.
    int randomNum = rand() % 4 + 5;
    char charNum[2]; 
    snprintf(charNum, sizeof(charNum), "%d", randomNum);
    printf("Writing: %s\n", charNum);
    strncpy(block, charNum, shm_SIZE);

    signalSemaphore(semID, 1);
    wait(NULL);
    wait(NULL);

    destroyMemoryBlock(shm_FILENAME);

    return 0;
}


void setUp() {
    pid_t studentPID, komisjaPID;

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

    printf("StudentPID: %d \n", studentPID);
    printf("KomisjaPID: %d \n", komisjaPID);
}
