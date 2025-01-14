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
            execlp("./student", NULL);
            break;
        default:
            break;
    }

    printf("StudentPID: %d \n", studentPID);
    sleep(10);

    wait(NULL);

    return 0;
}
