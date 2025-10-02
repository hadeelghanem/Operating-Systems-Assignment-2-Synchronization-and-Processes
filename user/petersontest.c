#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//helper function to print to console with lock protection
void console_print(int console_lock, int role, char *prefix, int pid, int r, int iter) {
  // Acquire console lock
  if (peterson_acquire(console_lock, role) < 0) {
    printf("Failed to acquire console lock\n");
    exit(1);
  }
  
  //print message atomically
  printf("%s %d (role %d) %s, iteration %d\n", 
         prefix, pid, r, 
         (strcmp(prefix, "Process") == 0) ? "" : prefix, 
         iter);
  
  //release console lock
  if (peterson_release(console_lock, role) < 0) {
    printf("Failed to release console lock\n");
    exit(1);
  }
}

int
main(int argc, char *argv[])
{
  //create one lock for mutual exclusion test
  int lock_id = peterson_create();
  if (lock_id < 0) {
    printf("Failed to create lock\n");
    exit(1);
  }
  
  //create another lock for console output synchronization
  int console_lock = peterson_create();
  if (console_lock < 0) {
    printf("Failed to create console lock\n");
    exit(1);
  }
  
  printf("Peterson Lock Test\n");
  printf("Created mutual exclusion lock with ID: %d\n", lock_id);
  printf("Created console lock with ID: %d\n", console_lock);
  
  int fork_ret = fork();
  if (fork_ret < 0) {
    printf("Fork failed\n");
    exit(1);
  }
  
  int role = fork_ret > 0 ? 0 : 1;
  int pid = getpid();
  
  for (int i = 0; i < 5; i++) {
    //acquire console lock and print attempt message
    if (peterson_acquire(console_lock, role) < 0) {
      printf("Failed to acquire console lock\n");
      exit(1);
    }
    printf("Process %d (role %d) attempting to enter critical section, iteration %d\n", 
           pid, role, i);
    if (peterson_release(console_lock, role) < 0) {
      printf("Failed to release console lock\n");
      exit(1);
    }
           
    if (peterson_acquire(lock_id, role) < 0) {
      if (peterson_acquire(console_lock, role) < 0) {
        printf("Failed to acquire console lock\n");
        exit(1);
      }
      printf("Process %d (role %d) failed to acquire lock\n", pid, role);
      if (peterson_release(console_lock, role) < 0) {
        printf("Failed to release console lock\n");
        exit(1);
      }
      exit(1);
    }
    
    //critical section start
    if (peterson_acquire(console_lock, role) < 0) {
      printf("Failed to acquire console lock\n");
      exit(1);
    }
    printf("Process %d (role %d) ENTERED critical section, iteration %d\n", 
           pid, role, i);
    if (peterson_release(console_lock, role) < 0) {
      printf("Failed to release console lock\n");
      exit(1);
    }
    
    //sleep for a longer time to make sure we can observe mutual exclusion
    sleep(5);
    
    if (peterson_acquire(console_lock, role) < 0) {
      printf("Failed to acquire console lock\n");
      exit(1);
    }
    printf("Process %d (role %d) LEAVING critical section, iteration %d\n", 
           pid, role, i);
    if (peterson_release(console_lock, role) < 0) {
      printf("Failed to release console lock\n");
      exit(1);
    }
    //critical section end
    
    if (peterson_release(lock_id, role) < 0) {
      if (peterson_acquire(console_lock, role) < 0) {
        printf("Failed to acquire console lock\n");
        exit(1);
      }
      printf("Process %d (role %d) failed to release lock\n", pid, role);
      if (peterson_release(console_lock, role) < 0) {
        printf("Failed to release console lock\n");
        exit(1);
      }
      exit(1);
    }
    
    //random delay between attempts
    sleep(role + 1); //different sleep times to create more interleaving
  }
  
  if (fork_ret > 0) {
    wait(0);
    
    if (peterson_acquire(console_lock, role) < 0) {
      printf("Failed to acquire console lock\n");
      exit(1);
    }
    printf("Parent process destroying locks\n");
    if (peterson_release(console_lock, role) < 0) {
      printf("Failed to release console lock\n");
      exit(1);
    }
    
    if (peterson_destroy(lock_id) < 0) {
      if (peterson_acquire(console_lock, role) < 0) {
        printf("Failed to acquire console lock\n");
        exit(1);
      }
      printf("Failed to destroy mutual exclusion lock\n");
      if (peterson_release(console_lock, role) < 0) {
        printf("Failed to release console lock\n");
        exit(1);
      }
      exit(1);
    }
    
    if (peterson_destroy(console_lock) < 0) {
      printf("Failed to destroy console lock\n");
      exit(1);
    }
  }
  
  exit(0);
}