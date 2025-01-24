#include <stdlib.h>
#include <stdio.h>
#include "list.h"

int addStudent(StudentGrade* head, int studentID) {
    if (head->studentID == 0) {
        head->studentID = studentID;
        head->grades[0] = 0;
        head->grades[1] = 0;
        head->grades[2] = 0;
        head->finalGrade = 0;
        head->previous = NULL;
    } else if (head->next == NULL) {
        StudentGrade* nextGrade = malloc(sizeof(StudentGrade));

        // Assign function pointers
        nextGrade->findStudentAndGrade = findStudentAndGrade;
        nextGrade->addStudent = addStudent;
        nextGrade->calculateFinalGrade = calculateFinalGrade;
        nextGrade->printQueue = printQueue;

        nextGrade->studentID = studentID;
        nextGrade->grades[0] = 0;
        nextGrade->grades[1] = 0;
        nextGrade->grades[2] = 0;
        nextGrade->finalGrade = 0;

        head->next = nextGrade;
        nextGrade->previous = head;
    } else {
        StudentGrade* nextGrade = malloc(sizeof(StudentGrade));
        StudentGrade* lastGrade = head->next; 
        while (lastGrade->next != NULL) {
            lastGrade = lastGrade->next;
        }

        nextGrade->findStudentAndGrade = findStudentAndGrade;
        nextGrade->addStudent = addStudent;
        nextGrade->calculateFinalGrade = calculateFinalGrade;
        nextGrade->printQueue = printQueue;

        nextGrade->studentID = studentID;
        nextGrade->grades[0] = 0;
        nextGrade->grades[1] = 0;
        nextGrade->grades[2] = 0;
        nextGrade->finalGrade = 0;

        lastGrade->next = nextGrade;
        nextGrade->previous = lastGrade;
    }
   
    return 0;
}

void findStudentAndGrade(StudentGrade* head, int studentID, int komisjaID, float grade) {
    StudentGrade* next = head;
    while (next->studentID != studentID) {
        next = next->next;
    }
    next->grades[komisjaID] = grade;
}

static float normalization(float grade) {
    int newGrade = grade * 10;
    if (newGrade <= 35) {
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

float calculateFinalGrade(StudentGrade* head, int studentID) {
    StudentGrade* next = head;
    while (next->studentID != studentID) {
        next = next->next;
    }

    if (next->grades[0] == 2 || next->grades[1] == 2 || next->grades[2] == 2) {
        next->finalGrade = 2;    
        return 2;
    }
    
    next->finalGrade = (next->grades[0] + next->grades[1] + next->grades[2]) / 3;
    next->finalGrade = normalization(next->finalGrade);
    return next->finalGrade;
}

void printQueue(StudentGrade* head) {
    StudentGrade* next = head;
    while (next != NULL) {
        printf("StudentID : %d\n", next->studentID);
        printf("Grades : %.1f %.1f %.1f\n", next->grades[0], next->grades[1], next->grades[2]);
        printf("FinalGrade : %.2f\n", next->finalGrade);
        printf("\n\n");
        next = next->next;
    }
}

void cleanGradeQueue(StudentGrade* head) {
    if (head->next == NULL) {
        return;
    }

    StudentGrade* next = head->next;
    while (next->next != NULL) {
        next = next->next;
    }

    while (next != head) {
        next = next->previous;
        free(next->next);
    }
    head->next = NULL;
}
