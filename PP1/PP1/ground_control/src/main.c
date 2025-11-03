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
#include <sys/time.h>

#define PLANES_LIMIT 20
int planes = 0;
int takeoffs = 0;
int traffic = 0;

#define SH_MEMORY_NAME "/air_control_shm"
pid_t* data;
int shm_id;

void Traffic(int signum) {
  // TODO:
  // Calculate the number of waiting planes.
  // Check if there are 10 or more waiting planes to send a signal and increment
  // planes. Ensure signals are sent and planes are incremented only if the
  // total number of planes has not been exceeded.
  if (planes >= 10) {
    printf("RUNWAY OVERLOADED\n");
  }
  if(planes < PLANES_LIMIT) {
    planes += 5;
    pid_t radio_pid = data[1];
    kill(radio_pid, SIGUSR2);
  }
}

void SigUSR1Handler(int signum) {
    takeoffs += 5; // increment takeoffs
}

void SigTERMHandler(int signum) {
    munmap(data, 10 * sizeof(int));
    close(shm_id);
    printf("finalization of operations...\n");
    _exit(0);
}


int main(int argc, char* argv[]) {
  // TODO:
  // 1. Open the shared memory block and store this process PID in position 2
  //    of the memory block.
  // 3. Configure SIGTERM and SIGUSR1 handlers
  //    - The SIGTERM handler should: close the shared memory, print
  //      "finalization of operations..." and terminate the program.
  //    - The SIGUSR1 handler should: increase the number of takeoffs by 5.
  // 2. Configure the timer to execute the Traffic function.

  //shm things
  int shm_id = shm_open(SH_MEMORY_NAME, O_RDWR, 0666);
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
  data[2] = getpid();

  // Set up SIGUSR1 and SIGTERM handlers
  struct sigaction sa;
  sa.sa_handler = SigUSR1Handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGUSR1, &sa, NULL);

  sa.sa_handler = SigTERMHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGTERM, &sa, NULL);


  struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 500 * 1000; // 500 ms
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 500 * 1000; // 500 ms
  

  if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
    perror("setitimer failed");
    exit(1);
  }


  //wait for signals
  while (1) {
    pause();
}
  return 0;
}