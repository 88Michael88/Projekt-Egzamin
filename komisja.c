#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include "const.h"
#include "error.h"
#include "./headers/sharedMemory.h"
#include "./headers/semaphore.h"

void setupKomisje(char* type, char* numberOfStudents, int* memoryBlock);

int main(int argc, char* argv[]) {
    (void)argc;
    int semID = allocSemaphore(sem_KOMISJA, 1, IPC_CREAT | 0666);
    initSemaphore(semID, 0, 0);

    // Create the shared memory with the Dziekanat so that you can pass the pid of KomisjaA and KomisjaB.
    int* memoryBlock = (int*)attachMemoryBlock(shm_KOMISJA, shm_KOMISJA_SIZE);
    if (memoryBlock == NULL) {
        printf(errors[SHARED_MEMORY]);
        return -1;
    }
    setupKomisje("A", argv[1], memoryBlock);
    setupKomisje("B", argv[1], memoryBlock + 1);

    signalSemaphore(semID, 0, 1);

    printf("A pid: %d, B pid: %d \n", *memoryBlock, *(memoryBlock + 1));

    wait(NULL);
    wait(NULL);

    destroySemaphore(semID, 0);
    destroyMemoryBlock(shm_KOMISJA);
    return 0;
}

void setupKomisje(char* type, char* numberOfStudents, int* memoryBlock) {
    // Allocate the needed memory so that I can save the pid of the parent, that is Dziekan.c
    char* ppid = malloc(sizeof(char) * 10); 
    if (ppid == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    sprintf(ppid, "%d", getppid());

    // Save the pid in the variable so that it could be saved in the shared memory by the Parent.
    int newPID = fork();
    switch (newPID) {
        case -1:
            perror(errors[FORK]);
            break;
        case 0:
            // Get the number of only the selected student that are going to take the Egzam.
            execlp("./komisjaT", "./komisjaT", type, numberOfStudents, ppid, NULL); 
            break;
        default:
            // Save the new child PID.
            *memoryBlock = newPID;
            break;
    }
}
