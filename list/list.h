#ifndef __list_double_link_h__
#define __list_double_link_h__


struct StudentGrade;

typedef struct studentGradeListDL {
    struct studentGradeListDL* next;
    struct studentGradeListDL* previous;
    int studentID;
    float grades[3];
    float finalGrade;

    void (*findStudentAndGrade)(struct studentGradeListDL*, int, int, float);
    int (*addStudent)(struct studentGradeListDL*, int);
    int (*calculateFinalGrade)(struct studentGradeListDL*, int);
    void (*printQueue)(struct studentGradeListDL*);
    void (*cleanGradeQueue)(struct studentGradeListDL* head);
} StudentGrade;

void findStudentAndGrade(StudentGrade* head, int studentID, int komisjaID, float grade);
int addStudent(StudentGrade* head, int studentID); 
int calculateFinalGrade(StudentGrade* head, int studentID);
void printQueue(StudentGrade* head);
void cleanGradeQueue(StudentGrade* head);

#endif // __list_double_link_h__
