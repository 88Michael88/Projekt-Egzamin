#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <signal.h>
#include "const.h"
#include "./headers/semaphore.h"
#include "./headers/sharedMemory.h"
#include "./headers/egzamStructures.h"
#include "./headers/messageQueue.h"
#include "./headers/colorPrintf.h"
#include "./headers/ANSI.h"

volatile sig_atomic_t exitSignal = 0;
int inEgzam = 0;
int egzamin(int msgID, float poprawia);
int msgID_A, msgID_B;
int semSharedMemoryID;

// Signal handler to set the flag when a signal is received
void signalHandler(int sig) {
    if (sig == SIGUSR2) {
        exitSignal = 1;
        deleteMessageQueue(msgID_A);
        deleteMessageQueue(msgID_B);
        destroySemaphore(semSharedMemoryID, 0);

        exit(0);
    }
}

int main(int argc, char* argv[]) {
    int kierunek = argc; // This is so that I don't a warning about not using all the variables.
    int poprawia = 0;
    
    sscanf(argv[0], "%d", &kierunek);
    sscanf(argv[1], "%d", &poprawia);

    // Add the semaphore so that you could go into the shared memory.
    semSharedMemoryID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | 0666);

    waitSemaphore(semSharedMemoryID, 0, 0); // Wait until the the Kierunek is chosen.
    int* block = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);

    if (!(block[0] == kierunek)){
        detachMemoryBlock((void*)block);
        exit(0);
    } 
    
    printf("%d - I am in! Kierunek %d, Poprawia %d\n", getpid(), kierunek, poprawia);

    int semKomisjaAID = allocSemaphore(sem_KomisjaA, 3, IPC_CREAT | 0666);

    printf("%d - I am in the waiting room for komisja A.\n", getpid());
    waitSemaphore(semKomisjaAID, 0, 0);
    printf("%d - I am in the egzam room for komisja A.\n", getpid());

    msgID_A = attachMessageQueue(msg_FILENAME_A); // communicate with komisja A.
    inEgzam = 1;
    signalSemaphore(semKomisjaAID, 1, 1);
    float finalGrade = egzamin(msgID_A, poprawia);
    waitSemaphore(semKomisjaAID, 2, 0);
    waitSemaphore(semKomisjaAID, 1, 0);
    signalSemaphore(semKomisjaAID, 2, 1);
    inEgzam = 0;
    printf("%d - I have left the egzam room for komisja A.\n", getpid());

    if (finalGrade == 2.0) { // Student failed.
        colorPrintf(RED, "%d - Student - Nie zdalem. \x1b[0m \n", getpid());
        exit(0);
    }

    colorPrintf(YELLOW, "%d - Student - Zdalem. \x1b[0m \n", getpid());

    if (exitSignal == 1) {
        exit(0);
    }
    int semKomisjaBID = allocSemaphore(sem_KomisjaB, 3, IPC_CREAT | 0666);

    printf("%d - I am in the waiting room for komisja B.\n", getpid());
    waitSemaphore(semKomisjaBID, 0, 0);
    printf("%d - I am in the egzam room for komisja B.\n", getpid());

    msgID_B = attachMessageQueue(msg_FILENAME_B); // communicate with komisja B
    inEgzam = 1;
    signalSemaphore(semKomisjaBID, 1, 1);
    finalGrade = egzamin(msgID_B, 0);
    waitSemaphore(semKomisjaBID, 2, 0);
    waitSemaphore(semKomisjaBID, 1, 0);
    signalSemaphore(semKomisjaBID, 2, 1);
    inEgzam = 0;
    printf("%d - I have left the egzam room for komisja B.\n", getpid());

    if (finalGrade == 2.0) { // Student failed.
        colorPrintf(RED, "%d - Student - Nie zdalem. \x1b[0m \n", getpid());
        
        exit(0);
    }
    colorPrintf(YELLOW, "%d - Student - Zdalem. \x1b[0m \n", getpid());
    if (exitSignal == 1) {
        exit(0);
    }

    return 0;
}

