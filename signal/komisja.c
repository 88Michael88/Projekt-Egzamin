#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    printf("Komisja Signal Handler\n");
    return;
}






int main() {
    signal(SIGUSR1, handler);
    
    for (int i = 0; i < 10; i++)
        sleep(2);

    return 0;
}
