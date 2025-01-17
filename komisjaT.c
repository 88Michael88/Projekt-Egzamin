#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include "gradeMessage.h"
#include "messageQueue.h"
#include "const.h"

int main(int argc, char **argv) {
    printf("%d, %s\n", argc, argv[1]);
    
    char *filename = (char *)malloc(6);
    if (filename == NULL) {
        perror("malloc");
        return -1;
    }

    strcpy(filename, msg_FILENAME);
    strcat(filename, argv[1]);
    printf("Concat string: %s\n", filename);

    struct gradeMessage testMessage;
    testMessage.messageType = 2;
    testMessage.komisja = *argv[1];
    testMessage.studentID = 1111;
    testMessage.grade = 4.88;

    int msgID = attachMessageQueue(filename);

    sendMessageQueue(msgID, (void*)&testMessage, sizeof(testMessage) - sizeof(testMessage.messageType), 0);
    sleep(20);
    deleteMessageQueue(msgID);
    return 0;
}
