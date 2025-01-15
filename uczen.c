#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main() {
    srand(time(NULL) + getpid());

    int wydzial = 1;
    int kierunek = rand() % 4 + 5;

    printf("%d, Studiuje na wydziale %d i na kieruneku %d\n", getpid(), wydzial, kierunek);
    
    return 0;
}
