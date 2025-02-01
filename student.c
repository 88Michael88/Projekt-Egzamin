#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include "error.h"
#include "const.h"

int generateIndex(int numberOfIndexes, int* array);

int main(int argc, char* argv[]) {
    int numberOfAllStudents = 0;
    // Allocate the needed array for the number of students in every faculty.
    int* numberOfStudents = malloc(sizeof(int) * SIZE_STUDENT_ARRAY);
    if (numberOfStudents == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    // Convert the string into a number.
    for (int i = 0; i < argc; i++) {
        sscanf(argv[i], "%d", &numberOfStudents[i]);
        numberOfAllStudents += numberOfStudents[i];
    }
    
    int kierunek, poprawia;
    // Allocate the needed array for the uczen data.
    char** uczenData = malloc(sizeof(char*) * 2);
    if (uczenData == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    for (int i = 0; i < 2; i++) { // Allocate the rows.
        if ((uczenData[i] = malloc(sizeof(char) * 3)) == NULL) { // Error handling.
            perror(errors[MEMORY_ALLOCATION]);
            for (int j = 0; j < i; j++) {
                free(uczenData[j]);
            }
            free(uczenData);
        }
    }

    for (int i = 0; i < numberOfAllStudents; i++) { // Generate the needed Uczens.
        kierunek = generateIndex(SIZE_STUDENT_ARRAY, numberOfStudents); // Pick from what faculty will the student be from.
        numberOfStudents[kierunek]--;
        poprawia = (!(rand() % 20)); // Randomly check if he is redoing the egzam.
        if (poprawia == 1) { // If he is what grade did he have?
            poprawia = (rand() % 4) + 1;
        }
        sprintf(uczenData[0], "%d", kierunek);
        sprintf(uczenData[1], "%d", poprawia);

        switch (fork()) { 
            case -1: 
                printf("there was an error!");
                perror(errors[FORK]);
                break;
            case 0:
                execvp("./uczen", uczenData);
                break;
            default:
                break;
        }
    }

    for (int i = 0; i < numberOfAllStudents; i++) {
        wait(NULL);
    }

    return 0;
}

// This function will give an index of a non zero field. 
// If there are only zero filed it will return -1;
int generateIndex(int numberOfIndexes, int* array) {
    int sum = 0;
    for (int i = 0; i < numberOfIndexes; i++) {
        sum += array[i];
    }
    if (sum <= 0) {
        return -1;
    }

    int value = 0;
    int index = -1;
    do {
        index = rand() % numberOfIndexes; 
        value = array[index];
    } while (value == 0);

    return index;
}


