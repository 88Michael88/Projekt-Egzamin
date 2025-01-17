#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

void handler(int sig) {
    printf("Student Signal Handler: %d\n", sig);
    return;
}

int main() {
    signal(SIGUSR1, handler);

    srand(time(NULL));
    int randomNumber = (rand() % 80) + 80; // Generate a number between 80 to 160.
    
    printf("Jest %d studentow\n", randomNumber);
    for (int i = 0; i < randomNumber; i++) {
        if ((rand() % 15) == 5) {
            sleep(rand() % 5);
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

    for (int i = 0; i < randomNumber; i++) {
        wait(NULL);
    }

    return 0;
}
