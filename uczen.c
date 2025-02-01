#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    printf("%d: %d, %s, %s\n", getpid(), argc, argv[0], argv[1]);
    return 0;
}
