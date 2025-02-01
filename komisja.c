#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "const.h"
#include "error.h"

void setupKomisje(char* type, char* numberOfStudents);

int main(int argc, char* argv[]) {
    setupKomisje("A", argv[1]);
    setupKomisje("B", argv[1]);

    wait(NULL);
    wait(NULL);
    return 0;
}

void setupKomisje(char* type, char* numberOfStudents) {
    switch (fork()) {
        case -1:
            perror(errors[FORK]);
            break;
        case 0:
            // Get the number of only the selected student that are going to take the Egzam.
            execlp("./komisjaT", "./komisjaT", type, numberOfStudents, NULL); 
            break;
        default:
            break;
    }

}
