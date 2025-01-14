#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t komisjaPID;

    komisjaPID = fork();
    switch(komisjaPID) {
        case -1:
            printf("There was an error!");
            break;
        case 0:
            // Create Komisja.c
            execlp("./komisja", NULL);
            break;
        default:
            break;
    }
    
    printf("KomisjaPID: %d \n", komisjaPID);
    sleep(10);

    wait(NULL);
    
    return 0;
}
