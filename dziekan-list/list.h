#ifndef __list_double_link_h__
#define __list_double_link_h__


struct DziekanFinalGrade;

typedef struct dziekanGradeListDL {
    struct dziekanGradeListDL* next;
    struct dziekanGradeListDL* previous;
    int studentID;
    float grades[6];
    float finalA;
    float finalB;

    float finalGrade;

    void (*findStudentAndGrade)(struct dziekanGradeListDL*, int, int*, int, int);
    int (*addStudent)(struct dziekanGradeListDL*, int);
    int (*calculateFinalGrade)(struct dziekanGradeListDL*, int);
    void (*printQueue)(struct dziekanGradeListDL*);
    void (*cleanGradeQueue)(struct dziekanGradeListDL* head);
    void (*statistics)(struct dziekanGradeListDL* head);
} DziekanFinalGrade;

void findStudentAndGrade(DziekanFinalGrade* head, int studentID, int* grades, int finalGrade, int komisja);
int addStudent(DziekanFinalGrade* head, int studentID); 
int calculateFinalGrade(DziekanFinalGrade* head, int studentID);
void printQueue(DziekanFinalGrade* head);
void cleanGradeQueue(DziekanFinalGrade* head);
void statistics(DziekanFinalGrade* head);

#endif // __list_double_link_h__
