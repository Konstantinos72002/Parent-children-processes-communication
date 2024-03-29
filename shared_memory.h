#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <sys/shm.h>

#define MAX_LINE 300
#define MAX_SR 300

struct shared_memory{
    int request_segment; 
    char buffer[MAX_SR][MAX_LINE];    
    int finished;
};

typedef struct shared_memory* SharedMemory;

void child(int,int,int,int,int,int*,sem_t*,sem_t*,sem_t*,sem_t**,SharedMemory);