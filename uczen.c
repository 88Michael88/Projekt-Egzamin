#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include "semaphore.h"
#include "egzamStructures.h"
#include "sharedMemory.h"
#include "messageQueue.h"
#include "const.h"

int main() {
    srand(time(NULL) + getpid());

    int wydzial = 1;
    int kierunek = rand() % num_studies;
    int retakeEgzam = !((rand() % 100) % 20); // I get a range from 0 to 99, then I devide this range into 0 to 19, because one number is now 5% of the set of numbers, so if the number is 0 then the uczen is a retake student. (I could just do % 20);
                                              
    int semID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | 0666);

    waitSemaphore(semID, 0, 0);
    int* block = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);

    if (!(block[0] == kierunek)){
        detachMemoryBlock((void*)block);
        exit(0);
    } 
    printf("%d, Studiuje na wydziale %d i na kieruneku %d, poprawiam %d\n", getpid(), wydzial, kierunek, retakeEgzam);

    int semIDKom = allocSemaphore(sem_KomisjaA, 1, IPC_CREAT | 0666);
    waitSemaphore(semIDKom, 0, 0);  

    int msgID = attachMessageQueue(msg_FILENAME_A);

    struct egzamHello hello[3];
    int threads[3] = {0};

    // send hello
    for (int i = 0; i < 3; i++) {
        hello[i].messageType = i+1;
        hello[i].studentID = getpid();
        hello[i].threadID = 0;
        sendMessageQueue(msgID, (void*)&hello[i], sizeof(struct egzamHello) - sizeof(hello[i].messageType), 0);
    }

    // receive hello
    for (int i = 0; i < 3; i++) {
        receiveMessageQueue(msgID, (void*)&hello[i], sizeof(struct egzamHello) - sizeof(hello[i].messageType), getpid(), 0);
        threads[i] = hello[i].threadID;
    }
    printf("%d: %d, %d, %d.\n", getpid(), threads[0], threads[1], threads[2]);
    // receive egzam question
    for (int i = 0; i < 3; i++) {
    }
    
    // send egzam answer
    for (int i = 0; i < 3; i++) {
    }

    // receive egzam grade 
    // if failed exit.
    for (int i = 0; i < 3; i++) {
    }


    detachMemoryBlock((void*)block);
    return 0;
}
