#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

void handler(int sig) {
    printf("Student Signal Handler\n");
    return;
}

int main() {
    
    signal(SIGUSR1, handler);
    
    for (int i = 0; i < 10; i++)
        sleep(2);

    return 0;
}
