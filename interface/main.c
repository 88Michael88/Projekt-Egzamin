#include <stdio.h>
#include <unistd.h>
#include "myPrintf.h"
#include "ANSI.h"


int main() {
    myPrintf(RED, "%d - StudentID\n", getpid());
    myPrintf(GREEN, "Hello, %s!\n", "world");
    myPrintf(BLUE, "The answer is %d.\n", 42);
    return 0;
}

