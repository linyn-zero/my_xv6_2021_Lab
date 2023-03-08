// pingpong
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]){
  int pp2c[2],pc2p[2];
  pipe(pp2c);
  pipe(pc2p);

  if(fork() == 0){
    close(pp2c[1]);
    close(pc2p[0]);
    char buf[16]; 
    if(-1 != read(pp2c[0], buf, sizeof(buf))){
      printf("%d: received ping\n", getpid());
    }
    write(pc2p[1], "hi, parent", 10);
    close(pp2c[1]);
    close(pc2p[0]);
  }
  else {
    close(pp2c[0]);
    close(pc2p[1]);
    char buf[16];
    write(pp2c[1],"hi, child", 9);
    if(-1 != read(pc2p[0], buf, sizeof(buf))){
      printf("%d: received pong\n", getpid());
    }
    close(pp2c[1]);
    close(pc2p[0]);
  }
  exit(0);
}