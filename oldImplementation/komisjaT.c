#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <time.h>
#include "./headers/list.h"
#include "./headers/egzamStructures.h"
#include "./headers/gradeMessage.h"
#include "./headers/messageQueue.h"
#include "./headers/semaphore.h"
#include "./headers/namedFIFO.h"
#include "./headers/ANSI.h"
#include "./headers/colorPrintf.h"
#include "const.h"

void* osobaZkomisji(void* msgID);
float gradeGeneration();
int semID, msgID;
int codes[10];


StudentGrade* head;
int main(int argc, char **argv) {
    srand(time(NULL));
    int enteredStudents = 0;

    char* filename = (char *)malloc(sizeof(char)*6);
    if (filename == NULL) {
        perror("malloc filename.");
        return -1;
    }
    char* semaphoreName = (char *)malloc(sizeof(char)*5);
    if (semaphoreName == NULL) {
        perror("malloc semaphoreName.");
        return -1;
    }

    strcpy(filename, msg_FILENAME);
    strcat(filename, argv[1]);
    strcpy(semaphoreName, sem_Komisja);
    strcat(semaphoreName, argv[1]);

    semID = allocSemaphore(semaphoreName, 2, IPC_CREAT | IPC_EXCL | 0666); 
    initSemaphore(semID, 0, 0);
    initSemaphore(semID, 1, 0);
    msgID = attachMessageQueue(filename);
    
    head = malloc(sizeof(StudentGrade));
    // Assign function pointers
    head->findStudentAndGrade = findStudentAndGrade;
    head->addStudent = addStudent;
    head->calculateFinalGrade = calculateFinalGrade;
    head->printQueue = printQueue;
    head->cleanGradeQueue = cleanGradeQueue;

    for (int i = 4; i < 13; i++) {
        codes[i-4] = i;
    }
    codes[9] = 16;

    pthread_t thread[2];
    int* threadNumber = malloc(sizeof(int) * 2);


    sleep(5); // try to remove this.
    waitSemaphore(semID, 1, 0);
    
    int numWaiting = 0;
    while ((numWaiting = numberWaiting(semID, 0)) != 0) {
        for (int i = 0; i < 2; i++) {
            threadNumber[i] = i + 2;
            if (pthread_create(&thread[i], NULL, osobaZkomisji, &threadNumber[i]) != 0) {
                perror("Failed to create thread");
                return 1;
            }
        }
        enteredStudents++;

        signalSemaphore(semID, 0, 1);

        // receive hello
        struct egzamHello hello;  
        int currentStudentID = 0;
        receiveMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 1, 0);

        // send hello
        currentStudentID = hello.studentID;
        hello.messageType = currentStudentID;
        hello.threadID = getpid();
        hello.codeForQuestion = codes[0];
        sendMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 0);

        head->addStudent(head, currentStudentID); 

        // send egzamQuestion
        struct egzamQuestion question;
        if ((!(rand() % 20)) == 1) {
            sleep((rand()) % 5); // question generation.
        }
        question.messageType = codes[0];
        question.threadID = getpid();  
        question.codeForAnswer = codes[3];
        question.question = (rand() % 100);
        sendMessageQueue(msgID, (void*)&question, sizeof(struct egzamQuestion) - sizeof(question.messageType), 0);

        //receive egzamAnswer
        struct egzamAnswer answer;
        receiveMessageQueue(msgID, (void*)&answer, sizeof(struct egzamAnswer) - sizeof(answer.messageType), question.codeForAnswer, 0);  

        // send egzamGrade
        struct egzamGrade grade;
        grade.messageType = answer.codeForGrade;
        grade.threadID = getpid();
        grade.grade = gradeGeneration();
        grade.codeForFinalGrade = codes[9];
        head->findStudentAndGrade(head, currentStudentID, 0, grade.grade);
        sendMessageQueue(msgID, (void*)&grade, sizeof(struct egzamGrade) - sizeof(grade.messageType), 0);

        // Wait for the thread to complete
        for (int i = 0; i < 2; i++) {
            if (pthread_join(thread[i], NULL) != 0) {
                perror("Failed to join thread");
                return 1;
            }
        }
        float finalGrade = head->calculateFinalGrade(head, currentStudentID); 

        // send egzamFinalGrade
        struct egzamFinalGrade finalEgzamGrade;
        finalEgzamGrade.messageType = grade.codeForFinalGrade;
        finalEgzamGrade.finalGrade = finalGrade;
        sendMessageQueue(msgID, (void*)&finalEgzamGrade, sizeof(struct egzamFinalGrade) - sizeof(finalEgzamGrade.messageType), 0);

        if (*argv[1] == 'B') {
            sleep(2); // Try to remove this. This could be a semaphore.
        }
    }
    colorPrintf(MAGENTA, "End %s \x1b[0m \n", argv[1]);

    int logSemID = allocSemaphore(LOG_FILE, 1, IPC_CREAT | 0666);
    waitSemaphore(logSemID, 0, 0);
    FILE* logs = fopen(LOG_FILE, "a");
    fprintf(logs, "The number of students that have come to committee %s equal to %d\n", argv[1], enteredStudents);
    fclose(logs);
    signalSemaphore(logSemID, 0, 1);

    char* pipePath = malloc(sizeof(char)*45);
    if (pipePath == NULL) {
        perror("malloc pipePath.");
        return -1;
    }

    strcpy(pipePath, fifo_PATH);
    strcat(pipePath, argv[1]);

    int fileDesk = openFIFOForWrite(pipePath);

    struct GradeData grade;
    StudentGrade* next;
    next = head;

    
    while (next != NULL) {
        grade.studentID = next->studentID;
        memcpy(grade.grades, next->grades, sizeof(float) * 3);
        grade.finalGrade = next->finalGrade;
        writeFIFO(fileDesk, &grade, sizeof(GradeData));
        next = next->next;
    }

    head->cleanGradeQueue(head);

    destroySemaphore(semID, 0);
    deleteMessageQueue(msgID);
    free(head);
    free(filename);
    free(semaphoreName);
    free(pipePath);
    return 0;
}

