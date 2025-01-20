#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <pthread.h>
#include "list.h"
#include "egzamStructures.h"
#include "gradeMessage.h"
#include "messageQueue.h"
#include "semaphore.h"
#include "const.h"

void* osobaZkomisji(void* msgID);

StudentGrade* head;
int main(int argc, char **argv) {
    printf("%d, %s\n", argc, argv[1]);
    
    char* filename = (char *)malloc(6);
    if (filename == NULL) {
        perror("malloc filename.");
        return -1;
    }
    char* semaphoreName = (char *)malloc(5);
    if (semaphoreName == NULL) {
        perror("malloc semaphoreName.");
        return -1;
    }

    strcpy(filename, msg_FILENAME);
    strcat(filename, argv[1]);
    printf("FILENAME %s\n", filename);
    strcpy(semaphoreName, sem_Komisja);
    strcat(semaphoreName, argv[1]);

    int semID = allocSemaphore(semaphoreName, 1, IPC_CREAT | IPC_EXCL | 0666); 
    initSemaphore(semID, 0, 0);
    int msgID = attachMessageQueue(filename);
    
    head = malloc(sizeof(StudentGrade));
    // Assign function pointers
    head->findStudentAndGrade = findStudentAndGrade;
    head->addStudent = addStudent;
    head->calculateFinalGrade = calculateFinalGrade;
    head->printQueue = printQueue;
    head->cleanGradeQueue = cleanGradeQueue;

    pthread_t thread[2];
   
    for (int i = 0; i < 2; i++) {
        if (pthread_create(&thread[i], NULL, osobaZkomisji, semaphoreName) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    signalSemaphore(semID, 0, 160);

    struct egzamHello hello;  
    // receive hello
    receiveMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 1, 0);
    head->addStudent(head, hello.studentID);

    // send hello
    hello.messageType = hello.studentID;
    hello.threadID = getpid();
    sendMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 0);
    sendMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 0);
    sendMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 0);



    // Wait for the thread to complete
    for (int i = 0; i < 2; i++) {
        if (pthread_join(thread[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }
    //struct gradeMessage testMessage;
    
    //testMessage.messageType = 2;
    //testMessage.komisja = *argv[1];
    //testMessage.studentID = 1111;
    //testMessage.grade = 4.88;

    //sendMessageQueue(msgID, (void*)&testMessage, sizeof(testMessage) - sizeof(testMessage.messageType), 0);
    sleep(20);
    destroySemaphore(semID, 0);
    deleteMessageQueue(msgID);
    free(head);
    return 0;
}

void* osobaZkomisji(void* semaphoreName) {
    pthread_t threadID = pthread_self();

    printf("ThreadID %p, %lu, %s\n", head, threadID, (char*)semaphoreName);
    // How can I access head variable here?

    return (void*)0;
}
