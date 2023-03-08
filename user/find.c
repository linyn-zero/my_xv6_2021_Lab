#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
  return buf;
}

void find(char* path, char* target){
  // printf("OK1\n");
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  // 当前目录是文件,符合则打印
  case T_FILE:
  // printf("OK2-FILE\n");
    if(!strcmp(fmtname(path), target))
    {  
      printf("%s\n", path);
    }
    break;
  // 当前目录是文件夹
  case T_DIR:
    // printf("OK2-DIR\n");
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    // 符合则打印
    if(!strcmp(fmtname(path), target))
    {  
      printf("%s\n", path);
    }
    // buf is the path of this DIR
    strcpy(buf, path);
    p = buf+strlen(buf);  // p is a pointer of buf
    *p++ = '/';
    // printf("OK3-DIR\n");
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0) // empty dir
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      // printf("OK4-DIR: buf=%s fmtname(buf)=%s\n", buf,fmtname(buf));
      if(!strcmp(fmtname(buf), ".") || !strcmp(fmtname(buf), "..")){
        // printf("continue\n");
        continue;
      }
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, target);
      // printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char* argv[]){
  int i;
  // 打印全部文件
  if(argc == 1){
    printf("shoula print all file\n");
  }
  // 目录默认为当前
  else if(argc == 2){
    find(".",argv[1]);
  }
  else {
    for (i = 2; i < argc; i++)
      find(argv[1], argv[i]);
  }
  exit(0);
}