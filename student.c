#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "./headers/ANSI.h"
#include "./headers/colorPrintf.h"

void handler(int sig) {
    printf("Student Signal Handler: %d\n", sig);
    return;
}

void* cleanUpStudents(void* numberOfStudents) {
    for (int i = 0; i < *(int *)numberOfStudents; i++) {
        wait(NULL);
    }
    printf("Finishing.\n");
    return (void*)0;
}

int main() {
    signal(SIGUSR1, handler);

    srand(time(NULL));
    int randomNumber = (rand() % 80) + 80; // Generate a number between 80 to 160.
    pthread_t thread;

    if (pthread_create(&thread, NULL, cleanUpStudents, &randomNumber) != 0) {
        perror("Failed to create thread");
        return 1;
    }
    
    colorPrintf(BLUE, "Jest %d studentow  \x1b[0m \n", randomNumber);
    for (int i = 0; i < randomNumber; i++) {
        if ((rand() % 15) == 5) {
            sleep(rand() % 5); // generate students.
        }
        switch (fork()) {
            case -1:
                printf("There was an error forking.");
                break;
            default:
                break;
            case 0:
                execlp("./uczen", "./uczen", NULL);
        }
    }


    if (pthread_join(thread, NULL) != 0) {
        perror("Failed to join thread");
        return 1;
    }

    return 0;
}
