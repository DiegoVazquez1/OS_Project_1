#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include "functions.h"

#define SHM_NAME "/air_control_shm"
int main() {
  // TODO 1: Call the function that creates the shared memory segment.
  MemoryCreate();
  // TODO 3: Configure the SIGUSR2 signal to increment the planes on the runway
  // by 5.
  struct sigaction sa;
  sa.sa_handler = SigHandler2;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGUSR2, &sa, NULL) == -1) {
    perror("sigaction failed");
    exit(1);
  }

  // TODO 4: Launch the 'radio' executable and, once launched, store its PID in
  // the second position of the shared memory block.
  pid_t radio_pid = fork();
  if (radio_pid == 0) {
    execlp("./radio", "./radio", NULL);
    perror("execl failed");
    exit(1);
  }
  else {
    data[1] = radio_pid;
  }


  // TODO 6: Launch 5 threads which will be the controllers; each thread will
  // execute the TakeOffsFunction().
  pthread_t controllers[5];
  for (int i = 0; i < 5; i++) {
    if (pthread_create(&controllers[i], NULL, TakeOffsFunction, NULL) != 0) {
      perror("pthread_create failed");
      exit(1);
    }
  
}