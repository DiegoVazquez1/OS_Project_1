// functions.c
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include "functions.h"


int TOTAL_TAKEOFFS = 20;
int planes = 0;
int takeoffs = 0;
int total_takeoffs = 0;
int* data = NULL;
int shm_id;

pthread_mutex_t runway1_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t runway2_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t state_lock   = PTHREAD_MUTEX_INITIALIZER;


void MemoryCreate() {
  // TODO2: create the shared memory segment, configure it and store the PID of 
  // the process in the first position of the memory block
    shm_id = shm_open(SH_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_id < 0) {
        perror("shm_open failed");
        exit(1);
    }

    ftruncate(shm_id, sizeof(int) * 10);

    data = mmap(NULL, 10 * sizeof(int),
                PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);

    if (data == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    data[0] = getpid(); 
}


void SigHandler2(int signal) {
  // TODO 5: implement the logic to increment the planes on the runway by 5
    planes += 5;
}


void* TakeOffsFunction(void* arg) {
   // TODO: implement the logic to control a takeoff thread
   // Use a loop that runs while total_takeoffs < TOTAL_TAKEOFFS 
   // Use runway1_lock or runway2_lock to simulate a locked runway 
   // Use state_lock for safe access to shared variables (planes, 
   // takeoffs, total_takeoffs) 
   // Simulate the time a takeoff takes with sleep(1) 
   // Send SIGUSR1 every 5 local takeoffs 
   // Send SIGTERM when the total takeoffs target is reached
    pthread_mutex_t* runway = NULL;

    while (total_takeoffs < TOTAL_TAKEOFFS) {
        
        while (!runway) {
            if (pthread_mutex_trylock(&runway1_lock) == 0) {
                runway = &runway1_lock;
            } else if (pthread_mutex_trylock(&runway2_lock) == 0) {
                runway = &runway2_lock;
            } else {
                usleep(1000);
            }
        }

        pthread_mutex_lock(&state_lock);

        if (planes > 0) {
            planes--;
            takeoffs++;
            total_takeoffs++;
            printf("Takeoff completed. Total takeoffs: %d\n", total_takeoffs);

            if (takeoffs % 5 == 0) {
                kill(data[1], SIGUSR1);
                takeoffs = 0;
            }
        }

        pthread_mutex_unlock(&state_lock);
        sleep(1); 
        pthread_mutex_unlock(runway);

        // runway = NULL; 
    }

    
    kill(data[1], SIGTERM);

    close(shm_id);
    munmap(data, sizeof(int) * 10);
    shm_unlink(SH_MEMORY_NAME);

    return NULL;
}
