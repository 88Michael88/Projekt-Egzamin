#ifndef __my_shared_memory_h__
#define __my_shared_memory_h__

#include <stdbool.h>

char* attachMemoryBlock(char* filename, int size);
bool detachMemoryBlock(char* block);
bool destroyMemoryBlock(char* filename);

#endif // __my_shared_memory_h__
