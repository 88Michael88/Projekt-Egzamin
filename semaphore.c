#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/errno.h>
#include "semaphore.h"

#define IPC_RESULT_ERROR (-1)

int allocSemaphore(char* filename, int number, int flags) { key_t key;
    if ((key = ftok(filename, 2)) == -1) {
        return IPC_RESULT_ERROR;
    }

    int semID;
    if ((semID = semget(key, number, flags)) == -1) {
        printf("ERROR: %s", filename);
        perror("Blad semget (alokujSemaphore): ");
        exit(1);
    }
    return semID;
}

int destroySemaphore(int semID, int number) {
    return semctl(semID, number, IPC_RMID, NULL);
}

void initSemaphore(int semID, int number, int val) {
    if (semctl(semID, number, SETVAL, val) == -1) {
        perror("Blad semctl (inicjalizujSemaphore): ");
        exit(1);
    }
}

int waitSemaphore(int semID, int number, int flags) {
    struct sembuf operacje[1];
    operacje[0].sem_num = number;
    operacje[0].sem_op = -1;
    operacje[0].sem_flg = 0 | flags;//SEM_UNDO;

    if (semop(semID, operacje, 1) == -1) {
        perror("Blad semop (waitSemaphore)");
        return -1;
    }

    return 1;
}

int numberWaiting(int semID, int number) {
    return semctl(semID, number, GETNCNT); 
}

void signalSemaphore(int semID, int number, int option) {
    struct sembuf operacje[1];
    operacje[0].sem_num = number;
    operacje[0].sem_op = option;
    operacje[0].sem_flg = SEM_UNDO;

    if (semop(semID, operacje, 1) == -1 )
        perror("Blad semop (postSemaphore): "); 

    return;
}

int valueSemaphore(int semID, int number) {
    return semctl(semID, number, GETVAL, NULL);
}
