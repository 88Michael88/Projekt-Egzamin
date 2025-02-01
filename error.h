#ifndef __my_error_h__
#define __my_error_h__

char* errors[] = {
    "Error: There was an error when allocating memory.",
    "Error: There was an error when forking."
};

#define MEMORY_ALLOCATION 0
#define FORK 1


#endif // __my_error_h__
