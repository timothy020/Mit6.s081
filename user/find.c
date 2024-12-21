/*
    1. 在当前目录下查找
    2. 在子目录下查找
    3. 递归来做，注意"."和".."
 */

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
  int len = strlen(p);
  if(len >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  buf[len] = '\0';
  return buf;
}

void find(char* path, char* file_name)
{
  // printf("path = %s, file_name = %s\n", path, file_name);

  int fd;
  struct stat st;
  char buf[512], *p;
  struct dirent de;

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
    case T_FILE:
      // printf("fmtname = %s\n", fmtname(path));
      if(strcmp(fmtname(path), file_name) == 0) {
          printf("%s\n",path);
      }
      break;

    case T_DIR:
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        // printf("find: path too long\n");
        break;
      }
      int len = strlen(path);
      // 检查最后一个字符是否是 '/'
      if (path[len - 1] == '/') {
          path[len - 1] = '\0'; // 去掉末尾的 '/'
      }
      strcpy(buf, path);
      p = buf+strlen(buf);
      *p++ = '/';

      // 遍历目录中的每个文件
      while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;  // 跳过无效的目录项

        // 构造新的路径
        if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
          continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;  // 确保字符串结束
        char* new_argv[] = {buf, file_name};
        find(buf, file_name);  // 递归调用
      }
      break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 3){
    fprintf(2, "Usage: find dirctory file_name\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}