#ifndef __my_semaphore_h__
#define __my_semaphore_h__

int allocSemaphore(char* filename, int number, int flags);
int destroySemaphore(int semID, int number);
void initSemaphore(int semID, int number, int val);
int waitSemaphore(int semID, int number, int flags);
void signalSemaphore(int semID, int number);
int valueSemaphore(int semID, int number);


#endif // __my_semaphore_h__
