#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define NPETERSON 15  //number of Peterson locks (at least 15 as required)

struct peterson_lock petersons[NPETERSON];  //global array of Peterson locks

//initialize all Peterson locks
void
init_peterson(void)
{
  int i;
  for(i = 0; i < NPETERSON; i++){
    petersons[i].active = 0; //initially, all locks are inactive
    petersons[i].flag[0] = 0; //no process intends to enter critical section
    petersons[i].flag[1] = 0; //no process intends to enter critical section
    petersons[i].turn = 0; //arbitrary initial turn
  }
}

//create a new Peterson lock
int
peterson_create(void)
{
  int i;
  for(i = 0; i < NPETERSON; i++){
    //try to find an inactive lock
    if(__sync_lock_test_and_set(&petersons[i].active, 1) == 0){
      //we've found an inactive lock and marked it as active
      //initialize the lock state
      petersons[i].flag[0] = 0;
      petersons[i].flag[1] = 0;
      petersons[i].turn = 0;
      __sync_synchronize(); //memory barrier to ensure visibility
      return i;  //return the lock ID
    }
  }
  return -1;  //no available locks
}

//acquire a Peterson lock
int
peterson_acquire(int lock_id, int role)
{
  //check if lock_id and role are valid
  if(lock_id < 0 || lock_id >= NPETERSON || role < 0 || role > 1)
    return -1;
  
  //check if the lock is active
  if(petersons[lock_id].active == 0)
    return -1;
  
  int other = 1 - role; //the other process role
  
  //declare intent to enter critical section
  petersons[lock_id].flag[role] = 1;
  __sync_synchronize(); //memory barrier after setting flag
  
  //give priority to the other process
  petersons[lock_id].turn = other;
  __sync_synchronize(); //memory barrier after setting turn
  
  //wait until either the other process doesn't want to enter or it's our turn
  while(1) {
    __sync_synchronize(); // Memory barrier before reading shared state
    
    //check if we can proceed (other process is not competing or it's our turn)
    if(petersons[lock_id].flag[other] == 0 || petersons[lock_id].turn == role)
      break;
      
    yield(); //yield CPU to other processes instead of busy-waiting
  }
  
  return 0; //lock acquired
}

//release a Peterson lock
int
peterson_release(int lock_id, int role)
{
  //check if lock_id and role are valid
  if(lock_id < 0 || lock_id >= NPETERSON || role < 0 || role > 1)
    return -1;
  
  //check if the lock is active
  if(petersons[lock_id].active == 0)
    return -1;
  
  //release the lock by setting flag to 0
  __sync_synchronize(); //memory barrier before modifying shared state
  petersons[lock_id].flag[role] = 0;
  __sync_synchronize(); //memory barrier after modifying shared state
  
  return 0; //lock released
}

//destroy a Peterson lock
int
peterson_destroy(int lock_id)
{
  //check if lock_id is valid
  if(lock_id < 0 || lock_id >= NPETERSON)
    return -1;
  
  //check if the lock is active
  if(petersons[lock_id].active == 0)
    return -1;
  
  //mark the lock as inactive
  __sync_lock_release(&petersons[lock_id].active); //set active to 0
  
  __sync_synchronize(); //memory barrier to ensure visibility
  
  return 0; //lock destroyed
}