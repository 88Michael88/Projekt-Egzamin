#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <time.h>
#include "list.h"
#include "egzamStructures.h"
#include "gradeMessage.h"
#include "messageQueue.h"
#include "semaphore.h"
#include "const.h"

void* osobaZkomisji(void* msgID);
int semID, msgID;
int codes[9];


StudentGrade* head;
int main(int argc, char **argv) {
    srand(time(NULL));
    printf("%d, %s\n", argc, argv[1]);
    
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

    semID = allocSemaphore(semaphoreName, 1, IPC_CREAT | IPC_EXCL | 0666); 
    initSemaphore(semID, 0, 0);
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

    pthread_t thread[2];
    int* threadNumber = malloc(sizeof(int) * 2);
   
    for (int i = 0; i < 2; i++) {
        threadNumber[i] = i + 2;
        if (pthread_create(&thread[i], NULL, osobaZkomisji, &threadNumber[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    signalSemaphore(semID, 0, 160);

    // receive hello
    struct egzamHello hello;  
    int currnetStudentID = 0;
    receiveMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 1, 0);
    head->addStudent(head, hello.studentID);

    // send hello
    currnetStudentID = hello.studentID;
    hello.messageType = currnetStudentID;
    hello.threadID = getpid();
    hello.codeForQuestion = codes[0];
    sendMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 0);

    // send egzamQuestion
    struct egzamQuestion question;
    question.messageType = 4;
    question.threadID = getpid();  
    if ((!(rand() % 20)) == 1) {
        sleep((rand()) % 10);
    }
    question.question = (rand() % 100);
    sendMessageQueue(msgID, (void*)&question, sizeof(struct egzamQuestion) - sizeof(question.messageType), 0);
    printf("question sent\n");

    // Wait for the thread to complete
    for (int i = 0; i < 2; i++) {
        if (pthread_join(thread[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    sleep(20);
    destroySemaphore(semID, 0);
    deleteMessageQueue(msgID);
    free(head);
    return 0;
}

void* osobaZkomisji(void* threadNumber) {
    srand(time(NULL));
    int currnetStudentID = 0;
    int curThreadNum = *(int *)threadNumber;
    pthread_t threadID = pthread_self() % 100000;

    struct egzamHello hello;  
    // receive hello
    receiveMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), curThreadNum, 0);
    printf("ThreadID %p, %lu, %d. StudentID %d\n", head, threadID, curThreadNum, hello.studentID);

    // send hello
    struct egzamHello ack;
    currnetStudentID = hello.studentID;
    ack.messageType = currnetStudentID;
    ack.threadID = threadID;
    sendMessageQueue(msgID, (void*)&ack, sizeof(struct egzamHello) - sizeof(ack.messageType), 0);

    // send egzamQuestion
    struct egzamQuestion question;
    question.messageType = codes[curThreadNum - 1 + 3];
    question.threadID = threadID; 
    if ((!(rand() % 20)) == 1) {
        sleep(rand() % 10);
    }
    question.question = rand() % 100; 
    sendMessageQueue(msgID, &question, sizeof(struct egzamQuestion) - sizeof(question.messageType), 0);


    return (void*)0;
}
