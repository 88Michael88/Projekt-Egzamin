#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include "const.h"
#include "error.h"
#include "./headers/semaphore.h"
#include "./headers/egzamStructures.h"
#include "./headers/messageQueue.h"
#include "./headers/list.h"
#include "./headers/ANSI.h"
#include "./headers/colorPrintf.h"
#include "./headers/namedFIFO.h"

volatile sig_atomic_t exitSignal = 0;

void* osobaZkomisji(void* msgID);
float gradeGeneration();
int codes[10];
int egzamin(int* codeForFinalGrade, char* argv, int threadID);
int semID, semCountID, msgID;
char ARG;

// Signal handler to set the flag when a signal is received
void signalHandler(int sig) {
    if (sig == SIGUSR1 || sig == SIGUSR2) {
        exitSignal = 1;
    }

    if (sig == SIGUSR2) {
        printf("Success \n");
        deleteMessageQueue(msgID);
        destroySemaphore(semID, 0);
        destroySemaphore(semID, 1);
        destroySemaphore(semID, 2);
        destroySemaphore(semCountID, 0);
    }
}

StudentGrade* head;
int main(int argc, char* argv[]) {
    // Set up the signal for the Dziekan to terminate the Komisja when it has finished.
    if (signal(SIGUSR1, signalHandler) == SIG_ERR) {
        perror(errors[SIGNAL_HANDLER]);
    }
    // Set up the signal for the Dziekan to terminate the Komisja when there is a fire.
    if (signal(SIGUSR2, signalHandler) == SIG_ERR) {
        perror(errors[SIGNAL_HANDLER]);
    }

    (void)argc;
    printf("%s %s\n", argv[1], argv[2]);
    int numberOfStudents = 0;
    int parentPID = 0;
    sscanf(argv[2], "%d", &numberOfStudents);
    sscanf(argv[3], "%d", &parentPID);
    printf("Parent PID: %d\n", parentPID);
    ARG = *argv[1];

    srand((time(NULL) + (20 * *argv[1])));

    // Create the samaphore file name to keep track of the number of students that have came to the Komisja.
    char* semCountName = malloc(sizeof(char) * 7);
    if (semCountName == NULL) { // Error handling.
        perror(errors[MEMORY_ALLOCATION]);
    }

    strcpy(semCountName, sem_COUNT_KOMISJA); 
    strcat(semCountName, argv[1]);

    semCountID = allocSemaphore(semCountName, 1, IPC_CREAT | 0666);
    initSemaphore(semCountID, 0, 0);

    // Set up the message codes.
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

    semID = allocSemaphore(semFileName, 3, IPC_CREAT | IPC_CREAT | 0666);
    initSemaphore(semID, 0, 0);
    initSemaphore(semID, 1, 0);
    initSemaphore(semID, 2, 0); 

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

    while (egzamTaken < numberOfStudents) {
        signalSemaphore(semCountID, 0, 1);

        while (valueSemaphore(semID, 1) == 0 && exitSignal == 0) {} 
        if (exitSignal == 1) {
            break;
        }

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

        // send egzamFinalGrade
        struct egzamFinalGrade finalEgzamGrade;
        finalEgzamGrade.messageType = codeForFinalGrade;
        finalEgzamGrade.finalGrade = finalGrade;
        sendMessageQueue(msgID, (void*)&finalEgzamGrade, sizeof(struct egzamFinalGrade) - sizeof(finalEgzamGrade.messageType), 0);
        //colorPrintf(MAGENTA, "Komisja%s Sent the Final Grade: %ld, %.1f \x1b[0m\n", argv[1], finalEgzamGrade.messageType, finalEgzamGrade.finalGrade);


        signalSemaphore(semID, 2, 1);
        while (valueSemaphore(semID, 1) == 1) {}
        waitSemaphore(semID, 2, 1);
        signalSemaphore(semID, 0, 1);
        egzamTaken++;

        // printf("egzamTaken: %d, semCountID: %d\n", egzamTaken, valueSemaphore(semCountID, 0));
    }
    printf("I have finished %s\n", argv[1]);
    if (egzamTaken != 0) {
        signalSemaphore(semCountID, 0, 1);
    }

    // Create the named pipe file path name. 
    char* pipePath = malloc(sizeof(char)*45);
    if (pipePath == NULL) { // Error handling.
        perror("malloc pipePath.");
        return -1;
    }

    // Give it the corect name that depends on the Komisja.
    strcpy(pipePath, fifo_PATH);
    strcat(pipePath, argv[1]);
    int fileDesk = 0;
    if (egzamTaken != 0) {
        fileDesk = openFIFOForWrite(pipePath);
    }

    deleteMessageQueue(msgID);
    destroySemaphore(semID, 0);
    destroySemaphore(semID, 1);
    destroySemaphore(semID, 2);
    destroySemaphore(semCountID, 0);

    if (egzamTaken != 0) {
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
    }

    head->cleanGradeQueue(head);

    free(head);
    free(semFileName);
    free(mesQFileName);
    free(semCountName);

    return 0;
}

