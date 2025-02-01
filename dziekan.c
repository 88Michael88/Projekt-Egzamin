#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "error.h"
#include "const.h"
#include "./headers/namedFIFO.h"


int main() {
    srand(time(NULL)); 
    // Allocate the needed array for the number of students in every faculty.
    int* numberOfStudents = malloc(sizeof(int) * SIZE_STUDENT_ARRAY);
    if (numberOfStudents == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    // Allocate the needed array to convert the numbers into a string. This will be passed to the Student.
    char** arg = malloc(sizeof(char*) * (SIZE_STUDENT_ARRAY + 1));
    if (arg == NULL) { // Erro handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    // Generate the Students number.
    for (int i = 0; i < SIZE_STUDENT_ARRAY; i++) {
        numberOfStudents[i] = (rand() % 80) + 80; // From 80 to 160.
        arg[i] = malloc(sizeof(char) * 4); // Allocate the individual rows to store the number. 
        if (arg[i] == NULL) { // Error handling.
            perror(errors[MEMORY_ALLOCATION]);
            for (int j=0; j < i; j++) { // Free the already allocated rows. 
                free(arg[j]);
                free(numberOfStudents);
                exit(EXIT_FAILURE);
            }
        }
        sprintf(arg[i], "%d", numberOfStudents[i]); // Convert the int to a string.
    }

    switch (fork()) { 
        case -1: 
            printf("there was an error!");
            perror(errors[FORK]);
            break;
        case 0:
            execvp("./student", arg);
            break;
        default:
            break;
    }

    wait(NULL);

    for (int i = 0; i < SIZE_STUDENT_ARRAY; i++) {
        free(arg[i]);
    }
    free(arg);
    free(numberOfStudents);
    return 0;
}
