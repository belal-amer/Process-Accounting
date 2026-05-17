#ifndef PROC_ACCOUNT_H
#define PROC_ACCOUNT_H

#include "types.h"

struct proc_account {
  int pid;
  int ppid;
  char name[16];

  uint64 cpu_ticks;

  uint64 mem_bytes;
  uint64 max_mem_bytes;

  uint64 creation_tick;
  uint64 exit_tick;
  uint64 elapsed_ticks;

  int exit_status;
  int exited;
};

#endif
