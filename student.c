#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <pthread.h>
#include "./headers/semaphore.h"
#include "./headers/ANSI.h"
#include "./headers/colorPrintf.h"
#include "const.h"

void handler(int sig) {
    printf("Student Signal Handler: %d\n", sig);
    return;
}

int semID;
void* cleanUpStudents(void* numberOfStudents) {
    for (int i = 0; i < *(int *)numberOfStudents; i++) {
        wait(NULL);
    }
    printf("Finishing.\n");
   
    waitSemaphore(semID, 0, 0);
    FILE* logs = fopen(LOG_FILE, "a");
    fprintf(logs, "The number of students that have gone home is equal to %d\n", *(int *)numberOfStudents);
    fclose(logs);
    signalSemaphore(semID, 0, 1);

    return (void*)0;
}

int main() {
    signal(SIGUSR1, handler);

    srand(time(NULL));
    int randomNumber = (rand() % 80) + 80; // Generate a number between 80 to 160.
    pthread_t thread;

    semID = allocSemaphore(LOG_FILE, 1, IPC_CREAT | IPC_EXCL | 0666);
    initSemaphore(semID, 0, 0);

    FILE* logs = fopen(LOG_FILE, "w");
    fprintf(logs, "The number of students that came was equal to %d\n", randomNumber);
    fclose(logs);
    signalSemaphore(semID, 0, 1);

    printf("%d, %d\n", getppid(), getpid());

    if (pthread_create(&thread, NULL, cleanUpStudents, &randomNumber) != 0) {
        perror("Failed to create thread");
        return 1;
    }

    printf("%d, %d\n", getppid(), getpid());
    
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

    printf("%d, %d\n", getppid(), getpid());

    if (pthread_join(thread, NULL) != 0) {
        perror("Failed to join thread");
        return 1;
    }

    return 0;
}
