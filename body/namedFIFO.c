#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "./headers/namedFIFO.h"

#define IPC_RESULT_ERROR (-1)

// Create a FIFO file
int createFIFO(char* filename) {
    if (mkfifo(filename, 0777) == IPC_RESULT_ERROR && errno != EEXIST) {
        perror("There was an error making a FIFO : ");
        return IPC_RESULT_ERROR;
    }
    return 0; // Successfully created or already exists
}

// Open the FIFO for write-only access
int openFIFOForWrite(char* filename) {
    int fd = open(filename, O_WRONLY);
    if (fd == IPC_RESULT_ERROR) {
        perror("Error opening FIFO for writing");
        return IPC_RESULT_ERROR;
    }
    return fd;
}

// Open the FIFO for read-only access
int openFIFOForRead(char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == IPC_RESULT_ERROR) {
        perror("Error opening FIFO for reading");
        return IPC_RESULT_ERROR;
    }
    return fd;
}

int writeFIFO(int fileDiscriptor, void* data, int size) {
    int bytesWritten;
    if ((bytesWritten = write(fileDiscriptor, data, size)) != size) {
        perror("There was an error reading the FIFO.");
        return IPC_RESULT_ERROR;
    }
    
    return 1;

}

int readFIFO(int fileDiscriptor, void* buf, int size) {
    int bytesRead;
    if ((bytesRead = read(fileDiscriptor, buf, size)) != size){
        perror("There was an error reading the FIFO.");
        return IPC_RESULT_ERROR;
    }
    
    return 1;
}

void cleanupFIFO(char* filename) {
    if (unlink(filename) == IPC_RESULT_ERROR) {
        perror("Error removing FIFO");
    }
}

