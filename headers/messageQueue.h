#ifndef __message_queue_h__
#define __message_queue_h__

int attachMessageQueue(char* filename);
int sendMessageQueue(int msgID, const void* msgp, size_t msgsize, int flags);
int receiveMessageQueue(int msgID, void* msgp, size_t mtextsize, long msgtype, int flags);
int deleteMessageQueue(int msgID);

#endif // __message_queue_h__
