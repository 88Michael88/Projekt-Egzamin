#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t studentPID, komisjaPID;

    studentPID = fork();
    switch(studentPID) {
        case -1:
            printf("There was an error!");
            break;
        case 0:
            // Create Student.c
            execlp("./student", "./student", NULL);
            break;
        default:
            break;
    }

    komisjaPID = fork();
    switch(komisjaPID) {
        case -1:
            printf("There was an error!");
            break;
        case 0:
            // Create Komisja.c
            execlp("./komisja", "./komisja", NULL);
            break;
        default:
            break;
    }

    printf("StudentPID: %d \n", studentPID);
    printf("KomisjaPID: %d \n", komisjaPID);
    sleep(5);

    kill(studentPID, SIGUSR1);
    kill(komisjaPID, SIGUSR1); 

    wait(NULL);
    wait(NULL);

    return 0;
}
