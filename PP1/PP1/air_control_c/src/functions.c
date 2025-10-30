#include <sys/shm.h>

int planes = 0;
int takeoffs = 0;
int total_takeoffs = 0;
#define SHM_NAME "/air_control_shm"

int* data;

void MemoryCreate() {
  // TODO2: create the shared memory segment, configure it and store the PID of
  // the process in the first position of the memory block.
  int shm_id = shm_open(SHM_NAME, O_CREATE | O_RDWR, 0666);
  if (shm_id < 0) {
    perror("shmget failed");
    exit(1);
  }

  ftruncate(shm_id, sizeof(int) * 10);
  data = mmap(NULL, 10 * sizeof(double), 
  PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0); // Adjust size as needed

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

void* TakeOffsFunction() {
  // TODO: implement the logic to control a takeoff thread
  //    Use a loop that runs while total_takeoffs < TOTAL_TAKEOFFS
  //    Use runway1_lock or runway2_lock to simulate a locked runway
  //    Use state_lock for safe access to shared variables (planes,
  //    takeoffs, total_takeoffs)
  //    Simulate the time a takeoff takes with sleep(1)
  //    Send SIGUSR1 every 5 local takeoffs
  //    Send SIGTERM when the total takeoffs target is reached

  while( total_takeoffs < TOTAL_TAKEOFFS) {
    


  }
}