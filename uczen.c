#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include "const.h"
#include "./headers/semaphore.h"
#include "./headers/sharedMemory.h"

int main(int argc, char* argv[]) {
    int kierunek = argc; // This is so that I don't a warning about not using all the variables.
    int poprawia = 0;
    
    sscanf(argv[0], "%d", &kierunek);
    sscanf(argv[1], "%d", &poprawia);


    // Add the semaphore so that you could go into the shared memory.
    int semID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | 0666);

    waitSemaphore(semID, 0, 0); // Wait until the the Kierunek is chosen.
    int* block = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);

    if (!(block[0] == kierunek)){
        detachMemoryBlock((void*)block);
        exit(0);
    } 
    
    printf("%d - I am in! Kierunek %d, Poprawia %d\n", getpid(), kierunek, poprawia);
    sleep(20);

    return 0;
}
