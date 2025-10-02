#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define MAX_PROC 16
#define MAX_LOCKS (MAX_PROC - 1) //max 15 lock

 int cur_proc;  //index of the current process
 int num_proc;
 int num_levels;
 int tournament_locks[MAX_LOCKS];   

//Helper funcion  to calculate log_2(n)
static int log_2(int n) {
    int L = 0;
    while (n > 1) {
        n /= 2;
        L++;
    }
    return L;
}

int tournament_create(int processes) {
    if (processes <= 0 || processes > MAX_PROC || (processes & (processes - 1)) != 0){
         printf("Invalid number of processes\n");
        return -1;
        }

    num_levels = log_2(processes);
    num_proc = processes;
 for (int i = 0; i < (processes - 1); i++) {
        tournament_locks[i] = peterson_create();
        if (tournament_locks[i] < 0){
            printf("Failed to create lock %d\n", i);
            return -1;
        }
    }
 cur_proc = 0;
 for (int i = 1; i < processes; i++) {
        int pid = fork();
        if (pid < 0){
            printf("fork failed\n");
            return -1;
        }
        if (pid == 0) {
            cur_proc = i;
            break;
        }
    }
    return cur_proc;
}

int tournament_acquire(void) {
    int index = cur_proc;
    for (int l = num_levels - 1; l >= 0; l--) {
        int role = (index & (1 << (num_levels - l - 1))) >> (num_levels - l - 1);
        int lock_l = (index >> (num_levels - l)) + (1 << l) - 1;
        if (peterson_acquire(tournament_locks[lock_l], role) < 0){
            printf("Failed to acquire lock at level %d\n", l);
            return -1;
    }
}
    return 0;
}

int tournament_release(void) {
    int index = cur_proc;
    for (int l = 0; l < num_levels; l++) {
        int role = (index & (1 << (num_levels - l - 1))) >> (num_levels - l - 1);
        int lock_l = (index >> (num_levels - l)) + (1 << l) - 1;
        if (peterson_release(tournament_locks[lock_l], role) < 0){
            printf("Failed to release lock at level %d\n", l);
            return -1;
    }
}
    return 0;
}
 