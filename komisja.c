#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void handler(int sig) {
    printf("Komisja Signal Handler: %d\n", sig);
    return;
}

void setUp();

pid_t komisjaA, komisjaB;

int main() {
    signal(SIGUSR1, handler);
    
    setUp();

    wait(NULL);
    wait(NULL);

    return 0;
}


void setUp() {
    komisjaA = fork();
    switch(komisjaA) {
        case -1:
            printf("There was an error!");
            break;
        case 0:
            execlp("./komisjaT", "./komisjaT", "A", NULL);
            break;
        default:
            break;
    }

    komisjaB = fork();
    switch(komisjaB) {
        case -1:
            printf("There was an error!");
            break;
        case 0:
            execlp("./komisjaT", "./komisjaT", "B", NULL);
            break;
        default:
            break;
    }
}
