// xargs.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "xargs.h"

void run(char* program, char* argv[]){
  if(fork() == 0){
    exec(program, argv); // argv[0] 为 program 名
    exit(0);
  }
  return ;
}

// 难点在于 buf ，既要 argv ，又要 read
// 将读到的参数全部放入 char*[] ,每得到换行就交给 exec 执行
int main(int argc, char* argv[]){
  char buf[2048]; // 
  char *p = buf, *head_p = buf;
  char *arg_buf[128]; // 参数 buf ，前半部分是 argv 参数，后半部分不断覆写为 stdin 参数
  char **gap_arg = arg_buf; // 指向第一个从 stdin 读入的参数
  
  // 将 argv 参数放入 arg_buf
  for (int i = 1; i < argc; i++)
  {
    *(gap_arg++) = argv[i];   
  }
  char** pa = gap_arg; // arg_buf 后半部分的干活的索引
  while(read(0, p, 1) != 0){
    if(*p == ' ' || *p == '\n'){
      *p = '\0';
      *(pa++) = head_p;
      head_p = p+1;

      // 遇到换行，执行一次  
      if(*p == '\n'){
        *pa = 0; // 标识 arg_buf 结束
        run(argv[1], arg_buf);
        pa = gap_arg; // 重置后半部分指针到后半部分开头
      }
    }
    p++;
  }

  if(pa != gap_arg){ // 如果最后一行不是空行
    *p = '\0';
    *(pa++) = head_p;
    *pa = 0;
    run(argv[1], arg_buf); 
  }
  
  while ((wait(0) != -1)); // wait 失败则返回1
  exit(0);
}