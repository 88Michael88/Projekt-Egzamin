#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include "semaphore.h"
#include "sharedMemory.h"
#include "const.h"

int main() {
    srand(time(NULL) + getpid());

    int wydzial = 1;
    int kierunek = rand() % num_studies;
    int retakeEgzam = !((rand() % 100) % 20); // I get a range from 0 to 99, then I devide this range into 0 to 19, because one number is now 5% of the set of numbers, so if the number is 0 then the uczen is a retake student. (I could just do % 20);
                                              
    int semID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | 0666);

    waitSemaphore(semID, 0, 0);
    int* block = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);

    if (block[0] == kierunek){
        printf("%d, Studiuje na wydziale %d i na kieruneku %d, poprawiam %d\n", getpid(), wydzial, kierunek, retakeEgzam);
        sleep(5);
    } 

    detachMemoryBlock((void*)block);
    
    return 0;
}
