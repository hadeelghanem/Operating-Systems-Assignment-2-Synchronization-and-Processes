# Operating Systems – Assignment 2: Synchronization and Processes

This repository contains my solution for **Assignment 2** of the Operating Systems course.  
It is based on **xv6-riscv** and extends the kernel and user space with synchronization primitives.

---

## Overview

### Task 1 – Peterson Locks (2 processes)
- Implemented kernel support for **Peterson locks** with the following system calls:
  - `int peterson_create(void)`
  - `void peterson_acquire(int lock_id, int role)`
  - `void peterson_release(int lock_id, int role)`
  - `int peterson_destroy(int lock_id)`
- Kernel maintains a table of locks (≥15).  
- Used `yield()` inside acquire to avoid busy waiting.  
- Ensured correctness with memory barriers:
  - `__sync_lock_test_and_set`
  - `__sync_lock_release`
  - `__sync_synchronize`
- Wrote a test program to show two processes alternating access to a critical section.

---

### Task 2 – Tournament Tree Locks (N processes)
- Designed a **user-level library** (`libtournament.c`) that generalizes Peterson’s lock to N processes (powers of 2, up to 16).  
- Internal representation: **binary tree in BFS order**; each internal node is a Peterson lock.  
- Each process climbs the tree, acquiring locks on its path to the root, then releases them on the way down.  
- Implemented library functions:
  - `tournament_create(int n)`
  - `tournament_acquire(int id)`
  - `tournament_release(int id)`
- Wrote a user program `tournament.c` that spawns N processes, synchronizing their access to the critical section.

---

## Skills Gained
- Kernel-level programming in xv6.  
- Implementing synchronization primitives beyond spinlocks.  
- Understanding **Peterson’s algorithm** for 2-process mutual exclusion.  
- Extending Peterson’s idea to multiple processes with a **tournament lock**.  
- Safe kernel-user space communication with atomic operations.  
- Writing and testing user-level synchronization libraries.  

---

## ▶️ How to Run

### Build xv6
```bash
make qemu

Inside xv6 shell:
peterson_test
