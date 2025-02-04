#ifndef __d_list_double_link_h__
#define __d_list_double_link_h__


struct DziekanFinalGrade;

typedef struct dziekanGradeListDL {
    struct dziekanGradeListDL* next;
    struct dziekanGradeListDL* previous;
    int studentID;
    float grades[6];
    float finalA;
    float finalB;

    float finalGrade;

    void (*findStudentAndGrade)(struct dziekanGradeListDL*, int, float*, float, int);
    int (*addStudent)(struct dziekanGradeListDL*, int);
    void (*calculateFinalGrades)(struct dziekanGradeListDL*);
    void (*printList)(struct dziekanGradeListDL*);
    void (*printListFile)(struct dziekanGradeListDL*, FILE*);
    void (*cleanGradeList)(struct dziekanGradeListDL* head);
    void (*statistics)(struct dziekanGradeListDL* head);
    void (*statisticsFile)(struct dziekanGradeListDL* head, FILE*);
    int (*komisjaStatistics)(struct dziekanGradeListDL* head, int);
} DziekanFinalGrade;

void findStudentAndGradeD(DziekanFinalGrade* head, int studentID, float* grades, float finalGrade, int komisja);
int addStudentD(DziekanFinalGrade* head, int studentID); 
void calculateFinalGradesD(DziekanFinalGrade* head);
void printListD(DziekanFinalGrade* head);
void printListFileD(DziekanFinalGrade* head, FILE* results);
void cleanGradeListD(DziekanFinalGrade* head);
void statisticsD(DziekanFinalGrade* head);
void statisticsFileD(DziekanFinalGrade* head, FILE* results);
int komisjaStatistics(DziekanFinalGrade* head, int komisjaID);

#endif // __d_list_double_link_h__
