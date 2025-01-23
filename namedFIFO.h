#ifndef __named_FIFO_h__
#define __named_FIFO_h__

int createFIFO(char* filename);
int openFIFOForWrite(char* filename);
int openFIFOForRead(char* filename);
int writeFIFO(int fileDiscriptor, void* data, int size);
int readFIFO(int fileDiscriptor, void* buf, int size);
void cleanupFIFO(char* filename);


#endif //__named_FIFO_h__
