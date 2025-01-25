#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "myPrintf.h"
#include "ANSI.h"


// Function definition
void myPrintf(const char *color, const char *format, ...) {
    va_list args;

    if (color != NULL) {
        printf("%s", color);
    }

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s", RESET);
}

