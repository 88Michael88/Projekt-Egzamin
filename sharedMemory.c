#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sharedMemory.h"

#define IPC_RESULT_ERROR (-1)

static int createMemoryBlock(char* filename, int size) {
    key_t key;
    
    if ((key = ftok(filename, 0)) == IPC_RESULT_ERROR) {
        return IPC_RESULT_ERROR;
    }

    return shmget(key, size, 0666 | IPC_CREAT);
}

char* attachMemoryBlock(char* filename, int size) {
    int sharedBlockID = createMemoryBlock(filename, size);
    char* result;

    if (sharedBlockID == IPC_RESULT_ERROR) {
        return NULL;
    }

    result = shmat(sharedBlockID, NULL, 0);
    if (result == (char*)IPC_RESULT_ERROR) {
        return NULL;
    }

    return result;
}

bool detachMemoryBlock(char* block) {
    return (shmdt(block) != IPC_RESULT_ERROR);
}

bool destroyMemoryBlock(char* filename) {
    int sharedBlockID = createMemoryBlock(filename, 0);

    if (sharedBlockID == IPC_RESULT_ERROR) {
        return NULL;
    }
    return (shmctl(sharedBlockID, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}
