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

    printf("StudentPID: %d \n", studentPID);
    printf("KomisjaPID: %d \n", komisjaPID);

    if ((rand() % 20) == 0) {
        printf("Dziekanat-- FIRE ALARM!");
        kill(studentPID, SIGUSR1);
        kill(komisjaPID, SIGUSR1); 
    }
}
