#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include "semaphore.h"
#include "egzamStructures.h"
#include "sharedMemory.h"
#include "messageQueue.h"
#include "const.h"

int egzamin(int msgID);

int main() {
    srand(time(NULL) + getpid());

    //int wydzial = 1;
    int kierunek = rand() % num_studies;
    //int retakeEgzam = !((rand() % 100) % 20); // I get a range from 0 to 99, then I devide this range into 0 to 19, because one number is now 5% of the set of numbers, so if the number is 0 then the uczen is a retake student. (I could just do % 20);
                                              
    int semID = allocSemaphore(sem_FILENAME, 1, IPC_CREAT | 0666);

    waitSemaphore(semID, 0, 0); // Wait until the the Kierunek is chosen.
    int* block = (int*)attachMemoryBlock(shm_FILENAME, shm_SIZE);

    if (!(block[0] == kierunek)){
        detachMemoryBlock((void*)block);
        exit(0);
    } 
    
    int semIDKomA = allocSemaphore(sem_KomisjaA, 2, IPC_CREAT | 0666);
    signalSemaphore(semIDKomA, 1, 1); // signal -> Student is ready.
    waitSemaphore(semIDKomA, 0, 0); // wait until Komisja A is ready. 
    printf("%d - Student - Jestem w komisji A\n", getpid());

    int msgID_A = attachMessageQueue(msg_FILENAME_A); // communicate with komisja A
    float finalGrade = egzamin(msgID_A);

    if (finalGrade == 2.0) { // Student failed.
        printf("%d - Student - Nie zdalem.");
        exit(0);
    }
    printf("%d - Student - Zdalem.");

    detachMemoryBlock((void*)block);

    int semIDKomB = allocSemaphore(sem_KomisjaB, 2, IPC_CREAT | 0666);
    signalSemaphore(semIDKomB, 1, 1); // signal -> Student is ready.
    waitSemaphore(semIDKomB, 0, 0); // wait until komisja B is ready.
    printf("%d - Student - Jestem w komisji B\n", getpid());

    int msgID_B = attachMessageQueue(msg_FILENAME_B); // communicate with komisja B
    finalGrade = egzamin(msgID_B);


    printf("%d I have finished B\n", getpid());
    return 0;
}

int egzamin(int msgID) {
    // send hello
    struct egzamHello hello[3];
    //unsigned long int threads[3] = {0};
    for (int i = 0; i < 3; i++) {
        hello[i].messageType = i+1;
        hello[i].studentID = getpid();
        hello[i].codeForQuestion = 0;
        hello[i].threadID = 0;
        sendMessageQueue(msgID, &hello[i], sizeof(struct egzamHello) - sizeof(hello[i].messageType), 0);
    }
    
    // receive hello
    for (int i = 0; i < 3; i++) {
        receiveMessageQueue(msgID, &hello[i], sizeof(struct egzamHello) - sizeof(hello[i].messageType), getpid(), 0);
     //   threads[i] = hello[i].threadID;
    }
    //printf("%d I will send to: %lu, %lu, %lu.\n", getpid(), threads[0], threads[1], threads[2]);

    // receive egzam question
    struct egzamQuestion question[3]; 
    for (int i = 0; i < 3; i++) {
        receiveMessageQueue(msgID, (void*)&question[i], sizeof(struct egzamQuestion) - sizeof(question[i].messageType), hello[i].codeForQuestion, 0);
    }
    //printf("Received all questions: %d, %d, %d\n", question[0].question, question[1].question, question[2].question);

    // send egzam answer
    sleep(2); // Try to remove this.
    struct egzamAnswer answer[3];

    for (int i = 0; i < 3; i++) {
        answer[i].messageType = question[i].codeForAnswer;
        answer[i].codeForGrade = 10 + i; 
        answer[i].answer = 6;
        sendMessageQueue(msgID, &answer[i], sizeof(struct egzamAnswer) - sizeof(answer[0].messageType), 0);
    }

    // receive egzam grade 
    // if failed exit.
    struct egzamGrade grade[3];
    int finalGradeCode;
    for (int i = 0; i < 3; i++) {
        receiveMessageQueue(msgID, (void*)&grade[i], sizeof(struct egzamGrade) - sizeof(grade[i].messageType), answer[i].codeForGrade, 0);
        //printf("Received Grade %.1f\n", grade[i].grade);
        if (grade[i].codeForFinalGrade != 0) {
            finalGradeCode = grade[i].codeForFinalGrade;
        }
    }
    
    struct egzamFinalGrade finalGrade;
    receiveMessageQueue(msgID, (void*)&finalGrade, sizeof(struct egzamFinalGrade) - sizeof(finalGrade.messageType), finalGradeCode, 0);

    return finalGrade.finalGrade;
}
