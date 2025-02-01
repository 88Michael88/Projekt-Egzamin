#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "../headers/colorPrintf.h"
#include "../headers/ANSI.h"

// Function definition
void colorPrintf(const char *color, const char *format, ...) {
    va_list args;

    if (color != NULL) {
        printf("%s", color);
    }

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s", RESET);
    printf("%s", RESET);
}