int egzamin(int* codeForFinalGrade, char* argv, int threadID) {
    (void)argv;
        // receive hello 
        struct egzamHello hello;  
        receiveMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), threadID + 1, 0); 
        colorPrintf(MAGENTA, "Komisja%s Receive Hello: %ld, %d, %d, %ld \x1b[0m\n", argv, hello.messageType, hello.studentID, hello.codeForQuestion, hello.threadID);

        // Check if the Uczen is redoing the Egzam.
        int currentStudentID = hello.studentID;
        if (hello.poprawia != 0) {
            head->addStudent(head, currentStudentID);
            head->findStudentAndGrade(head, currentStudentID, threadID, hello.poprawia);
            *codeForFinalGrade = codes[9];
            return currentStudentID;
        }

        // send hello
        hello.messageType = currentStudentID;
        hello.threadID = getpid();
        hello.codeForQuestion = codes[0 + threadID];
        sendMessageQueue(msgID, (void*)&hello, sizeof(struct egzamHello) - sizeof(hello.messageType), 0);
        colorPrintf(MAGENTA, "Komisja%s Sent Hello: %ld, %d, %d, %ld \x1b[0m\n", argv, hello.messageType, hello.studentID, hello.codeForQuestion, hello.threadID);

        head->addStudent(head, currentStudentID);
        
        // send question
        struct egzamQuestion question;
        question.messageType = codes[0 + threadID]; 
        question.threadID = getpid();  
        question.codeForAnswer = codes[3 + threadID]; 
        question.question = (rand() % 100);
        sendMessageQueue(msgID, (void*)&question, sizeof(struct egzamQuestion) - sizeof(question.messageType), 0);
        colorPrintf(MAGENTA, "Komisja%s Sent a Question: %ld, %ld, %d, %d \x1b[0m\n", argv, question.messageType, question.threadID, question.codeForAnswer, question.question);

        // receive egzamAnswer
        struct egzamAnswer answer;
        receiveMessageQueue(msgID, (void*)&answer, sizeof(struct egzamAnswer) - sizeof(answer.messageType), question.codeForAnswer, 0);  
        colorPrintf(MAGENTA, "Komisja%s Received an Answer: %ld, %d, %d \x1b[0m\n", argv, answer.messageType, answer.codeForGrade, answer.answer);

        // send egzamGrade
        struct egzamGrade grade;
        grade.messageType = answer.codeForGrade;
        grade.threadID = getpid();
        grade.grade = gradeGeneration();
        grade.codeForFinalGrade = codes[9];
        *codeForFinalGrade = codes[9];

        head->findStudentAndGrade(head, currentStudentID, threadID, grade.grade);

        sendMessageQueue(msgID, (void*)&grade, sizeof(struct egzamGrade) - sizeof(grade.messageType), 0);
        colorPrintf(MAGENTA, "Komisja%s Sent the Grade: %ld, %ld, %.1f, %d \x1b[0m\n", argv, grade.messageType, grade.threadID, grade.grade, grade.codeForFinalGrade);

        return currentStudentID;
}

void* osobaZkomisji(void* threadNumber) {
    (void)threadNumber;
    int codeForFinalGrade = 0;
    int currentStudentID = egzamin(&codeForFinalGrade, &ARG, (*(int *)threadNumber - 1));
    (void)codeForFinalGrade;
    (void)currentStudentID;
    return (void*)NULL;
}

float gradeGeneration() {
//    return 2.0;
    int grade = (rand() % 20);
    if ((!grade) == 1) {
        return 2.0;
    } else {
        return (grade % 3) + 3;
    }
}
