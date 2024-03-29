
#include "shared_memory.h"

// Children code

void child(int number_of_requests,int num_of_segments,int N,int s_r,int id, int* ready_children,
sem_t* request_parent,sem_t* child_ready,sem_t* parent_answer,sem_t** segment_semaphores,
SharedMemory sm) {

    char buffer[120];
    snprintf(buffer, sizeof(buffer), "%s%d", "child", id);
    FILE *f = fopen(buffer, "w");

    // Number of requests
    int k = 0;
    // clock for request and for answer
    clock_t request;
    clock_t answer;
    while (k<number_of_requests) {
        
        int a,b; // a = desired segment , b = desired line
        if (k == 0) {
            srand(time(NULL) ^ (getpid()<<16));
            a = rand()%num_of_segments;
            
        } else {
            int p = rand()%10 + 1; // p = propability
            if (p > 7) a = rand()%num_of_segments;
        }
        b = rand()%s_r;
        
        // clock starting when child find her line                
        request = clock();

        // wait all children in segment semaphore (not the first time (sem_val = 0))
        if(sem_wait(segment_semaphores[a]) < 0) {
            fprintf(stderr, "error in wait semaphore %d", a);
            exit(1);
        }

        // children of segment a is ready
        ready_children[a]++;

        // FiFo logic.
        if(ready_children[a] == 1) {

            if(sem_wait(request_parent) < 0) {
                perror("sem_wait failed on child");
                exit(1);
            }

            // Upload request segment to shared memory
            sm->request_segment = a;

            // request ends
            request = clock() - request;

            // clock starting when child make request
            answer = clock();

            // child ready
            if(sem_post(child_ready) < 0){
                perror("sem_post child_ready failed on child");
                exit(1);
            }

            // wait until parent write to shared memory    
            if(sem_wait(parent_answer) < 0) {
                perror("sem_wait parent_answer failed on child");
                exit(1);
            }
        }
        
        // Enter to reading section
        if(sem_post(segment_semaphores[a]) < 0){
            fprintf(stderr, "error in post semaphore %d", a);
            exit(1);
        }
        
        
        // Reading section
        if(a == sm->request_segment) {

            // child reading requesting line
            char reading_line[MAX_LINE];
            strcpy(reading_line,sm->buffer[b]);
            
            // child finished one request
            k++;
            
            // child get the answer
            answer = clock() - answer;

            fprintf(f,"%ld %ld <%d,%d>\n %s \n",answer,request,a,b,reading_line);
        }

        // Leaving reading section
        if(sem_wait(segment_semaphores[a]) < 0) {
            fprintf(stderr, "error in wait semaphore %d", a);
            exit(1);
        }
        
        // Child of segment a finished
        ready_children[a]--;

        // Change segment
        if(ready_children[a] == 0) {
            
            // Change segment
            if(sem_post(request_parent) < 0){
                perror("sem post request_parent failed on child");
                exit(1);
            }
        }

        // Next child to read
        if(sem_post(segment_semaphores[a]) < 0) {
            fprintf(stderr, "error in post semaphore %d", a);
            exit(1);
        }
    }
    
    // This child finished his requests
    sm->finished++;
    
    // Conclusion to finished the parent programm
    if(sm->finished == N) {
        if(sem_post(child_ready) < 0) {
            perror("error in sem post child ready in child");
            exit(1);
        }
    } 
    // Close all semaphorses
    if(sem_close(request_parent) < 0 ) {
        perror("request parent sem_close error");
        exit(1);
    }    
    if(sem_close(parent_answer) < 0 ) {
        perror("request parent sem_close error");
        exit(1);
    }    
    if(sem_close(child_ready) < 0 ) {
        perror("request parent sem_close error");
        exit(1);
    }    
    for(int i = 0; i < num_of_segments;i++) {
        if(sem_close(segment_semaphores[i]) < 0 ) {
        perror("segment semaphoreses sem_close error");
        exit(1);
        }    
    }
}