int egzamin(int msgID, float poprawia) {
    // send hello
    struct egzamHello hello[3];
    //unsigned long int threads[3] = {0};
    for (int i = 0; i < 3; i++) {
        hello[i].messageType = 1 + i;
        hello[i].studentID = getpid();
        hello[i].codeForQuestion = 0;
        hello[i].threadID = 0;
        hello[i].poprawia = poprawia;
        sendMessageQueue(msgID, &hello[i], sizeof(struct egzamHello) - sizeof(hello[i].messageType), 0);
        colorPrintf(BLUE, "Uczen Sent Hello: %ld, %d, %d, %ld \x1b[0m\n", hello[i].messageType, hello[i].studentID, hello[i].codeForQuestion, hello[i].threadID);
    }

    if (poprawia != 0) {
        struct egzamFinalGrade finalGrade;
        receiveMessageQueue(msgID, (void*)&finalGrade, sizeof(struct egzamFinalGrade) - sizeof(finalGrade.messageType), 16, 0);
        colorPrintf(BLUE, "Uczen Receive the Final Grade: %ld, %f \x1b[0m\n", finalGrade.messageType, finalGrade.finalGrade);

        return finalGrade.finalGrade;
    }


    // receive hello
    for (int i = 0; i < 3; i++) {
        receiveMessageQueue(msgID, &hello[i], sizeof(struct egzamHello) - sizeof(hello[i].messageType), getpid(), i);
        colorPrintf(BLUE, "Uczen Receive Hello: %ld, %d, %d, %ld \x1b[0m\n", hello[i].messageType, hello[i].studentID, hello[i].codeForQuestion, hello[i].threadID);
        //   threads[i] = hello[i].threadID;
    }

    // receive question
    struct egzamQuestion question[3]; 
    for (int i = 0; i < 3; i++) {
        receiveMessageQueue(msgID, (void*)&question[i], sizeof(struct egzamQuestion) - sizeof(question[i].messageType), hello[i].codeForQuestion, i);
        colorPrintf(BLUE, "Uczen Receive a Question: %ld, %ld, %d, %d \x1b[0m\n", question[i].messageType, question[i].threadID, question[i].codeForAnswer, question[i].question);
    }

    // send question
    struct egzamAnswer answer[3];
    for (int i = 0; i < 3; i++) {
        answer[i].messageType = question[i].codeForAnswer;
        answer[i].codeForGrade = question[i].codeForAnswer + 3;
        answer[i].answer = 6;
        sendMessageQueue(msgID, &answer[i], sizeof(struct egzamAnswer) - sizeof(answer[i].messageType), 0);
        colorPrintf(BLUE, "Uczen Sent an Answer: %ld, %d, %d \x1b[0m\n", answer[i].messageType, answer[i].codeForGrade, answer[i].answer);
    }

    // receive egzam grade 
    // if failed exit.
    struct egzamGrade grade[3];
    int finalGradeCode;
    for (int i = 0; i < 3; i++) {
        receiveMessageQueue(msgID, (void*)&grade[i], sizeof(struct egzamGrade) - sizeof(grade[i].messageType), answer[i].codeForGrade, i);
        colorPrintf(BLUE, "Uczen Receive the Grade: %ld, %ld, %.1f, %d \x1b[0m\n", grade[i].messageType, grade[i].threadID, grade[i].grade, grade[i].codeForFinalGrade);
    }
    finalGradeCode = grade[0].codeForFinalGrade;

    struct egzamFinalGrade finalGrade;
    receiveMessageQueue(msgID, (void*)&finalGrade, sizeof(struct egzamFinalGrade) - sizeof(finalGrade.messageType), finalGradeCode, 0);
    colorPrintf(BLUE, "Uczen Receive the Final Grade: %ld, %.1f \x1b[0m\n", finalGrade.messageType, finalGrade.finalGrade);

    return finalGrade.finalGrade;
}
