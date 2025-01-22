#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "namedFIFO.h"

#define IPC_RESULT_ERROR (-1)

int namedFIFO(char* filename) {
    int fileDiscriptor;
    if ((fileDiscriptor = mkfifo(filename, 0777)) != IPC_RESULT_ERROR) {
        perror("There was an error making a FIFO : ");
        return IPC_RESULT_ERROR;
    }

    return fileDiscriptor;
}

int writeFIFO(int fileDiscriptor, void* data, int size) {

}

int readFIFO(int fileDiscriptor, void* buf, int size) {
    
}
