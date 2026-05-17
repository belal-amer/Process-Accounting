#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/proc_account.h"
#include "user/user.h"

void
busy_work(int loops)
{
  volatile int x = 0;

  for(int i = 0; i < loops; i++){
    x = x + (i % 11);
  }
}

void
touch_memory(char *mem, int bytes)
{
  for(int i = 0; i < bytes; i += 4096){
    mem[i] = 'B';
  }
}

void
run_case(char *case_name, int loops, int bytes, int exit_code)
{
  int pid;
  int status = -1;
  struct proc_account info;

  printf("\nRunning case: %s\n", case_name);

  pid = fork();

  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    char *mem = sbrk(bytes);

    if(mem == (char *)-1){
      printf("child: sbrk failed\n");
      exit(2);
    }

    touch_memory(mem, bytes);
    busy_work(loops);

    exit(exit_code);
  }

  int waited_pid = waitacct(&status, &info);

  if(waited_pid < 0){
    printf("waitacct failed\n");
    exit(1);
  }

  printf("pid: %d\n", waited_pid);
  printf("status: %d\n", status);
  printf("account exit status: %d\n", info.exit_status);
  printf("cpu ticks: %d\n", (int)info.cpu_ticks);
  printf("memory bytes: %d\n", (int)info.mem_bytes);
  printf("max memory bytes: %d\n", (int)info.max_mem_bytes);
  printf("elapsed ticks: %d\n", (int)info.elapsed_ticks);

  if(status == exit_code && info.exit_status == exit_code && info.max_mem_bytes > 0){
    printf("case result: PASS\n");
  } else {
    printf("case result: FAIL\n");
  }
}

int
main(int argc, char *argv[])
{
  printf("Process Accounting Tests\n");
  printf("========================\n");

  run_case("small cpu small memory", 10000000, 16 * 1024, 4);
  run_case("large cpu medium memory", 80000000, 32 * 1024, 5);
  run_case("medium cpu large memory", 40000000, 96 * 1024, 6);

  printf("\nAll accounting tests finished.\n");

  exit(0);
}
