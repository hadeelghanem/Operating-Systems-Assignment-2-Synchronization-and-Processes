#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_power_of_two(int n) {     //trick to check if a number has only one bit set in binary
  return n > 0 && (n & (n - 1)) == 0;
}


int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Usage: tournament <number_of_processes>\n");
    exit(1);
  }

  int n = atoi(argv[1]);
  if (!is_power_of_two(n) || n > 16) {
    printf("Number of processes must be a power of 2 and <= 16\n");
    exit(1);
  }

  int tid = tournament_create(n); 
  if (tid < 0) {
    printf("Failed to create tournament\n");
    exit(1);
  }

  for (int i = 0; i < 5; i++) {   // Print the 4 iterations
    if (tournament_acquire() < 0) {
      printf("Failed to acquire tournament lock\n");
      exit(1);
    }

    printf("[Process %d | Tournament ID %d] in critical section, iteration %d\n", getpid(), tid, i);

    if (tournament_release() < 0) {
      printf("Failed to release tournament lock\n");
      exit(1);
    }
  }
  // At the end the parent waits for all child processes to exit 
  if (tid == 0) {
    for (int i = 1; i < n; i++) {
      wait(0);
    }
  }

  exit(0);
}