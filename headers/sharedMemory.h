#ifndef __my_shared_memory_h__
#define __my_shared_memory_h__

#include <stdbool.h>

void* attachMemoryBlock(char* filename, int size);
bool detachMemoryBlock(void* block);
bool destroyMemoryBlock(char* filename);

#endif // __my_shared_memory_h__
