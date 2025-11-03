#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <sys/types.h>
#include <pthread.h>


#define SH_MEMORY_NAME "/air_control_shm"

extern pid_t* data;
extern int planes;
extern int total_takeoffs;
extern int takeoffs;
extern int shm_id;

extern pthread_mutex_t runway1_lock;
extern pthread_mutex_t runway2_lock;
extern pthread_mutex_t state_lock;

void MemoryCreate(void);
void SigHandler2(int signal);
void* TakeOffsFunction(void* arg);

#endif  // FUNCTIONS_H
