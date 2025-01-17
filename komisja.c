#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig) {
    printf("Komisja Signal Handler: %d\n", sig);
    return;
}

int main() {
    signal(SIGUSR1, handler);
    
    


    return 0;
}
