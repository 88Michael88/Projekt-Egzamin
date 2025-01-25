#include <stdlib.h>
#include <stdio.h>
#include "./headers/dziekan-list.h"

int addStudentD(DziekanFinalGrade* head, int studentID) {
    if (head->studentID == 0) {
        head->studentID = studentID;
        head->previous = NULL;
    } else if (head->next == NULL) {
        DziekanFinalGrade* nextGrade = malloc(sizeof(DziekanFinalGrade));

        // Assign function pointers
        nextGrade->findStudentAndGrade = findStudentAndGradeD;
        nextGrade->addStudent = addStudentD;
        nextGrade->calculateFinalGrades = calculateFinalGradesD;
        nextGrade->printList = printListD;

        nextGrade->studentID = studentID;

        head->next = nextGrade;
        nextGrade->previous = head;
    } else {
        DziekanFinalGrade* nextGrade = malloc(sizeof(DziekanFinalGrade));
        DziekanFinalGrade* lastGrade = head->next; 
        while (lastGrade->next != NULL) {
            lastGrade = lastGrade->next;
        }

        nextGrade->findStudentAndGrade = findStudentAndGradeD;
        nextGrade->addStudent = addStudentD;
        nextGrade->calculateFinalGrades = calculateFinalGradesD;
        nextGrade->printList = printListD;

        nextGrade->studentID = studentID;

        lastGrade->next = nextGrade;
        nextGrade->previous = lastGrade;
    }
   
    return 0;
}

void findStudentAndGradeD(DziekanFinalGrade* head, int studentID, float* grades, float finalGrade, int komisja) {
    DziekanFinalGrade* next = head;
    while (next->studentID != studentID) {
        next = next->next;
    }

    if (komisja == 1) { // Komisja A
        next->finalA = finalGrade;
        for (int i = 0; i < 3; i++) {
            next->grades[i] = grades[i];
        }
    } else { // Komisja B
        next->finalB = finalGrade;
        for (int i = 3; i < 6; i++) {
            next->grades[i] = grades[i];
        }
    }
}

static float normalization(float grade) {
    int newGrade = grade * 10;
    if (newGrade == 30) {
        return 3.0;
    } else if (newGrade <= 35) {
        return 3.5;
    } else if (newGrade <= 40) {
        return 4.0;
    } else if (newGrade <= 45) {
        return 4.5;
    } else if (newGrade <= 50) {
        return 5.0;
    } else {
        return -1.0; // Or another appropriate default value
    }
}

void calculateFinalGradesD(DziekanFinalGrade* head) {
    DziekanFinalGrade* next = head;
    while (next != NULL) {
        if (next->finalA == 2 || next->finalA == 0 || next->finalB == 2 || next->finalB == 0) {
            next->finalGrade = 2.0;    
            next = next->next;
            continue;
        }

        next->finalGrade = (next->finalA + next->finalB) / 2;
        next->finalGrade = normalization(next->finalGrade);
        next = next->next;
    }

}

void printListD(DziekanFinalGrade* head) {
    DziekanFinalGrade* next = head;
    while (next != NULL) {
        printf("StudentID : %d\n", next->studentID);
        printf("Komisja A : %.2f\n", next->finalA);
        printf("Komisja B : %.2f\n", next->finalB);
        printf("FinalGrade : %.2f\n", next->finalGrade);
        printf("\n\n");
        next = next->next;
    }
}

void cleanGradeListD(DziekanFinalGrade* head) {
    if (head->next == NULL) {
        return;
    }

    DziekanFinalGrade* next = head->next;
    while (next->next != NULL) {
        next = next->next;
    }

    while (next != head) {
        next = next->previous;
        free(next->next);
    }
    head->next = NULL;
}

void statisticsD(DziekanFinalGrade* head) {
    DziekanFinalGrade* next = head;
    int numberOfStudents = 0, passedStudents = 0, highestGrade = 0;
    while (next != NULL) {
        numberOfStudents++;
        int finalGrade = next->finalGrade * 10;
        if (finalGrade != 20) {
            passedStudents++; 
        }
        if (highestGrade < finalGrade) {
            highestGrade = finalGrade;
        }
        next = next->next;
    }

    float fhighestGrade = highestGrade / 10.0;
    printf("Number Of Students: %d\n", numberOfStudents);
    printf("Number Of Passed Students : %d\n", passedStudents);
    printf("Highest Grade %.1f\n", fhighestGrade);
    printf("\n\n");
}
