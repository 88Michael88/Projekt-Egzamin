#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include "const.h"
#include "error.h"

void setupKomisje(char* type, char* numberOfStudents);

int main(int argc, char* argv[]) {
    (void)argc;
    // TODO: 
    // Create the shared memory with the Dziekanat Process.
    setupKomisje("A", argv[1]); // Pass in the shared memory block.
    setupKomisje("B", argv[1]);

    wait(NULL);
    wait(NULL);
    return 0;
}

void setupKomisje(char* type, char* numberOfStudents) {
    // Allocate the needed memory so that I can save the pid of the parent, that is Dziekan.c
    char* ppid = malloc(sizeof(char) * 10); 
    if (ppid == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    sprintf(ppid, "%d", getppid());

    switch (fork()) {
        case -1:
            perror(errors[FORK]);
            break;
        case 0:
            // Get the number of only the selected student that are going to take the Egzam.
            execlp("./komisjaT", "./komisjaT", type, numberOfStudents, ppid, NULL); 
            break;
        default:
            break;
    }

    // save the childs pid to the shared memory. 

}
