#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include "const.h"
#include "./headers/semaphore.h"
#include "./headers/sharedMemory.h"
#include "./headers/egzamStructures.h"
#include "./headers/messageQueue.h"
#include "./headers/colorPrintf.h"
#include "./headers/ANSI.h"

int egzamin(int msgID);

int main(int argc, char* argv[]) {
    int kierunek = argc; // This is so that I don't a warning about not using all the variables.
    int poprawia = 0;
    
    sscanf(argv[0], "%d", &kierunek);
    sscanf(argv[1], "%d", &poprawia);


    // Add the semaphore so that you could go into the shared memory.
    int semSharedMemoryID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | 0666);

    waitSemaphore(semSharedMemoryID, 0, 0); // Wait until the the Kierunek is chosen.
    int* block = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);

    if (!(block[0] == kierunek)){
        detachMemoryBlock((void*)block);
        exit(0);
    } 
    
    //printf("%d - I am in! Kierunek %d, Poprawia %d\n", getpid(), kierunek, poprawia);

    int semKomisjaAID = allocSemaphore(sem_KomisjaA, 1, IPC_CREAT | 0666);

    printf("%d - I am in the waiting room for komisja A.\n", getpid());
    int value = valueSemaphore(semKomisjaAID, 0);
    printf("%d\n", value);
    waitSemaphore(semKomisjaAID, 0, 0);
    printf("%d - I am in the egzam room for komisja A.\n", getpid());

    int msgID_A = attachMessageQueue(msg_FILENAME_A); // communicate with komisja A
    float finalGrade = egzamin(msgID_A);

    if (finalGrade == 2.0) { // Student failed.
        colorPrintf(RED, "%d - Student - Nie zdalem. \x1b[0m \n", getpid());
        
 //       exit(0);
    }

    int semKomisjaBID = allocSemaphore(sem_KomisjaB, 1, IPC_CREAT | 0666);

    printf("%d - I am in the waiting room for komisja B.\n", getpid());
    waitSemaphore(semKomisjaBID, 0, 0);
    printf("%d - I am in the egzam room for komisja B.\n", getpid());

    int msgID_B = attachMessageQueue(msg_FILENAME_B); // communicate with komisja B
    finalGrade = egzamin(msgID_B);

    if (finalGrade == 2.0) { // Student failed.
        colorPrintf(RED, "%d - Student - Nie zdalem. \x1b[0m \n", getpid());
        
//        exit(0);
    }
    return 0;
}

int egzamin(int msgID) {
    (void)msgID;

    // send hello
    struct egzamHello hello[3];
    //unsigned long int threads[3] = {0};
    //for (int i = 0; i < 3; i++) {
    hello[0].messageType = 1;
    hello[0].studentID = getpid();
    hello[0].codeForQuestion = 0;
    hello[0].threadID = 0;
    sendMessageQueue(msgID, &hello[0], sizeof(struct egzamHello) - sizeof(hello[0].messageType), 0);
    colorPrintf(BLUE, "Uczen Sent Hello: %ld, %d, %d, %ld \x1b[0m\n", hello[0].messageType, hello[0].studentID, hello[0].codeForQuestion, hello[0].threadID);
    //}

    // receive hello
    // for (int i = 0; i < 3; i++) {
    receiveMessageQueue(msgID, &hello[0], sizeof(struct egzamHello) - sizeof(hello[0].messageType), getpid(), 0);
    colorPrintf(BLUE, "Uczen Receive Hello: %ld, %d, %d, %ld \x1b[0m\n", hello[0].messageType, hello[0].studentID, hello[0].codeForQuestion, hello[0].threadID);
    //   threads[i] = hello[i].threadID;
    // }

    // receive question
    struct egzamQuestion question[3]; 
    //for (int i = 0; i < 3; i++) {
    receiveMessageQueue(msgID, (void*)&question[0], sizeof(struct egzamQuestion) - sizeof(question[0].messageType), hello[0].codeForQuestion, 0);
    colorPrintf(BLUE, "Uczen Receive a Question: %ld, %ld, %d, %d \x1b[0m\n", question[0].messageType, question[0].threadID, question[0].codeForAnswer, question[0].question);
    // }

    // send question
    struct egzamAnswer answer[3];
    // for (int i = 0; i < 3; i++) {
    answer[0].messageType = question[0].codeForAnswer;
    answer[0].codeForGrade = 10 + 0; 
    answer[0].answer = 6;
    sendMessageQueue(msgID, &answer[0], sizeof(struct egzamAnswer) - sizeof(answer[0].messageType), 0);
    // }
    colorPrintf(BLUE, "Uczen Sent an Answer: %ld, %d, %d \x1b[0m\n", answer[0].messageType, answer[0].codeForGrade, answer[0].answer);

    // receive egzam grade 
    // if failed exit.
    struct egzamGrade grade[3];
    int finalGradeCode;
    // for (int i = 0; i < 3; i++) {
    receiveMessageQueue(msgID, (void*)&grade[0], sizeof(struct egzamGrade) - sizeof(grade[0].messageType), answer[0].codeForGrade, 0);
    colorPrintf(BLUE, "Uczen Receive the Grade: %ld, %ld, %f, %d \x1b[0m\n", grade[0].messageType, grade[0].threadID, grade[0].grade, grade[0].codeForFinalGrade);
    //printf("Received Grade %.1f\n", grade[i].grade);
    //    if (grade[i].codeForFinalGrade != 0) {
    finalGradeCode = grade[0].codeForFinalGrade;
    //    }
    // }
    
    struct egzamFinalGrade finalGrade;
    receiveMessageQueue(msgID, (void*)&finalGrade, sizeof(struct egzamFinalGrade) - sizeof(finalGrade.messageType), finalGradeCode, 0);
    colorPrintf(BLUE, "Uczen Receive the Final Grade: %ld, %f \x1b[0m\n", finalGrade.messageType, finalGrade.finalGrade);

    return finalGrade.finalGrade;
}
