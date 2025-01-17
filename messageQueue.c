#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/errno.h>
#include <stddef.h>
#include <stdio.h>
#include "messageQueue.h"

#define IPC_RESULT_ERROR (-1)

static int createMessageQueue(char* filename) {
    key_t key;

    if ((key = ftok(filename, 2)) == IPC_RESULT_ERROR) {
        perror("Error createMessageQueue :");
        return IPC_RESULT_ERROR;
    }

    return msgget(key, 0666 | IPC_CREAT);
}

int attachMessageQueue(char* filename) {
    int messageQueue; 

    if ((messageQueue = createMessageQueue(filename)) == IPC_RESULT_ERROR) {
        perror("Error attachMessageQueue :");
        return IPC_RESULT_ERROR;
    }

    return messageQueue;
}

int sendMessageQueue(int msgID, const void* msgp, size_t msgsize, int flags) {
    int result;
    if ((result = msgsnd(msgID, msgp, msgsize, flags)) == IPC_RESULT_ERROR) {
        perror("Error sendMessageQueue :");
        return IPC_RESULT_ERROR;
    }

    return result;
}

int receiveMessageQueue(int msgID, void* msgp, size_t mtextsize, long msgtype, int flags) {
    int result;
    if ((result = msgrcv(msgID, msgp, mtextsize, msgtype, flags)) == IPC_RESULT_ERROR) {
        perror("Error receiveMessageQueue :");
        return IPC_RESULT_ERROR;
    }

    return result;
}

int deleteMessageQueue(int msgID) {
    int result;
    if ((result = msgctl(msgID, IPC_RMID, NULL)) == IPC_RESULT_ERROR) {
        perror("Error deleteMessageQueue :");
        return IPC_RESULT_ERROR;
    }

    return result;
}
