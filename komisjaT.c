#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "const.h"
#include "error.h"
#include "./headers/semaphore.h"
#include "./headers/egzamStructures.h"
#include "./headers/messageQueue.h"
#include "./headers/list.h"
#include "./headers/ANSI.h"
#include "./headers/colorPrintf.h"
#include "./headers/namedFIFO.h"

void* osobaZkomisji(void* msgID);
float gradeGeneration();
int semID, msgID;
int codes[10];
int egzamin(int* codeForFinalGrade, char* argv, int threadID);

StudentGrade* head;
int main(int argc, char* argv[]) {
    (void)argc;
    printf("%s %s\n", argv[1], argv[2]);
    int numberOfStudents = 0;
    int parentPID = 0;
    sscanf(argv[2], "%d", &numberOfStudents);
    sscanf(argv[3], "%d", &parentPID);
    printf("Parent PID: %d\n", parentPID);

    srand((time(NULL) + (20 * *argv[1])));

    for (int i = 4; i < 13; i++) {
        codes[i-4] = i;
    }
    codes[9] = 16;

    // Create the semaphore file name from the data that is assumed to be known.
    char* semFileName = malloc(sizeof(char) * 5);
    if (semFileName == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    strcpy(semFileName, sem_Komisja);
    strcat(semFileName, argv[1]);

    // Create the message queue file name from the data that is assumed to be known.
    char* mesQFileName = malloc(sizeof(char) * 6);
    if (mesQFileName == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    strcpy(mesQFileName, msg_FILENAME);
    strcat(mesQFileName, argv[1]);

    semID = allocSemaphore(semFileName, 1, IPC_CREAT | IPC_CREAT | 0666);
    initSemaphore(semID, 0, 0);

    msgID = attachMessageQueue(mesQFileName);

    int egzamTaken = 0;
    signalSemaphore(semID, 0, 1);

    head = malloc(sizeof(StudentGrade));
    // Assign function pointers
    head->findStudentAndGrade = findStudentAndGrade;
    head->addStudent = addStudent;
    head->calculateFinalGrade = calculateFinalGrade;
    head->printQueue = printQueue;
    head->cleanGradeQueue = cleanGradeQueue;
    head->getNewlyAddedStudent = getNewlyAddedStudent;

    int threadNumber[3];
    pthread_t thread[2];

    // Create the named pipe file path name. 
    char* pipePath = malloc(sizeof(char)*45);
    if (pipePath == NULL) { // Error handling.
        perror("malloc pipePath.");
        return -1;
    }

    // Give it the corect name that depends on the Komisja.
    strcpy(pipePath, fifo_PATH);
    strcat(pipePath, argv[1]);

    /*
    // Create the needed named path for communication.
    createFIFO(pipePath);
    */
    
    int fileDesk = openFIFOForWrite(pipePath);

    while (egzamTaken < numberOfStudents) {

        for (int i = 0; i < 2; i++) {
            threadNumber[i] = i + 2;
            if (pthread_create(&thread[i], NULL, osobaZkomisji, &threadNumber[i]) != 0) {
                perror("Failed to create thread");
                return 1;
            }
        }

        int codeForFinalGrade = 0;
        int currentStudentID = egzamin(&codeForFinalGrade, argv[1], 0);

        // Wait for the thread to complete
        for (int i = 0; i < 2; i++) {
            if (pthread_join(thread[i], NULL) != 0) {
                perror("Failed to join thread");
                return 1;
            }
        }

        float finalGrade = head->calculateFinalGrade(head, currentStudentID); 
        StudentGrade newStudent = head->getNewlyAddedStudent(head);
        
        GradeData grade;
        grade.studentID = newStudent.studentID;
        memcpy(grade.grades, newStudent.grades, sizeof(newStudent.grades));
        grade.finalGrade = newStudent.finalGrade;

        // send egzamFinalGrade
        struct egzamFinalGrade finalEgzamGrade;
        finalEgzamGrade.messageType = codeForFinalGrade;
        finalEgzamGrade.finalGrade = finalGrade;
        sendMessageQueue(msgID, (void*)&finalEgzamGrade, sizeof(struct egzamFinalGrade) - sizeof(finalEgzamGrade.messageType), 0);
//        colorPrintf(MAGENTA, "Komisja%s Sent the Final Grade: %ld, %f \x1b[0m\n", argv[1], finalEgzamGrade.messageType, finalEgzamGrade.finalGrade);

        signalSemaphore(semID, 0, 1);
        egzamTaken++;

        writeFIFO(fileDesk, &grade, sizeof(GradeData));
    }
    printf("I have finished %s\n", argv[1]);

    head->cleanGradeQueue(head);

    free(head);
    free(semFileName);
    free(mesQFileName);

    deleteMessageQueue(msgID);
    destroySemaphore(semID, 0);

    return 0;
}

int egzamin(int* codeForFinalGrade, char* argv, int threadID) {
    (void)argv;
        // receive hello 
        struct egzamHello hello;  
        receiveMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), threadID + 1, 0); 
//        colorPrintf(MAGENTA, "Komisja%s Receive Hello: %ld, %d, %d, %ld \x1b[0m\n", argv, hello.messageType, hello.studentID, hello.codeForQuestion, hello.threadID);

        // send hello
        int currentStudentID = hello.studentID;
        hello.messageType = currentStudentID;
        hello.threadID = getpid();
        hello.codeForQuestion = codes[0 + threadID];
        sendMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 0);
 //       colorPrintf(MAGENTA, "Komisja%s Sent Hello: %ld, %d, %d, %ld \x1b[0m\n", argv, hello.messageType, hello.studentID, hello.codeForQuestion, hello.threadID);

        head->addStudent(head, currentStudentID);
        
        // send question
        struct egzamQuestion question;
        question.messageType = codes[0 + threadID]; 
        question.threadID = getpid();  
        question.codeForAnswer = codes[3 + threadID]; 
        question.question = (rand() % 100);
        sendMessageQueue(msgID, (void*)&question, sizeof(struct egzamQuestion) - sizeof(question.messageType), 0);
 //       colorPrintf(MAGENTA, "Komisja%s Sent a Question: %ld, %ld, %d, %d \x1b[0m\n", argv, question.messageType, question.threadID, question.codeForAnswer, question.question);

        // receive egzamAnswer
        struct egzamAnswer answer;
        receiveMessageQueue(msgID, (void*)&answer, sizeof(struct egzamAnswer) - sizeof(answer.messageType), question.codeForAnswer, 0);  
 //       colorPrintf(MAGENTA, "Komisja%s Received an Answer: %ld, %d, %d \x1b[0m\n", argv, answer.messageType, answer.codeForGrade, answer.answer);
        // send egzamGrade
        struct egzamGrade grade;
        grade.messageType = answer.codeForGrade;
        grade.threadID = getpid();
        grade.grade = gradeGeneration();
        grade.codeForFinalGrade = codes[9];
        *codeForFinalGrade = codes[9];

        head->findStudentAndGrade(head, currentStudentID, threadID, grade.grade);

        sendMessageQueue(msgID, (void*)&grade, sizeof(struct egzamGrade) - sizeof(grade.messageType), 0);
//        colorPrintf(MAGENTA, "Komisja%s Sent the Grade: %ld, %ld, %f, %d \x1b[0m\n", argv, grade.messageType, grade.threadID, grade.grade, grade.codeForFinalGrade);

        return currentStudentID;
}

void* osobaZkomisji(void* threadNumber) {
    (void)threadNumber;
    int codeForFinalGrade = 0;
    int currentStudentID = egzamin(&codeForFinalGrade, "C", (*(int *)threadNumber - 1));
    (void)codeForFinalGrade;
    (void)currentStudentID;
    return (void*)NULL;
}

float gradeGeneration() {
    int grade = (rand() % 20);
    if ((!grade) == 1) {
        return 2.0;
    } else {
        return (grade % 3) + 3;
    }
}
