#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "list.h"
#include <stdlib.h>

float gradeGeneration() {
    int grade = (rand() % 20);
    if ((!grade) == 1) {
        return 2.0;
    } else {
        return (grade % 3) + 3;
    }
}

float normalization(float grade) {
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


float finalGrade(int* grades, int komisja) {
    if (komisja == 1) {
        if (grades[0] == 2 || grades[1] == 2 || grades[2] == 2) {
            return 2.0;
        }
    } else {
        if (grades[3] == 2 || grades[4] == 2 || grades[5] == 2) {
            return 2.0;
        }
    }

    return normalization((grades[0] + grades[1] + grades[2]) / 3);
}

int main() {
    srand(time(NULL));
    DziekanFinalGrade* head = malloc(sizeof(DziekanFinalGrade));
    int* grades = malloc(sizeof(int) * 6);

    // Assign function pointers
    head->findStudentAndGrade = findStudentAndGrade;
    head->addStudent = addStudent;
    head->calculateFinalGrade = calculateFinalGrade;
    head->printQueue = printQueue;
    head->cleanGradeQueue = cleanGradeQueue;
    head->statistics = statistics;

    head->addStudent(head, 1234);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1234, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1234, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1234);

    head->addStudent(head, 1235);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1235, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1235, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1235);

    head->addStudent(head, 1236);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1236, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1236, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1236);

    head->addStudent(head, 1237);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1237, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1237, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1237);

    head->addStudent(head, 1238);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1238, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1238, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1238);

    head->addStudent(head, 1239);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1239, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1239, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1239);

    head->addStudent(head, 1240);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1240, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1240, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1240);

    head->addStudent(head, 1241);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1241, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1241, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1241);

    head->addStudent(head, 1242);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1242, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1242, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1242);

    head->addStudent(head, 1243);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1243, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1243, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1243);

    head->addStudent(head, 1244);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1244, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1244, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1244);

    head->addStudent(head, 1245);
    for (int i = 0; i < 6; i++) {
        grades[i] = gradeGeneration();
    }
    head->findStudentAndGrade(head, 1245, grades, finalGrade(grades, 1), 1);
    head->findStudentAndGrade(head, 1245, grades, finalGrade(grades, 2), 2);
    head->calculateFinalGrade(head, 1245);


    head->printQueue(head);
    head->statistics(head);

    head->cleanGradeQueue(head);

    head->printQueue(head);

    free(grades);
    free(head);
    return 0;
}
