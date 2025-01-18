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

int main() {
    srand(time(NULL));
    StudentGrade* head = malloc(sizeof(StudentGrade));

    // Assign function pointers
    head->findStudentAndGrade = findStudentAndGrade;
    head->addStudent = addStudent;
    head->calculateFinalGrade = calculateFinalGrade;
    head->printQueue = printQueue;
    head->cleanGradeQueue = cleanGradeQueue;

    head->addStudent(head, 1234);
    head->findStudentAndGrade(head, 1234, 0, gradeGeneration());
    head->findStudentAndGrade(head, 1234, 1, gradeGeneration());
    head->findStudentAndGrade(head, 1234, 2, gradeGeneration());
    head->calculateFinalGrade(head, 1234);
    head->addStudent(head, 9877);
    head->findStudentAndGrade(head, 9877, 0, gradeGeneration());
    head->findStudentAndGrade(head, 9877, 1, gradeGeneration());
    head->findStudentAndGrade(head, 9877, 2, gradeGeneration());
    head->calculateFinalGrade(head, 9877);
    head->addStudent(head, 1235);
    head->findStudentAndGrade(head, 1235, 0, gradeGeneration());
    head->findStudentAndGrade(head, 1235, 1, gradeGeneration());
    head->findStudentAndGrade(head, 1235, 2, gradeGeneration());
    head->calculateFinalGrade(head, 1235);
    head->addStudent(head, 9878);
    head->findStudentAndGrade(head, 9878, 0, gradeGeneration());
    head->findStudentAndGrade(head, 9878, 1, gradeGeneration());
    head->findStudentAndGrade(head, 9878, 2, gradeGeneration());
    head->calculateFinalGrade(head, 9878);
    head->addStudent(head, 1236);
    head->findStudentAndGrade(head, 1236, 0, gradeGeneration());
    head->findStudentAndGrade(head, 1236, 1, gradeGeneration());
    head->findStudentAndGrade(head, 1236, 2, gradeGeneration());
    head->calculateFinalGrade(head, 1236);
    head->addStudent(head, 9879);
    head->findStudentAndGrade(head, 9879, 0, gradeGeneration());
    head->findStudentAndGrade(head, 9879, 1, gradeGeneration());
    head->findStudentAndGrade(head, 9879, 2, gradeGeneration());
    head->calculateFinalGrade(head, 9879);
    head->addStudent(head, 1237);
    head->findStudentAndGrade(head, 1237, 0, gradeGeneration());
    head->findStudentAndGrade(head, 1237, 1, gradeGeneration());
    head->findStudentAndGrade(head, 1237, 2, gradeGeneration());
    head->calculateFinalGrade(head, 1237);
    head->addStudent(head, 9880);
    head->findStudentAndGrade(head, 9880, 0, gradeGeneration());
    head->findStudentAndGrade(head, 9880, 1, gradeGeneration());
    head->findStudentAndGrade(head, 9880, 2, gradeGeneration());
    head->calculateFinalGrade(head, 9880);
    head->addStudent(head, 1238);
    head->findStudentAndGrade(head, 1238, 0, gradeGeneration());
    head->findStudentAndGrade(head, 1238, 1, gradeGeneration());
    head->findStudentAndGrade(head, 1238, 2, gradeGeneration());
    head->calculateFinalGrade(head, 1238);
    head->addStudent(head, 9881);
    head->findStudentAndGrade(head, 9881, 0, gradeGeneration());
    head->findStudentAndGrade(head, 9881, 1, gradeGeneration());
    head->findStudentAndGrade(head, 9881, 2, gradeGeneration());
    head->calculateFinalGrade(head, 9881);
    head->addStudent(head, 1239);
    head->findStudentAndGrade(head, 1239, 0, gradeGeneration());
    head->findStudentAndGrade(head, 1239, 1, gradeGeneration());
    head->findStudentAndGrade(head, 1239, 2, gradeGeneration());
    head->calculateFinalGrade(head, 1239);

    head->printQueue(head);

    head->cleanGradeQueue(head);

    head->printQueue(head);

    free(head);
    return 0;
}