void* osobaZkomisji(void* threadNumber) {
    srand(time(NULL));
    int currentStudentID = 0;
    int curThreadNum = *(int *)threadNumber;
    pthread_t threadID = pthread_self() % 100000;

    // receive hello
    struct egzamHello hello;  
    receiveMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), curThreadNum, 0);

    // send hello
    currentStudentID = hello.studentID;
    hello.messageType = currentStudentID;
    hello.threadID = threadID;
    sendMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 0);

    // send egzamQuestion
    struct egzamQuestion question;
    if ((!(rand() % 5)) == 1) {
        sleep(rand() % 5); // question generation.
    }
    question.messageType = codes[curThreadNum - 1 + 3*0];
    question.threadID = threadID; 
    question.codeForAnswer = codes[curThreadNum - 1 + 3*1];
    question.question = rand() % 100; 
    sendMessageQueue(msgID, &question, sizeof(struct egzamQuestion) - sizeof(question.messageType), 0);

    //receive egzamAnswer
    struct egzamAnswer answer;
    receiveMessageQueue(msgID, (void*)&answer, sizeof(struct egzamAnswer) - sizeof(answer.messageType), question.codeForAnswer, 0);  
    //printf("Answer received: %d\n", answer.answer);

    // send egzamGrade
    struct egzamGrade grade;
    grade.messageType = answer.codeForGrade;
    grade.threadID = threadID;
    grade.codeForFinalGrade = 0;
    grade.grade = gradeGeneration();
    head->findStudentAndGrade(head, currentStudentID, curThreadNum - 1, grade.grade);
    sendMessageQueue(msgID, (void*)&grade, sizeof(struct egzamGrade) - sizeof(grade.messageType), 0);

    return (void*)0;
}

float gradeGeneration() {
    int grade = (rand() % 20);
    if ((!grade) == 1) {
        return 2.0;
    } else {
        return (grade % 3) + 3;
    }
}
