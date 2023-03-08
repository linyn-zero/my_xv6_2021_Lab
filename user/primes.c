#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


void
child_filter(int pre_pipe[2]){
  int prime = 0;
  if(-1 != read(pre_pipe[0], &prime, sizeof(prime))){
    if(prime == -1) exit(0);
    printf("prime %d\n",prime);
  }

  int next_pipe[2];
  pipe(next_pipe);
  if(fork() == 0){
    close(next_pipe[1]);
    close(pre_pipe[0]);
    child_filter(next_pipe);
    exit(0);
  }
  else {
    close(next_pipe[0]);
    int n = 0;
    // 第二个条件很关键，缺少则会循环中的read悬挂
    while(-1 != read(pre_pipe[0], &n, sizeof(n)) && -1 != n){
      if(n % prime){
        write(next_pipe[1], &n, sizeof(n));
      }
    }
    close(pre_pipe[0]);
    n = -1;
    write(next_pipe[1], &n, sizeof(n));
    close(next_pipe[1]);
    wait(0);
  }
  exit(0);
  
}

int 
main(int argc, char* argv[]){
  int pipe_first[2];
  pipe(pipe_first);

  // fork之后马上close不需要的管道fid
  if(fork() == 0){
    close(pipe_first[1]);
    child_filter(pipe_first);
    exit(0);
  }
  else {
    close(pipe_first[0]);
    int n = 2;
    for (; n <=35; n++)
    {
      write(pipe_first[1], &n, sizeof(n));
    }
    n = -1;
    write(pipe_first[1], &n, sizeof(n));
    // 用完后立马close管道fid
    close(pipe_first[1]);
    wait(0);
  }
  exit(0);
}