#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/proc_account.h"
#include "user/user.h"

void
busy_work(int loops)
{
  volatile int x = 0;

  for(int i = 0; i < loops; i++){
    x = x + (i % 7);
  }
}

void
touch_memory(char *mem, int bytes)
{
  for(int i = 0; i < bytes; i += 4096){
    mem[i] = 'A';
  }
}

int
main(int argc, char *argv[])
{
  int pid;
  int status;
  struct proc_account info;

  printf("Process Accounting Demo\n");
  printf("-----------------------\n");

  pid = fork();

  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    int bytes = 64 * 1024;

    char *mem = sbrk(bytes);

    if(mem == (char *)-1){
      printf("child: sbrk failed\n");
      exit(3);
    }

    touch_memory(mem, bytes);

    busy_work(60000000);

    exit(7);
  }

  int waited_pid = waitacct(&status, &info);

  if(waited_pid < 0){
    printf("waitacct failed\n");
    exit(1);
  }

  printf("child pid: %d\n", waited_pid);
  printf("process name: %s\n", info.name);
  printf("exit status from waitacct: %d\n", status);
  printf("exit status from accounting: %d\n", info.exit_status);
  printf("cpu ticks: %d\n", (int)info.cpu_ticks);
  printf("current memory bytes: %d\n", (int)info.mem_bytes);
  printf("maximum memory bytes: %d\n", (int)info.max_mem_bytes);
  printf("creation tick: %d\n", (int)info.creation_tick);
  printf("exit tick: %d\n", (int)info.exit_tick);
  printf("elapsed ticks: %d\n", (int)info.elapsed_ticks);
  printf("exited: %d\n", info.exited);

  if(status == 7 && info.exit_status == 7 && info.max_mem_bytes > 0){
    printf("PASS\n");
  } else {
    printf("FAIL\n");
  }

  exit(0);
}
